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
#include "Exception.hpp"
#include "ABI.hpp"
#include "Logger.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#define TIAL_MODULE "Tial::Utility::Exception"

Tial::Utility::Exception::CommonArgs::CommonArgs(): _line(0) {}

Tial::Utility::Exception::CommonArgs::CommonArgs(
	const std::string &module, const std::string &file, unsigned int line, const std::string &function
): _module(module), _file(file), _line(line), _function(function) {}

const std::string &Tial::Utility::Exception::CommonArgs::module() const noexcept {
	return _module;
}

const std::string &Tial::Utility::Exception::CommonArgs::file() const noexcept {
	return _file;
}

const unsigned int &Tial::Utility::Exception::CommonArgs::line() const noexcept {
	return _line;
}

const std::string &Tial::Utility::Exception::CommonArgs::function() const noexcept {
	return _function;
}

void Tial::Utility::Exception::setCommonArgs(const CommonArgs &commonArgs) {
	_commonArgs = commonArgs;
	std::ostringstream oss;
	oss << file() << ":" << line() << " " << module() << " " << function() << " " << message();
	_fullMessage = oss.str();
}

Tial::Utility::Exception::Exception(const std::string &message)
	: _message(message), _fullMessage(message) {}

const std::string &Tial::Utility::Exception::module() const noexcept {
	return _commonArgs.module();
}

const std::string &Tial::Utility::Exception::file() const noexcept {
	return _commonArgs.file();
}

const unsigned int &Tial::Utility::Exception::line() const noexcept {
	return _commonArgs.line();
}

const std::string &Tial::Utility::Exception::function() const noexcept {
	return _commonArgs.function();
}

const std::string &Tial::Utility::Exception::message() const noexcept {
	return _message;
}

const char *Tial::Utility::Exception::what() const noexcept {
	return _fullMessage.c_str();
}

Tial::Utility::StandardException::StandardException(const std::exception &exception)
	: Exception(exception.what()), exception(std::make_exception_ptr(exception)) {}

static std::vector<Tial::Utility::ThrowHook> throwHooks;

void Tial::Utility::installExceptionThrowHook(const ThrowHook &hook) {
	throwHooks.push_back(hook);
}

void Tial::Utility::_triggerThrowHooks(const Exception &exception, const bool log) noexcept {
    try {
    	TIAL_UTILITY_LOGGER_LOG(log ? Logger::Level::Warning : Logger::Level::Nice3) << "Exception: " << exception.message();
	    TIAL_UTILITY_LOGGER_LOG(log ? Logger::Level::Warning : Logger::Level::Nice3) << exception.file() << ":"
		    	<< exception.line() << " " << exception.module() << " " << exception.function() << " " << exception.message();

	    std::vector <std::string> st;
    	LOGN3 << "Stack trace:\n" << (
            ABI::currentStackTrace([&st](const std::string &s) { st.push_back(s); }),
            boost::algorithm::join(st, "\n")
	    );

        for(auto &hook: throwHooks)
		    hook(exception);
    } catch (const std::exception &e) {
        LOGC << "Error while processing throw hooks: " << e.what();
    } catch (...) {
        LOGC << "Error while processing throw hooks: unknown";
    }
}

Tial::Utility::ThrowHookTrigger::ThrowHookTrigger(const bool log): log(log) {}

Tial::Utility::Exceptions::NotSupported::NotSupported(): Exception("Not supported") {}

Tial::Utility::Exceptions::NotImplemented::NotImplemented(): Exception("Not implemented") {}
