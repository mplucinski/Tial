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
#include "ABI.hpp"
#include "Logger.hpp"

#include <functional>
#include <memory>

#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
#include <cxxabi.h>
#include <execinfo.h>
#else
#error "Platform not supported"
#endif

#define TIAL_MODULE "Tial::Utility::ABI"

std::string Tial::Utility::ABI::demangle(const std::string &name) {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	int status;
	std::unique_ptr<char, std::function<void(char*)>> ptr{
		abi::__cxa_demangle(name.c_str(), 0, 0, &status),
		[](char *ptr) {
			free(ptr);
		}
	};
	if(status != 0)
		return "<unknown>";
	return ptr.get();
#else
#error "Platform not supported"
#endif
}

void Tial::Utility::ABI::currentStackTrace(std::function<void(const std::string&)> fn, unsigned int skipLevels) {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	void *buffer[256];
	int count = backtrace(buffer, sizeof(buffer));
	std::unique_ptr<char*, std::function<void(char**)>> symbols{
		backtrace_symbols(buffer, count),
		[](char **ptr) {
			free(ptr);
		}
	};
	if(!symbols)
		fn("<failed>");

	for(int i = skipLevels; i < count; ++i) {
		std::string line = symbols.get()[i];
		try {
			auto first = line.find_first_not_of(" ");
			auto last = line.find_first_of(" ", first);
			std::string id = line.substr(first, last-first);
			first = line.find_first_not_of(" ", last);
			last = line.find_first_of(" ", first);
			std::string module = line.substr(first, last-first);
			first = line.find_first_not_of(" ", last);
			last = line.find_first_of(" ", first);
			std::string addr = line.substr(first, last-first);
			first = line.find_first_not_of(" ", last);
			last = line.find_first_of(" ", first);
			std::string name = demangle(line.substr(first, last-first));
			first = line.find_first_not_of(" ", last);
			last = line.find_first_of(" ", first);
			first = line.find_first_not_of(" ", last);
			std::string offset = line.substr(first);

			line = id;
			while(line.size() < 5) line.append(" ");
			line += module + " ";
			while(line.size() < 40) line.append(" ");
			line += addr + " ";
			while(line.size() < 50) line.append(" ");
			line += name + " + " + offset;
			fn(line);
		} catch(const std::out_of_range &e) {
			fn(line);
		}
	}
#else
#error "Platform not supported"
#endif
}

std::type_index Tial::Utility::ABI::currentExceptionType() {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	std::type_info *info = abi::__cxa_current_exception_type();
	if(!info)
		return typeid(void);
	return *info;
#else
#error "Platform not supported"
#endif
}
