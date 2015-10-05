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
#include "tialutility_export.h"

#include <limits>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

#include <boost/predef.h>

#include "Exception.hpp"
#include "Logger.hpp"
#include "Wildcards.hpp"

#define TIAL_MODULE "Tial::Utility::Path"

namespace Tial {
namespace Utility {

template<typename PathFormatDescriptor>
class GenericPath;

class Path;

}
}

namespace std {

template<typename PathFormatDescriptor>
struct hash<Tial::Utility::GenericPath<PathFormatDescriptor>> {
	std::size_t operator()(
		const Tial::Utility::GenericPath<PathFormatDescriptor> &path
	) const {
		LOGN3 << "Calculating hash for " << path;
		return hash<string>()(path.path);
	}
};

template<>
struct hash<Tial::Utility::Path> {
	std::size_t operator()(const Tial::Utility::Path &path) const;
};

}

namespace Tial {
namespace Utility {

enum class PathFormat: int {
	Unix,
	Windows
};

namespace Exceptions {

class TIALUTILITY_EXPORT InvalidPathFormat: public Exception {
	PathFormat format;
public:
	explicit InvalidPathFormat(PathFormat format);
};

}

template<typename PathFormatDescriptor>
class GenericPath;

namespace PathFormatDescriptors {

class _UnixStyle {
public:
	TIALUTILITY_EXPORT static const std::string currentDirectory;
	TIALUTILITY_EXPORT static const std::string parentDirectory;
};

class TIALUTILITY_EXPORT Unix: public _UnixStyle {
public:
	static const PathFormat format = PathFormat::Unix;
	static const char separator;
	static bool isRoot(const std::string &name);
	static bool isMultiRootFormat();
};

class TIALUTILITY_EXPORT Windows: public _UnixStyle {
public:
	static const PathFormat format = PathFormat::Windows;
	static const char separator;
	static bool isRoot(const std::string &name);
	static bool isMultiRootFormat();
};

}

template<typename PathFormatDescriptor>
class GenericPathConstIterator;

template<typename PathFormatDescriptor>
std::ostream &operator<<(std::ostream &os, const GenericPathConstIterator<PathFormatDescriptor> &iterator);

template<typename PathFormatDescriptor>
class GenericPathConstIterator {
	GenericPathConstIterator(const GenericPath<PathFormatDescriptor> &path, size_t pos)
		: path(path), pos(pos) {}

	const GenericPath<PathFormatDescriptor> &path;
	size_t pos;
public:
	GenericPathConstIterator(const GenericPathConstIterator<PathFormatDescriptor> &other)
		: path(other.path), pos(other.pos) {}

	const std::string operator*() const {
		return path[pos];
	}

	GenericPathConstIterator &operator++() {
		pos++;
		return *this;
	}

	GenericPathConstIterator operator++(int) {
		GenericPathConstIterator<PathFormatDescriptor> old = *this;
		++(*this);
		return old;
	}

	bool operator==(const GenericPathConstIterator &second) const {
		return pos == second.pos;
	}

	bool operator!=(const GenericPathConstIterator &second) const {
		return !operator==(second);
	}

	friend class GenericPath<PathFormatDescriptor>;

	friend std::ostream &operator<< <PathFormatDescriptor>(
		std::ostream &os, const GenericPathConstIterator<PathFormatDescriptor> &iterator
	);
};

template<typename PathFormatDescriptor>
std::ostream &operator<<(std::ostream &os, const GenericPathConstIterator<PathFormatDescriptor> &iterator) {
	if(iterator.pos < iterator.path.size())
		os << *iterator;
	else if(iterator.pos == iterator.path.size())
		os << "<end iterator>";
	else
		os << "<invalid iterator>";
	return os;
}

template<typename PathFormatDescriptor>
class GenericPath {
public:
	typedef GenericPathConstIterator<PathFormatDescriptor> ConstIterator;

	friend struct std::hash<GenericPath<PathFormatDescriptor>>;

private:
	void invalidate() const {
		std::unique_lock<std::recursive_mutex> lock(mutex);
		cacheValid = false;
		cachedSize = 0;
		cachedParts.clear();
	}

	std::string _element(size_t index) const {
		size_t beginIdx = cachedParts.at(index);
		size_t endIdx = cachedParts.at(index+1);

		if(index != 0 && path[beginIdx] == PathFormatDescriptor::separator)
			beginIdx++;

		return path.substr(beginIdx, endIdx - beginIdx);
	}

	void ensureCache() const {
		if(cacheValid)
			return;

		std::unique_lock<std::recursive_mutex> lock(mutex);
		LOGN2 << "Refreshing cache for path " << path;

		cachedSize = 0;
		cachedParts.clear();

		// calculate positions of path elements
		size_t i = 0;
		if(!path.empty()) {
			//check if path starts with root (usually '/')
			if(path[i] == PathFormatDescriptor::separator) {
				cachedParts.push_back(i);
				cachedSize++;
				i++;
			}

			if(path.size()>i) {
				cachedParts.push_back(i);
				cachedSize++;
				i++;
			}

			for(; i < path.size(); ++i)
				if(path[i] == PathFormatDescriptor::separator) {
					cachedParts.push_back(i);
					cachedSize++;
				}
		}

		cachedParts.push_back(i);

		// check if path is canonical
		if(cachedParts.size() >= 2) {
			isCanonical = true;
			bool firstNonParentFound = false;
			for(size_t i = 0; i < cachedSize; ++i) {
				std::string part = _element(i);
				LOGN3 << "Checking " << part << " for canonicality";
				if(part == PathFormatDescriptor::currentDirectory) {
					isCanonical = false;
					break;
				} else if(part == PathFormatDescriptor::parentDirectory) {
					if(firstNonParentFound) {
						isCanonical = false;
						break;
					}
				} else
					firstNonParentFound = true;
			}
		}

		cacheValid = true;
	}

	void removeExcessingSeparators() {
		std::unique_lock<std::recursive_mutex> lock(mutex);
		if(!path.empty())
			for(size_t i = 0; i < path.size() - 1; ++i)
				if(path[i] == PathFormatDescriptor::separator && path[i+1] == PathFormatDescriptor::separator)
					path.erase(i, 1);

		while(path.size() > 1 && path[path.size() - 1] == PathFormatDescriptor::separator)
			path.erase(path.size() - 1, 1);
	}

	std::string path;

	mutable std::recursive_mutex mutex;
	mutable bool cacheValid = false;
	mutable bool isCanonical = false;
	mutable size_t cachedSize = 0;
	mutable std::vector<size_t> cachedParts;
public:
	static const size_t npos = std::numeric_limits<size_t>::max();

	GenericPath() {}

	GenericPath(const GenericPath<PathFormatDescriptor> &other): path(other.path) {}

	GenericPath(const char *path): GenericPath(std::string(path)) {}

	GenericPath(const std::string &path): path(path) {
		removeExcessingSeparators();
	}

	bool empty() const {
		return size() == 0;
	}

	size_t size() const {
		ensureCache();
		return cachedSize;
	}

	bool absolute() const {
		ensureCache();
		return !empty() && PathFormatDescriptor::isRoot((*this)[0]);
	}

	bool relative() const {
		return !absolute();
	}

	bool canonical() const {
		ensureCache();
		return isCanonical;
	}

	operator std::string() const {
		return path;
	}

	template<typename TargetPathFormatDescriptor>
	operator GenericPath<TargetPathFormatDescriptor>() const {
		if(absolute() &&
			PathFormatDescriptor::isMultiRootFormat() != TargetPathFormatDescriptor::isMultiRootFormat())
			throw std::invalid_argument("Root kind is incompatible between old and new path format");

		GenericPath<TargetPathFormatDescriptor> newPath;
		for(auto &&i: *this)
			newPath /= i;
		return newPath;
	}

	bool startsWith(const GenericPath &prefix) const {
		if(prefix.size() > size())
			return false;

		auto i = cbegin();
		auto j = prefix.cbegin();
		for(; j != prefix.cend(); ++i, ++j)
			if(*i != *j)
				return false;

		return true;
	}

	ConstIterator cbegin() const {
		return ConstIterator(*this, 0);
	}

	ConstIterator cend() const {
		return ConstIterator(*this, size());
	}

	ConstIterator begin() const {
		return cbegin();
	}

	ConstIterator end() const {
		return cend();
	}

	std::string operator[](size_t index) const {
		ensureCache();
		return _element(index);
	}

	GenericPath<PathFormatDescriptor> parent() const {
		if(empty())
			return GenericPath<PathFormatDescriptor>();
		return subpath(0, size()-1);
	}

	std::string basename() const {
		ensureCache();
		return empty() ? std::string() : (*this)[size()-1];
	}

	GenericPath<PathFormatDescriptor> subpath(size_t begin, size_t length = npos) const {
		ensureCache();

		size_t end = (length == npos ? size() : begin + length);

		size_t beginIdx = cachedParts.at(begin);
		size_t endIdx = cachedParts.at(end);

		if(begin != 0 && path[beginIdx] == PathFormatDescriptor::separator)
			beginIdx++;

		return path.substr(beginIdx, endIdx - beginIdx);
	}

	GenericPath<PathFormatDescriptor> canonicalized() const {
		ensureCache();

		if(canonical())
			return *this;

		GenericPath<PathFormatDescriptor> output;
		for(size_t i = 0; i < size(); ++i) {
			std::string element = (*this)[i];
			if(element == PathFormatDescriptor::currentDirectory)
				continue;
			else if(element == PathFormatDescriptor::parentDirectory) {
				if(output.size() > 0)
					output = output.subpath(0, output.size()-1);
				else
					output /= element;
			} else
				output /= element;
			LOGN3 << "Building canonical path: " << output;
		}
		return output;
	}

	GenericPath<PathFormatDescriptor> operator/(const GenericPath<PathFormatDescriptor> &second) const {
		GenericPath<PathFormatDescriptor> _path = *this;
		_path /= second;
		return _path;
	}

	bool operator==(const GenericPath<PathFormatDescriptor> &second) const {
		return path == second.path;
	}

	bool operator<(const GenericPath<PathFormatDescriptor> &second) const {
		return path < second.path;
	}

	bool match(const GenericPath<PathFormatDescriptor> &second) const {
		LOGN1 << "Matching " << *this << " with " << second;

		size_t i = 0, j = 0;
		for(; i < size(); ++i, ++j) {
			if((*this)[i] == "**") {
				if(i+1 == size())
					return true;

				LOGN3 << "Matching recursively";
				for(size_t k = j; k < second.size(); ++k)
					if(subpath(i+1).match(second.subpath(k)))
						return true;
			} else if(j >= second.size()) {
				break;
			} else if(Wildcards::match((*this)[i], second[j])) {
				auto subThis = subpath(1);
				auto subSecond = second.subpath(1);
				return subThis.match(subSecond);
			} else
				return false;
		}
		bool result = (i == size() && j == second.size());
		LOGN3 << "End of search on " << i << ":" << j << " " << result;
		return result;
	}

	GenericPath<PathFormatDescriptor> &operator=(const GenericPath<PathFormatDescriptor> &second) {
		std::unique_lock<std::recursive_mutex> lock(mutex);
		invalidate();
		path = second.path;
		return *this;
	}

	GenericPath<PathFormatDescriptor> &operator/=(const GenericPath<PathFormatDescriptor> &second) {
		std::unique_lock<std::recursive_mutex> lock(mutex);
		if(!path.empty() && !second.path.empty() && second.path[0] != PathFormatDescriptor::separator)
			path += PathFormatDescriptor::separator;
		path += second.path;
		removeExcessingSeparators();
		invalidate();
		return *this;
	}
};

template<typename PathFormatDescriptor>
std::ostream &operator<<(std::ostream &os, const GenericPath<PathFormatDescriptor> &path) {
	os << std::string(path);
	return os;
}

typedef GenericPath<PathFormatDescriptors::Unix> UnixPath;
typedef GenericPath<PathFormatDescriptors::Windows> WindowsPath;

#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
typedef UnixPath NativePath;
#elif BOOST_OS_WINDOWS
typedef WindowsPath NativePath;
#else
#error "Platform not supported"
#endif


class TIALUTILITY_EXPORT Path {
	PathFormat format;
	union {
		GenericPath<PathFormatDescriptors::Unix> unix;
		GenericPath<PathFormatDescriptors::Windows> windows;
	};

public:
	static const size_t npos = std::numeric_limits<size_t>::max();

	template<typename PathFormatDescriptor>
	Path(const GenericPath<PathFormatDescriptor> &path)
		: format(PathFormatDescriptor::format) {
			switch(format) {
			case PathFormat::Unix:
				new(&unix) GenericPath<PathFormatDescriptor>(path);
				break;
			case PathFormat::Windows:
				new(&windows) GenericPath<PathFormatDescriptor>(path);
				break;
			}
		}

	template<typename T>
	T specific() const {
		switch(format) {
		case PathFormat::Unix: return unix;
		case PathFormat::Windows: return windows;
		default: throw Exceptions::InvalidPathFormat(format);
		}
	}

	Path(const std::string &path, PathFormat format);
	Path(const Path &other);
	~Path();
	bool empty() const;
	size_t size() const;
	bool absolute() const;
	bool relative() const;
	bool canonical() const;
	operator std::string() const;
	bool startsWith(const Path &prefix) const;
	std::string basename() const;
	Path subpath(size_t begin, size_t length = npos) const;
	Path canonicalized() const;
	std::string operator[](size_t index) const;
	bool operator==(const Path &second) const;

	friend struct std::hash<Path>;
};

TIALUTILITY_EXPORT std::ostream &operator<<(std::ostream &os, const Path &path);

}
}

#undef TIAL_MODULE
