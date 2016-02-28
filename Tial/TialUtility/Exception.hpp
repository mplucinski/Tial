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
#include "TialUtilityExport.hpp"
#include "Language.hpp"

#include <exception>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

namespace Tial {
namespace Utility {

class StandardException;

class TIALUTILITY_EXPORT Exception: public std::exception {
public:
	class TIALUTILITY_EXPORT CommonArgs {
		std::string _module;
		std::string _file;
		unsigned int _line;
		std::string _function;

	public:
		CommonArgs();
		CommonArgs(
			const std::string &module, const std::string &file, unsigned int line, const std::string &function
		);
		const std::string &module() const noexcept;
		const std::string &file() const noexcept;
		const unsigned int &line() const noexcept;
		const std::string &function() const noexcept;
	};

private:
	std::string _message;
	std::string _fullMessage;
	CommonArgs _commonArgs;

protected:
	void setCommonArgs(const CommonArgs &commonArgs);
public:
	Exception(const std::string &message);

	const std::string &module() const noexcept;
	const std::string &file() const noexcept;
	const unsigned int &line() const noexcept;
	const std::string &function() const noexcept;
	const std::string &message() const noexcept;
	virtual const char *what() const noexcept override;

	template<typename T, typename = typename std::enable_if<std::is_base_of<Exception, T>::value>::type>
	friend T operator>>(const Exception::CommonArgs &commonArgs, T &&exception) {
		exception.setCommonArgs(commonArgs);
		return exception;
	}
};

template<typename T, typename = typename std::enable_if<!std::is_base_of<Exception, T>::value>::type>
StandardException operator>>(const Exception::CommonArgs &commonArgs, T &&exception);

class TIALUTILITY_EXPORT StandardException: public Exception {
	std::exception_ptr exception;
public:
	explicit StandardException(const std::exception &exception);

	template<typename T, typename>
	friend StandardException operator>>(const Exception::CommonArgs &commonArgs, T &&exception) {
		StandardException standardException(std::forward<T>(exception));
		standardException.setCommonArgs(commonArgs);
		return standardException;
	}
};

typedef std::function<void(const Exception &)> ThrowHook;
TIALUTILITY_EXPORT void installExceptionThrowHook(const ThrowHook &hook);

TIALUTILITY_EXPORT void _triggerThrowHooks(const Exception &exception, const bool log) noexcept;

class TIALUTILITY_EXPORT ThrowHookTrigger {
	bool log = false;
public:
	explicit ThrowHookTrigger(const bool log);

	template<typename ExceptionType>
	ExceptionType operator>(const ExceptionType &exception) {
		_triggerThrowHooks(exception, log);
		return exception;
	}
};

namespace Exceptions {

class TIALUTILITY_EXPORT NotSupported: public Exception {
public:
	explicit NotSupported();
};

class TIALUTILITY_EXPORT NotImplemented: public Exception {
public:
	explicit NotImplemented();
};

}

}
}

#define TIAL_UTILITY_EXCEPTION_COMMONARGS \
	(::Tial::Utility::Exception::CommonArgs( \
		(TIAL_UTILITY_LANGUAGE_CURRENT_MODULE.to_string()), \
		(__FILE__), \
		(__LINE__), \
		(TIAL_UTILITY_LANGUAGE_FUNCTION_SIGNATURE) \
	))

#define TIAL_UTILITY_THROW_HOOK_TRIGGER(log) \
	(::Tial::Utility::ThrowHookTrigger(log))

// note: it is expected to use standard ExceptionClass(...args) construction after following macro instance
#define TIAL_UTILITY_THROW_IMPL(log) \
	throw TIAL_UTILITY_THROW_HOOK_TRIGGER(log) > TIAL_UTILITY_EXCEPTION_COMMONARGS >>

#define TIAL_UTILITY_THROW \
	TIAL_UTILITY_THROW_IMPL(true)

#define TIAL_UTILITY_THROW_NO_LOG \
	TIAL_UTILITY_THROW_IMPL(false)

// easy throwing
#ifndef TIAL_UTILITY_EXCEPTION_DISABLE_THROW_MACRO
#define THROW TIAL_UTILITY_THROW
#define THROW_NL TIAL_UTILITY_THROW_NO_LOG
#endif
