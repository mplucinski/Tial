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
#include "Platform.hpp"

#include <algorithm>
#include <memory>
#include <system_error>

#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
#include <dirent.h>
#include <sys/param.h>
#include <unistd.h>
#endif

#include <boost/predef.h>

Tial::Utility::NativePath Tial::Utility:: _currentPath() {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	std::unique_ptr<char, std::function<void(char*)>> rawPath(
		getcwd(nullptr, MAXPATHLEN),
		[](char *ptr) {
			free(ptr);
		}
	);
	if(!rawPath)
		throw std::system_error(errno, std::system_category());

	return std::string(rawPath.get());
#else
#error "Platform not supported"
#endif
}

std::vector<std::shared_ptr<Tial::Utility::_DirectoryEntry<
	Tial::Utility::NativePath
>>> Tial::Utility::_directoryContent(const NativePath &path) {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	std::vector<std::shared_ptr<_DirectoryEntry<NativePath>>> output;

	std::unique_ptr<DIR, std::function<void(DIR*)>> directory(
		opendir(std::string(path).c_str()),
		[](DIR *ptr) {
			closedir(ptr);
		}
	);
	if(!directory)
		throw std::system_error(errno, std::system_category());

	struct dirent *entry;
	while((entry = readdir(directory.get()))) {
		std::string name(entry->d_name);
		if(name == "." || name == "..")
			continue;

		NativePath entryPath(name);

		std::shared_ptr<_DirectoryEntry<NativePath>> outputEntry;
		if(entry->d_type == DT_DIR)
			outputEntry.reset(new Directory<NativePath>(entryPath));
		else
			outputEntry.reset(new _DirectoryEntry<NativePath>(entryPath));
		output.push_back(outputEntry);
	}

	std::sort(output.begin(), output.end());

	return output;
#else
#error "Platform not supported"
#endif
}
