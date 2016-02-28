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
#include "Directory.hpp"
#include "Exception.hpp"
#include "Platform.hpp"

#include <algorithm>
#include <memory>
#include <system_error>

#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
#include <dirent.h>
#include <sys/param.h>
#include <unistd.h>
#elif BOOST_OS_WINDOWS
#include <Windows.h>
#endif

#include <boost/predef.h>

#define TIAL_MODULE "Tial::Utility::Directory"

Tial::Utility::NativePath Tial::Utility:: _currentPath() {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	std::unique_ptr<char, std::function<void(char*)>> rawPath(
		getcwd(nullptr, MAXPATHLEN),
		[](char *ptr) {
			free(ptr);
		}
	);
	if(!rawPath)
		THROW std::system_error(errno, std::system_category());

	return std::string(rawPath.get());
#elif BOOST_OS_WINDOWS
	auto size = GetCurrentDirectoryW(0, nullptr);

	std::vector<wchar_t> directory;
	directory.resize(size);

	size = GetCurrentDirectoryW(directory.size(), directory.data());
	if(size == 0 || size+1 != directory.size()) {
		LOGW << "Too less bytes for current directory: " << directory.size()
			<< ", needed " << size;
		Platform::Win32::throwLastError();
	}

	return Platform::Win32::wideStringUTF8Cast<char, wchar_t>(directory.data());
#else
#error "Platform not supported"
#endif
}

std::vector<std::shared_ptr<Tial::Utility::_DirectoryEntry<
	Tial::Utility::NativePath
>>> Tial::Utility::_directoryContent(const NativePath &path) {
	std::vector<std::shared_ptr<_DirectoryEntry<NativePath>>> output;

#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	std::unique_ptr<DIR, std::function<void(DIR*)>> directory(
		opendir(std::string(path).c_str()),
		[](DIR *ptr) {
			closedir(ptr);
		}
	);
	if(!directory)
		THROW std::system_error(errno, std::system_category());

	struct dirent *entry;
	while((entry = readdir(directory.get()))) {
		std::string name{entry->d_name};

#elif BOOST_OS_WINDOWS
	struct FindHandle {
		HANDLE handle;

		explicit FindHandle(HANDLE handle): handle(handle) {
			if(handle == INVALID_HANDLE_VALUE)
				Platform::Win32::throwLastError();
		}

		~FindHandle() {
			if(handle != INVALID_HANDLE_VALUE)
				if(!FindClose(handle))
					Platform::Win32::throwLastError();
		}
	};

	WIN32_FIND_DATAW findData;
	FindHandle find{FindFirstFileExW(
		Platform::Win32::wideStringUTF8Cast<wchar_t, char>(std::string(path)+"\\*").c_str(),
		FindExInfoBasic,
		&findData,
		FindExSearchNameMatch,
		nullptr,
		0
	)};

	do {
		std::string name{Platform::Win32::wideStringUTF8Cast<char, wchar_t>(findData.cFileName)};
#else
#error "Platform not supported"
#endif

		if(name == "." || name == "..")
			continue;

		NativePath entryPath(name);

		std::shared_ptr<_DirectoryEntry<NativePath>> outputEntry;
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
		if(entry->d_type == DT_DIR)
#elif BOOST_OS_WINDOWS
		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#else
#error "Platform not supported"
#endif
			outputEntry.reset(new Directory<NativePath>(entryPath));
		else
			outputEntry.reset(new _DirectoryEntry<NativePath>(entryPath));
		output.push_back(outputEntry);
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	}
#elif BOOST_OS_WINDOWS
	} while(FindNextFileW(find.handle, &findData));
	
	auto error = GetLastError();
	if(error != ERROR_NO_MORE_FILES)
		Platform::Win32::throwLastError();
#else
#error "Platform not supported"
#endif

	std::sort(output.begin(), output.end());

	return output;
}
