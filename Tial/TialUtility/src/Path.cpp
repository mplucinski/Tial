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
#include "Path.hpp"

#include "Logger.hpp"

#define TIAL_MODULE "Tial::Utility::ArgumentParser"

const std::string Tial::Utility::PathFormatDescriptors::_UnixStyle::currentDirectory = ".";
const std::string Tial::Utility::PathFormatDescriptors::_UnixStyle::parentDirectory = "..";

const char Tial::Utility::PathFormatDescriptors::Unix::separator = '/';

Tial::Utility::Exceptions::InvalidPathFormat::InvalidPathFormat(PathFormat format)
	: Exception("Invalid path format"), format(format) {}

bool Tial::Utility::PathFormatDescriptors::Unix::isRoot(const std::string &name) {
	return !name.empty() && name[0] == separator;
}

bool Tial::Utility::PathFormatDescriptors::Unix::isMultiRootFormat() {
	return false;
}

const char Tial::Utility::PathFormatDescriptors::Windows::separator = '\\';

bool Tial::Utility::PathFormatDescriptors::Windows::isRoot(const std::string &name) {
	return name.size() == 2 && name[1] == ':';
}

bool Tial::Utility::PathFormatDescriptors::Windows::isMultiRootFormat() {
	return true;
}

Tial::Utility::Path::Path(const std::string &path, PathFormat format): format(format) {
	switch(format) {
	case PathFormat::Unix:
		new(&unix) GenericPath<PathFormatDescriptors::Unix>(path);
		break;
	case PathFormat::Windows:
		new(&windows) GenericPath<PathFormatDescriptors::Windows>(path);
		break;
	}
}

Tial::Utility::Path::Path(const Path &other): format(other.format) {
	switch(format) {
	case PathFormat::Unix:
		new(&unix) GenericPath<PathFormatDescriptors::Unix>(other.unix);
		break;
	case PathFormat::Windows:
		new(&windows) GenericPath<PathFormatDescriptors::Windows>(other.windows);
		break;
	}
}

Tial::Utility::Path::~Path() {
	switch(format) {
	case PathFormat::Unix:
		unix.~GenericPath<PathFormatDescriptors::Unix>();
		break;
	case PathFormat::Windows:
		windows.~GenericPath<PathFormatDescriptors::Windows>();
		break;
	}
}

#define TIAL_UTILITY_PATH__FORMAT_DISPATCH \
	LOGN3 << "Dispatch"; \
	switch(OBJECT .format) { \
	case Tial::Utility::PathFormat::Unix: \
		EXPRESSION(unix, Tial::Utility::PathFormatDescriptors::Unix); \
		break; \
	case Tial::Utility::PathFormat::Windows: \
		EXPRESSION(windows, Tial::Utility::PathFormatDescriptors::Windows); \
		break; \
	default: \
		throw std::invalid_argument("Unknown path format"); \
	}

#define OBJECT (*this)

bool Tial::Utility::Path::empty() const {
#define EXPRESSION(x, y) return OBJECT.x.empty()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

size_t Tial::Utility::Path::size() const {
#define EXPRESSION(x, y) return OBJECT.x.size()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

bool Tial::Utility::Path::absolute() const {
#define EXPRESSION(x, y) return OBJECT.x.absolute()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

bool Tial::Utility::Path::relative() const {
#define EXPRESSION(x, y) return OBJECT.x.relative()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

bool Tial::Utility::Path::canonical() const {
#define EXPRESSION(x, y) return OBJECT.x.canonical()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

Tial::Utility::Path::operator std::string() const {
#define EXPRESSION(x, y) return OBJECT.x.operator std::string()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

bool Tial::Utility::Path::startsWith(const Path &prefix) const {
	if(format != prefix.format)
		throw std::invalid_argument("Incompatible path used in startsWith");

#define EXPRESSION(x, y) return OBJECT.x.startsWith(prefix.specific<GenericPath<y>>());
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

std::string Tial::Utility::Path::basename() const {
#define EXPRESSION(x, y) return OBJECT.x.basename()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

Tial::Utility::Path Tial::Utility::Path::subpath(size_t begin, size_t length) const {
#define EXPRESSION(x, y) return OBJECT.x.subpath(begin, length)
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

Tial::Utility::Path Tial::Utility::Path::canonicalized() const {
#define EXPRESSION(x, y) return OBJECT.x.canonicalized()
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

std::string Tial::Utility::Path::operator[](size_t index) const {
#define EXPRESSION(x, y) return OBJECT.x.operator[](index)
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

bool Tial::Utility::Path::operator==(const Path &second) const {
	if(format != second.format)
		return false;

#define EXPRESSION(x, y) return OBJECT.x == second.x
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
}

#undef OBJECT

std::ostream &Tial::Utility::operator<<(std::ostream &os, const Path &path) {
	return os << std::string(path);
}

std::size_t std::hash<Tial::Utility::Path>::operator()(
	const Tial::Utility::Path &path
) const {
#define OBJECT path
#define EXPRESSION(x, y) return std::hash<Tial::Utility::GenericPath<y>>()(OBJECT.x);
	TIAL_UTILITY_PATH__FORMAT_DISPATCH
#undef EXPRESSION
#undef OBJECT
}
