/* Copyright (c) 2015, Mariusz Plucinski
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include "TialTestingExport.hpp"
#include "Thread.hpp"

#include <map>

#include <experimental/string_view>

#include <boost/lexical_cast.hpp>

#include "dtl/dtl.hpp"

#include <TialUtility/TialUtility.hpp>

#define TIAL_MODULE "Tial::Testing::Check"

namespace Tial {
namespace Testing {
namespace Check {

class TIALTESTING_EXPORT PointInfo {
	std::string _file;
	unsigned int _line;
	std::string _testCase;
public:
	PointInfo(
		const std::experimental::string_view &file,
		const unsigned int line,
		const std::experimental::string_view &testCase
	);
	const std::string &file() const;
	unsigned int line() const;
	const std::string &testCase() const;
};
TIALTESTING_EXPORT Utility::Logger::Stream &operator<<(Utility::Logger::Stream &s, const PointInfo &info);

namespace Exceptions {

class TIALTESTING_EXPORT CheckFailure {
public:
	typedef std::map<std::string, std::string> FailureInfo;
private:
	PointInfo _point;
	FailureInfo _info;
public:
	explicit CheckFailure(const PointInfo &point, const FailureInfo &info);
	const PointInfo &point() const;
	const FailureInfo &info() const;
};

class TIALTESTING_EXPORT CheckUnexpectedSucceeded: public CheckFailure {
public:
	using CheckFailure::CheckFailure;
};

}

enum class ExpressionOperation {
	Equal, NotEqual, LessOrEqual, Less, GreaterOrEqual, Greater
};

TIALTESTING_EXPORT void startTestSuite(const std::experimental::string_view &name, const bool printAll);
TIALTESTING_EXPORT bool printAll();
TIALTESTING_EXPORT void check(
	const PointInfo &point,
	const bool value,
	const std::function<Exceptions::CheckFailure::FailureInfo()> &buildFailureInfo
);
TIALTESTING_EXPORT void expectFail(const PointInfo &point);
TIALTESTING_EXPORT void fail(const PointInfo &point);
TIALTESTING_EXPORT void expression(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const bool value
);

template<typename T>
typename std::enable_if<Utility::TypeTraits::IsLeftShiftApplicable<std::ostream, const T>::value, std::string>::type
argToString(const T& v) {
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

template<typename T>
typename std::enable_if<!Utility::TypeTraits::IsLeftShiftApplicable<std::ostream, const T>::value, std::string>::type
argToString(const T&) {
	return "<cannot convert to string>";
}

template<ExpressionOperation operation, typename TL, typename TR>
class CheckExpressionImpl;

template<typename TL, typename TR>
class CheckExpressionImpl<ExpressionOperation::Equal, TL, TR> {
public:
	static bool check(const TL &argLeft, const TR &argRight) {
		return argLeft == argRight;
	}
};

template<typename TL, typename TR>
class CheckExpressionImpl<ExpressionOperation::NotEqual, TL, TR> {
public:
	static bool check(const TL &argLeft, const TR &argRight) {
		return argLeft != argRight;
	}
};

template<typename TL, typename TR>
class CheckExpressionImpl<ExpressionOperation::LessOrEqual, TL, TR> {
public:
	static bool check(const TL &argLeft, const TR &argRight) {
		return argLeft <= argRight;
	}
};

template<typename TL, typename TR>
class CheckExpressionImpl<ExpressionOperation::Less, TL, TR> {
public:
	static bool check(const TL &argLeft, const TR &argRight) {
		return argLeft < argRight;
	}
};

template<typename TL, typename TR>
class CheckExpressionImpl<ExpressionOperation::GreaterOrEqual, TL, TR> {
public:
	static bool check(const TL &argLeft, const TR &argRight) {
		return argLeft >= argRight;
	}
};

template<typename TL, typename TR>
class CheckExpressionImpl<ExpressionOperation::Greater, TL, TR> {
public:
	static bool check(const TL &argLeft, const TR &argRight) {
		return argLeft > argRight;
	}
};

template<ExpressionOperation operation, typename TL, typename TR>
bool checkExpression(const TL &argLeft, const TR &argRight) {
	return CheckExpressionImpl<operation, TL, TR>::check(argLeft, argRight);
}

template<ExpressionOperation operation, typename TL, typename TR> void expression(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const std::experimental::string_view &argOperation,
	const std::experimental::string_view &argLeft,
	const TL &valueLeft,
	const std::experimental::string_view &argRight,
	const TR &valueRight
) {
	unused(argOperation);
	unused(operation);

	if(printAll())
		LOGD << point << " [[ Verify ]] " << expression.data();

	check(point, checkExpression<operation>(valueLeft, valueRight), [&](){
		Exceptions::CheckFailure::FailureInfo failureInfo = {
			{"Message", "Expression evaluated to false"},
			{"Expression", expression.to_string()}
		};
		std::string stringLeft = argToString(valueLeft);
		std::string stringRight = argToString(valueRight);
		if(operation == ExpressionOperation::Equal) {
			auto linesLeft = Utility::Algorithm::splitted<std::vector<std::string>>(stringLeft, '\n');
			auto linesRight = Utility::Algorithm::splitted<std::vector<std::string>>(stringRight, '\n');

			dtl::Diff<std::string, std::vector<std::string>> diff(linesLeft, linesRight);
			std::ostringstream oss;
			diff.compose();
			diff.composeUnifiedHunks();
			diff.printUnifiedFormat(oss);
			failureInfo["Difference"] = oss.str();
		} else {
			failureInfo["Left operand"] = argLeft.to_string()+" ("+stringLeft+")";
			failureInfo["Right operand"] = argRight.to_string()+" ("+stringRight+")";
		}
		return failureInfo;
	});
}

class _NoException {};

inline void _addExceptionDetails(Exceptions::CheckFailure::FailureInfo &info, const std::exception &exception) {
	info.emplace("Exception", exception.what());
}

template<typename... Args> inline void _addExceptionDetails(Args...) {}

template<typename EXCEPTION> static void _catchWrongException(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const std::experimental::string_view &message,
	const EXCEPTION *exception = nullptr
) {
	check(point, false, [&]()->Exceptions::CheckFailure::FailureInfo{
		Exceptions::CheckFailure::FailureInfo info{
			{"Message", message.to_string()},
			{"Expression", expression.to_string()},
			{"Exception type", Utility::ABI::demangle(Utility::ABI::currentExceptionType().name())},
		};
		if(exception)
			_addExceptionDetails(info, exception);
		return info;
	});
}

template<typename FUNCTION, typename T> typename std::enable_if<!std::is_void<T>::value, T>::type
_tryBlock(const FUNCTION &fn, const std::function<void()> &fn2) {
	T result = fn();
	fn2();
	return result;
}

template<typename FUNCTION, typename T> typename std::enable_if<std::is_void<T>::value, T>::type
_tryBlock(const FUNCTION &fn, const std::function<void()> &fn2) {
	fn();
	fn2();
}

template<typename T>
T defaultResult() {
	return T();
}

template<typename EXCEPTION, typename FUNCTION, typename RETURN> typename std::enable_if<
	std::is_same<EXCEPTION, std::exception>::value,
	RETURN 
>::type _catch(
	const FUNCTION &fn,
	const std::function<void()> &noException,
	const std::function<void(const EXCEPTION &)> &specifiedException,
	const std::function<void(const std::exception &)> &,
	const std::function<void()> &unknownException
) {
	try {
		return _tryBlock<FUNCTION, RETURN>(fn, noException);
	} catch(const EXCEPTION &exception) {
		specifiedException(exception);
	} catch(const Exceptions::CheckFailure &) {
		throw;
	} catch(...) {
		unknownException();
	}
	return defaultResult<RETURN>();
}

template<typename EXCEPTION, typename FUNCTION, typename RETURN> typename std::enable_if<
	std::is_same<EXCEPTION, _NoException>::value,
	RETURN
>::type _catch(
	const FUNCTION &fn,
	const std::function<void()> &noException,
	const std::function<void(const EXCEPTION &)> &,
	const std::function<void(const std::exception &)> &standardException,
	const std::function<void()> &unknownException
) {
	try {
		return _tryBlock<FUNCTION, RETURN>(fn, noException);
	} catch(const std::exception &exception) {
		standardException(exception);
	} catch(const Exceptions::CheckFailure &) {
		throw;
	} catch(...) {
		unknownException();
	}
	return defaultResult<RETURN>();
}

template<typename EXCEPTION, typename FUNCTION, typename RETURN> typename std::enable_if<
	!std::is_same<EXCEPTION, _NoException>::value && !std::is_same<EXCEPTION, std::exception>::value,
	RETURN
>::type _catch(
	const FUNCTION &fn,
	const std::function<void()> &noException,
	const std::function<void(const EXCEPTION &)> &specifiedException,
	const std::function<void(const std::exception &)> &standardException,
	const std::function<void()> &unknownException
) {
	try {
		return _tryBlock<FUNCTION, RETURN>(fn, noException);
	} catch(const EXCEPTION &exception) {
		specifiedException(exception);
	} catch(const std::exception &exception) {
		standardException(exception);
	} catch(const Exceptions::CheckFailure &) {
		throw;
	} catch(...) {
		unknownException();
	}
}

enum class ThrowsCheckMode {
	NoException,
	ConvertibleException,
	ExactTypeException,
};

template<typename EXCEPTION, typename FUNCTION, typename RETURN> RETURN _throws(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const FUNCTION &fn,
	const std::function<void(const EXCEPTION &exception)> exactTypeException,
	const ThrowsCheckMode mode
) {
	return _catch<EXCEPTION, FUNCTION, RETURN>(
		fn,
		[&](){ // no exception
			if(mode != ThrowsCheckMode::NoException)
				check(point, false, [&]()->Exceptions::CheckFailure::FailureInfo{
					return {
						{"Message", "No exception thrown"},
						{"Expression", expression.to_string()}
					};
				});
		},
		[&](const EXCEPTION &exception){ // specified exception
			if(typeid(exception) == typeid(EXCEPTION))
				exactTypeException(exception);
			else if(mode == ThrowsCheckMode::ExactTypeException)
					_catchWrongException(point, expression, "Thrown not-matching (but convertible) exception"_sv, &exception);

		},
		[&](const std::exception &exception) { // standard exception
			_catchWrongException(point, expression, "Thrown not-matching exception"_sv, &exception);
		},
		[&]() { // unknown exception
			_catchWrongException<_NoException>(point, expression, "Thrown unknown exception"_sv);
		}
	);
}

template<typename EXCEPTION> void throws(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const std::function<void()> &fn,
	bool exact=false
) {
	if(printAll())
		LOGD << point << " [[ Throw" << (exact ? "Exact" : "") << " ]] " << expression.data();

	_throws<EXCEPTION, decltype(fn), void>(
			point,
			expression,
			fn,
			[](const EXCEPTION &){},
			(exact ? ThrowsCheckMode::ExactTypeException : ThrowsCheckMode::ConvertibleException)
	);
}

template<typename EXCEPTION>
void throwsEqual(
	const PointInfo &point,
	const std::experimental::string_view &strInstance,
	const EXCEPTION &instance,
	const std::experimental::string_view &expression,
	const std::function<void()> &fn
) {
	if(printAll())
		LOGD << point << " [[ ThrowEqual (" << strInstance.data() << ") ]] " << expression.data();

	_throws<EXCEPTION, decltype(fn), void>(point, expression, fn, [&](const EXCEPTION &exception) {
		if(exception != instance)
			_catchWrongException(point, expression, "Thrown not-equal exception"_sv, &exception);
	}, ThrowsCheckMode::ExactTypeException);
}

template<typename T>
auto noThrows(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const T &fn
) -> decltype(fn()) {
	if(printAll())
		LOGD << point << " [[ NoThrow ]] " << expression.data();

	return _throws<_NoException, decltype(fn), decltype(fn())>(point, expression, fn, [](...){}, ThrowsCheckMode::NoException);
}

bool catchTestExceptions(const std::experimental::string_view &caseName, const std::function<void()> &fn);

struct Checker {
	std::string currentTestSuite;
	bool expectFailure = false;
	bool printAll = false;
};

Checker checker();
void setChecker(Checker checker);

}
}
}

#define TIAL_TESTING_POINTINFO \
	(::Tial::Testing::Check::PointInfo(__FILE__, __LINE__, _caseName()))

#undef TIAL_MODULE
