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

#include <ostream>

namespace Tial {
namespace Utility {

struct intend {
    unsigned int count;
	unsigned int depth;

    explicit intend(const unsigned int count, const unsigned int depth = 4) : count(count), depth(depth) { }
};

template<typename Character>
std::basic_ostream<Character> &operator<<(std::basic_ostream<Character> &os, const intend &intendation) {
	for (unsigned int i = 0; i < intendation.count*intendation.depth; ++i)
		os << Character(' ');
	return os;
}

}
}

// NOTE: extending std with own members is not allowed by C++ standard, so may cause problems
#ifndef TIAL_UTILITY_DISABLE_NONSTANDARD_STD_OPERATORS
namespace std {

template<
		typename T,
		typename = decltype(std::declval<T>().begin()),
		typename = typename std::enable_if<
				!std::is_convertible<T, std::string>::value &&
				!std::is_convertible<T, std::experimental::string_view>::value
		>::type
> std::ostream &operator<<(std::ostream &os, const T &collection) {
	for(auto i = collection.begin(); i != collection.end(); ++i) {
		if(i != collection.begin())
			os << ", ";
		os << *i;
	}
	return os;
}

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &pair) {
	return os << pair.first << ": " << pair.second;
}

}
#endif
