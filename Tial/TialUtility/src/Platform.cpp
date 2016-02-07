/* Copyright (c) 2016, Mariusz Plucinski
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
#include "Platform.hpp"

#include <vector>
#include <system_error>

#if BOOST_OS_WINDOWS
#define WINVER 0x0600
#include <Windows.h>

void Tial::Utility::Platform::Win32::throwLastError() {
	auto error = GetLastError();
	throw std::system_error(error, std::system_category());
}

namespace {

template<typename OutputCharacter, typename InputCharacter>
int _convertString(
	OutputCharacter *output,
	const int outputSize,
	const std::experimental::basic_string_view<InputCharacter> &string
);

template<>
int _convertString<char, wchar_t>(
	char *output,
	const int outputSize,
	const std::experimental::wstring_view &string
) {
	return WideCharToMultiByte(
		CP_UTF8,
		WC_ERR_INVALID_CHARS,
		string.data(),
		string.size(),
		output,
		outputSize,
		nullptr,
		nullptr
	);
}

template<>
int _convertString<wchar_t, char>(
	wchar_t *output,
	const int outputSize,
	const std::experimental::string_view &string
) {
	return MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		string.data(),
		string.size(),
		output,
		outputSize
	);
}

}

template<typename OutputCharacter, typename InputCharacter>
std::basic_string<OutputCharacter>
Tial::Utility::Platform::Win32::wideStringUTF8Cast(const std::experimental::basic_string_view<InputCharacter> &string) {
	if(string.empty())
		return {};

	int size = _convertString<OutputCharacter>(nullptr, 0, string);
	if(size == 0)
		throwLastError();

	std::vector<OutputCharacter> result;
	result.resize(size);

	size = _convertString<OutputCharacter>(result.data(), result.size(), string);
	if(size < 0 || static_cast<unsigned int>(size) != result.size())
		throwLastError();

	return {result.cbegin(), result.cend()};
}

template
std::string Tial::Utility::Platform::Win32::wideStringUTF8Cast<char, wchar_t>(
	const std::experimental::wstring_view &
);

template
std::wstring Tial::Utility::Platform::Win32::wideStringUTF8Cast<wchar_t, char>(
	const std::experimental::string_view &
);

#endif
