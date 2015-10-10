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
#include "../Check.hpp"
#include "../Thread.hpp"

#include <TialUtility/TialUtility.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <dtl/dtl.hpp>

#define TIAL_MODULE "Tial::Testing::Check"

static thread_local Tial::Testing::Check::Checker checker;

Tial::Testing::Check::PointInfo::PointInfo(
	const std::experimental::string_view &file,
	const unsigned int line,
	const std::experimental::string_view &testCase
): _file(file), _line(line), _testCase(testCase) {}

const std::string &Tial::Testing::Check::PointInfo::file() const {
	return _file;
}

unsigned int Tial::Testing::Check::PointInfo::line() const {
	return _line;
}

const std::string &Tial::Testing::Check::PointInfo::testCase() const {
	return _testCase;
}

Tial::Utility::Logger::Stream &Tial::Testing::Check::operator<<(
	Utility::Logger::Stream &s, const PointInfo &info
) {
	return s
		<< info.testCase().c_str()
		<< ":" << Utility::UnixPath(info.file()).basename().c_str()
		<< ":" << info.line();
}

Tial::Testing::Check::Exceptions::CheckFailure::CheckFailure(const PointInfo &point, const FailureInfo &info)
  : _point(point), _info(info) {}

const Tial::Testing::Check::PointInfo &Tial::Testing::Check::Exceptions::CheckFailure::point() const {
	return _point;
}

const Tial::Testing::Check::Exceptions::CheckFailure::FailureInfo &
Tial::Testing::Check::Exceptions::CheckFailure::info() const {
	return _info;
}

void Tial::Testing::Check::startTestSuite(const std::experimental::string_view &name, const bool printAll) {
	::checker.currentTestSuite = name.to_string();
	::checker.expectFailure = false;
	::checker.printAll = printAll;
}

bool Tial::Testing::Check::printAll() {
	return ::checker.printAll;
}

void Tial::Testing::Check::check(
	const PointInfo &point,
	const bool value,
	const std::function<Exceptions::CheckFailure::FailureInfo()> &buildFailureInfo
) {
	if(!value != ::checker.expectFailure) {
		if(::checker.expectFailure)
			throw Tial::Testing::Check::Exceptions::CheckUnexpectedSucceeded(point, buildFailureInfo());
		else
			throw Tial::Testing::Check::Exceptions::CheckFailure(point, buildFailureInfo());
	}
	::checker.expectFailure = false;
}

void Tial::Testing::Check::expectFail(const PointInfo &point) {
	if(printAll())
		LOGW << point << " [[ ExpectFail ]]";
	::checker.expectFailure = true;
}

void Tial::Testing::Check::fail(const PointInfo &point) {
	if(printAll())
		LOGD << point << " [[ Fail ]]";
	check(point, false, []()->Exceptions::CheckFailure::FailureInfo{ return {{"Message", "Forced failure"}}; });
}

void Tial::Testing::Check::expression(
	const PointInfo &point,
	const std::experimental::string_view &expression,
	const bool value
) {
	if(printAll())
		LOGD << point << " [[ Verify ]] " << expression.data();
	check(point, value, [&]()->Exceptions::CheckFailure::FailureInfo{
		return {
			{"Message", "Expression returned false"},
			{"Expression", expression.to_string()}
		};
	});
}

bool Tial::Testing::Check::catchTestExceptions(const std::experimental::string_view &caseName, const std::function<void()> &fn) {
	try {
		fn();
		return true;
	} catch(const Exceptions::CheckUnexpectedSucceeded &failed) {
		LOGC << "TEST SUCCEEDED, WHEN FAILURE WAS EXPECTED";
		LOGC << "\tTest: " << caseName;
		LOGC << "\tLocation: " << failed.point().file().c_str() << ":" << failed.point().line();
		for(auto i: failed.info())
			LOGC << "\t" << i.first.c_str() << ": " << i.second.c_str();
	} catch(const Exceptions::CheckFailure &failed) {
		LOGC << "TEST FAILED";
		LOGC << "\tTest: " << caseName;
		LOGC << "\tLocation: " << failed.point().file().c_str() << ":" << failed.point().line();
		for(auto i: failed.info())
			LOGC << "\t" << i.first.c_str() << ": " << i.second.c_str();
	} catch(const Utility::Exception &exception) {
		LOGC << "Exception caught during test processing: " << Utility::ABI::demangle(typeid(exception).name());
		LOGC << "\tTest: " << caseName;
		LOGC << "\tLocation: " << exception.file().c_str() << ":" << exception.line();
		LOGC << "\tModule: " << exception.module().c_str();
		LOGC << "\tFunction: " << exception.function().c_str();
		LOGC << "\tMessage: " << exception.message().c_str();
	} catch(const std::exception &exception) {
		LOGC << "Exception caught during test processing: " << Utility::ABI::demangle(typeid(exception).name());
		LOGC << "\tTest: " << caseName;
		LOGC << "\tMessage: " << exception.what();
	} catch(...) {
		LOGC << "Unknown exception caught during test processing";
	}
	return false;
}

Tial::Testing::Check::Checker Tial::Testing::Check::checker() {
	return ::checker;
}

void Tial::Testing::Check::setChecker(Checker checker) {
	::checker = checker;
}
