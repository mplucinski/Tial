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

#include "Logger.hpp"

#include <iostream>

#define TIAL_MODULE "Tial::Utility::Wildcards"

namespace Tial {
namespace Utility {
namespace Wildcards {

template<typename CharacterType, typename IteratorType>
bool _matchWildcard(
	IteratorType patternBegin, IteratorType patternEnd,
	IteratorType stringBegin, IteratorType stringEnd
) {
	typedef typename std::basic_string<CharacterType> String;

	LOGN2 << "Matching " << String(patternBegin, patternEnd)
		<< " with " << String(stringBegin, stringEnd);

	CharacterType one = CharacterType('?');
	CharacterType any = CharacterType('*');

	while(patternBegin != patternEnd && stringBegin != stringEnd) {
		if(*patternBegin == any) {
			IteratorType subpatternBegin = patternBegin+1;
			IteratorType substringBegin = stringBegin;
			LOGN3 << "Testing subpattern *" << String(subpatternBegin, patternEnd) << " on " << String(substringBegin, stringEnd);
			while(substringBegin != stringEnd) {
				LOGN3 << "Calling subpattern on " << String(substringBegin, stringEnd);
				if(_matchWildcard<CharacterType, IteratorType>(subpatternBegin, patternEnd, substringBegin, stringEnd)) {
					LOGN3 << "Subpattern matched, result true";
					return true;
				}
				substringBegin++;
			}
			if(subpatternBegin == patternEnd) {
				LOGN3 << "Captured all pattern characters";
				return true;
			}
		} else if(*patternBegin != one && *patternBegin != *stringBegin) {
			LOGN3 << "Not matched, result false";
			return false;
		}
		++patternBegin;
		++stringBegin;
	}
	bool result = (patternBegin == patternEnd && stringBegin == stringEnd);
	LOGN3 << "Reached end of matching, result " << result;
	return result;
}

template<typename CharacterType, typename IteratorType>
bool _match(
	IteratorType patternBegin, IteratorType patternEnd,
	IteratorType stringBegin, IteratorType stringEnd
) {
	typedef typename std::basic_string<CharacterType> String;
	LOGN1 << "Mating " << String(patternBegin, patternEnd) << " with "
		<< String(stringBegin, stringEnd);
	return _matchWildcard<CharacterType, IteratorType>(
		patternBegin, patternEnd, stringBegin, stringEnd
	);
}

inline bool match(const std::string &pattern, const std::string &string) {
	return _match<char>(
		pattern.cbegin(), pattern.cend(),
		string.cbegin(), string.cend()
	);
}

inline bool match(const std::u16string &pattern, const std::u16string &string) {
	return _match<char16_t>(
		pattern.cbegin(), pattern.cend(),
		string.cbegin(), string.cend()
	);
}

inline bool match(const std::u32string &pattern, const std::u32string &string) {
	return _match<char32_t>(
		pattern.cbegin(), pattern.cend(),
		string.cbegin(), string.cend()
	);
}

}
}
}

#undef TIAL_MODULE
