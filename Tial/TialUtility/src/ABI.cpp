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
#include "Platform.hpp"

#include <functional>
#include <memory>

#include <boost/core/demangle.hpp>

#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
#include <execinfo.h>
#elif BOOST_OS_WINDOWS
#include <stdio.h>

#define NOMINMAX
#include <Windows.h>
#include <Dbghelp.h>
#endif

#define TIAL_MODULE "Tial::Utility::ABI"

std::string Tial::Utility::ABI::demangle(const std::string &name) {
	return boost::core::demangle(name.c_str());
}

void Tial::Utility::ABI::currentStackTrace(std::function<void(const std::string&)> fn, unsigned int skipLevels) noexcept {
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
#elif BOOST_OS_WINDOWS
	const size_t backTraceSize = std::numeric_limits<unsigned short>::max();
	void *backTrace[backTraceSize];
	USHORT count = CaptureStackBackTrace(
		skipLevels,
		std::numeric_limits<unsigned short>::max(),
		backTrace,
		nullptr
	);
	if(count == 0) {
		fn("<failed CaptureStackBackTrace>");
		return;
	}

	HANDLE process = GetCurrentProcess();
	if(!SymInitialize(process, nullptr, true)) {
		auto error = GetLastError();
		fn("<failed SymInitialize: "+std::to_string(error)+">");
		return;
	}

	const size_t nameSize = 256;
	const size_t symbolInfoSize = sizeof(SYMBOL_INFOW) + (nameSize+1)*sizeof(wchar_t);
	std::unique_ptr<SYMBOL_INFOW, std::function<void(SYMBOL_INFOW*)>> info{
		reinterpret_cast<SYMBOL_INFOW*>(new char[symbolInfoSize]),
		[](SYMBOL_INFOW *info) {
			delete[] reinterpret_cast<char*>(info);
		}
	};

    memset(info.get(), 0, symbolInfoSize);
    info->MaxNameLen = nameSize;
    info->SizeOfStruct = sizeof(SYMBOL_INFOW);

    for(unsigned int i = 0; i < count; ++i) {
		if(!SymFromAddrW(process, reinterpret_cast<DWORD64>(backTrace[i]), 0, info.get())) {
			auto error = GetLastError();
			fn("<failed SymFromAddrW: "+std::to_string(error)+">");
		} else
			fn(Platform::Win32::wideStringUTF8Cast<char, wchar_t>(std::wstring{info->Name, info->NameLen}));
	}
#else
#error "Platform not supported"
#endif
}

std::type_index Tial::Utility::ABI::currentExceptionType() noexcept {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	std::type_info *info = abi::__cxa_current_exception_type();
	if(!info)
		return typeid(void);
	return *info;
#elif BOOST_OS_WINDOWS
	LOGW << "currentExceptionType is not available on this platform";
	return typeid(void);
#else
#error "Platform not supported"
#endif
}
