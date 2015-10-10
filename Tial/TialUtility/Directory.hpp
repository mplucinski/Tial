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
#include "Path.hpp"

namespace Tial {
namespace Utility {

template<typename Path>
class Directory;

template<typename Path>
class _DirectoryEntry;

TIALUTILITY_EXPORT NativePath _currentPath();

TIALUTILITY_EXPORT
std::vector<std::shared_ptr<_DirectoryEntry<NativePath>>>
_directoryContent(const NativePath &path);

template<typename Path>
class _DirectoryEntry {
	Path entryPath;

	_DirectoryEntry(const Path &path): entryPath(path) {}

public:
	virtual ~_DirectoryEntry() {}

	Path path() const {
		return entryPath;
	}

	friend class Directory<Path>;
	friend std::vector<std::shared_ptr<_DirectoryEntry<NativePath>>> _directoryContent(const NativePath &path);
};

template<typename Path>
class Directory: public _DirectoryEntry<Path> {
public:
	typedef _DirectoryEntry<Path> Entry;
	typedef std::shared_ptr<Entry> EntryPtr;
	typedef std::vector<EntryPtr> Entries;

	static Directory<Path> current() {
		return _currentPath();
	}

	Directory(const Path &path): _DirectoryEntry<Path>(path) {}

	Entries content() const {
		return _directoryContent(this->path());
	}

	Directory operator/(const NativePath &second) {
		return this->path()/second;
	}
};

template<typename Path>
std::ostream &operator<<(std::ostream &os, const _DirectoryEntry<Path> &entry) {
	os << entry->path();
	return os;
}

typedef Directory<NativePath> NativeDirectory;

}
}
