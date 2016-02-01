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
#include <memory>
#include <typeindex>
#include <utility>
#include <experimental/string_view>

#include <boost/current_function.hpp>
#include <boost/predef.h>

namespace Tial {
namespace Utility {

template<typename T>
std::type_index typeId(T &&t) {
	return typeid(std::forward<T>(t));
}

namespace Language {
namespace StringViewLiteral {

constexpr std::experimental::string_view operator""_sv(const char *s, std::size_t size) {
	return std::experimental::string_view(s, size);
}

}

template<typename Stream, std::ios_base::openmode defaultOpenMode>
class FStreamWrapper: public Stream {
	void enableExceptions() {
		this->exceptions(Stream::failbit | Stream::badbit);
	}
public:
	FStreamWrapper() {
		enableExceptions();
	}

	template<typename String>
	explicit FStreamWrapper(const String& filename, std::ios_base::openmode mode = defaultOpenMode) {
		enableExceptions();
		this->open(filename, mode);
	}

	FStreamWrapper(FStreamWrapper &&other): Stream{std::move(other)} {
		enableExceptions();
	}

	FStreamWrapper(const FStreamWrapper &other) = delete;
};

using FStream = FStreamWrapper<std::fstream, std::ios_base::in | std::ios_base::out>;
using IFStream = FStreamWrapper<std::ifstream, std::ios_base::in>;
using OFStream = FStreamWrapper<std::ofstream, std::ios_base::out>;

namespace Cast {

#define TIAL_UTILITY_LANGUAGE_CAST(name, ptrContainer, elementType, castOp) \
	template< \
		typename ElementTarget, \
		typename PointerSource, \
		typename ElementSource = elementType \
	> typename std::enable_if< \
		std::is_same< \
				typename std::remove_reference<PointerSource>::type, \
				ptrContainer<ElementSource> \
		>::value, \
		ptrContainer<ElementTarget> \
	>::type name(PointerSource &&source) { \
		ElementSource *const sourcePtr = source.get(); \
		ElementTarget *const targetPtr = castOp<ElementTarget*>(sourcePtr); \
		if(!targetPtr) \
			return nullptr; \
		source.release(); \
		return ptrContainer<ElementTarget>(targetPtr); \
	}

#define TIAL_UTILITY_LANGUAGE_CAST_SET(ptrContainer, elementType) \
	TIAL_UTILITY_LANGUAGE_CAST(statically,  ptrContainer, elementType, static_cast ) \
	TIAL_UTILITY_LANGUAGE_CAST(dynamically, ptrContainer, elementType, dynamic_cast)

TIAL_UTILITY_LANGUAGE_CAST_SET(std::unique_ptr, typename std::remove_reference<PointerSource>::type::element_type)
}

}

}

namespace Cast = Utility::Language::Cast;

}

#define unused(x) ((void)x);

#define TIAL_UTILITY_LANGUAGE_FUNCTION_SIGNATURE BOOST_CURRENT_FUNCTION

#define TIAL_UTILITY_LANGUAGE_CURRENT_MODULE (std::experimental::string_view(TIAL_MODULE))

#ifndef TIAL_UTILITY_LANGUAGE_DISABLE_STRING_VIEW_LITERAL

using namespace Tial::Utility::Language::StringViewLiteral;

#endif
