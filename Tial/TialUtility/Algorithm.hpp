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

#include <functional>

#include <experimental/string_view>

namespace Tial {
namespace Utility {
namespace Algorithm {

template<typename T>
void removePrefix(T &collection) {
	assert(!collection.empty());
	collection.erase(collection.begin());
}

template<typename T>
void removeSuffix(T &collection) {
	assert(!collection.empty());
	auto it = collection.end();
	--it;
	collection.erase(it);
}

template<typename Element>
bool defaultStripPredicate(const Element &element) {
	return element == Element();
}

template<>
TIALUTILITY_EXPORT void removePrefix(std::experimental::string_view &stringView);

template<>
TIALUTILITY_EXPORT void removeSuffix(std::experimental::string_view &stringView);

template<>
TIALUTILITY_EXPORT bool defaultStripPredicate(const char &ch);

template<typename Collection, typename Element = typename Collection::value_type>
void stripLeft(Collection &collection, const std::function<bool(const Element&)> predicate = defaultStripPredicate<Element>) {
	while(!collection.empty() && predicate(collection.front()))
		removePrefix(collection);
}

template<typename Collection, typename Element = typename Collection::value_type>
void stripRight(Collection &collection, const std::function<bool(const Element&)> predicate = defaultStripPredicate<Element>) {
	while(!collection.empty() && predicate(collection.back()))
		removeSuffix(collection);
}

template<typename Collection, typename Element = typename Collection::value_type>
void strip(Collection &collection, const std::function<bool(const Element&)> predicate = defaultStripPredicate<Element>) {
	stripLeft(collection, predicate);
	stripRight(collection, predicate);
}

template<typename Collection, typename Element = typename Collection::value_type>
Collection stripped(const Collection &collection, const std::function<bool(const Element&)> predicate = defaultStripPredicate<Element>) {
	Collection copy(collection);
	strip(copy, predicate);
	return copy;
}

namespace Impl {
	template<typename OutputContainer, typename Collection, typename Element>
	struct Splitted {
		static OutputContainer splitted(const Collection &collection, const Element &element) {
			OutputContainer output;
			if(!collection.empty()) {
				output.push_back(Collection());
				for(auto i = collection.begin(); i != collection.end(); ++i) {
					if(*i == element)
						output.push_back(Collection());
					else
						(output.end()-1)->push_back(*i);
				}
			}
			return output;
		}
	};

	template<typename OutputContainer, typename Element>
	struct Splitted<OutputContainer, std::experimental::string_view, Element> {
		static OutputContainer splitted(const std::experimental::string_view &collection, const Element &element) {
			OutputContainer output;
			if(!collection.empty()) {
				unsigned int start = 0, i = 0;
				for(; i != collection.size();) {
					if(collection[i] == element) {
						output.push_back(collection.substr(start, i-start));
						++i;
						start = i;
					} else
						++i;
				}
				output.push_back(collection.substr(start, i));
			}
			return output;
		}
	};
}

template<typename OutputContainer, typename Collection, typename Element = char>
OutputContainer splitted(const Collection &collection, const Element &element = ' ') {
	return Impl::Splitted<OutputContainer, Collection, Element>::splitted(collection, element);
}

}
}
}
