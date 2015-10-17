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
#include <iostream>
#include <experimental/string_view>
#include "Main.hpp"

namespace Tial {
namespace Testing {

template<typename Suite, typename Case>
void runCase(const Main::SingleCaseCaller &singleCaseCaller) {
	Case testCase;
	testCase._caller = singleCaseCaller;
	testCase._runTestCase();
}

template<typename Suite>
void runCases(const Main::SingleCaseCaller &) {}

template<typename Suite, typename Case, typename... Cases>
void runCases(const Main::SingleCaseCaller &singleCaseCaller) {
	runCase<Suite, Case>(singleCaseCaller);
	runCases<Suite, Cases...>(singleCaseCaller);
}

template<typename... Cases>
class Suite {
	std::string name;
public:
	Suite(const std::experimental::string_view &name): name(name) {
		Tial::Testing::Main::instance().registerTestSuiteFunction(
			std::bind(&Suite<Cases...>::run, this, std::placeholders::_1), name
		);
	}

	void run(const Main::SingleCaseCaller &singleCaseCaller) {
		runCases<Suite<Cases...>, Cases...>(singleCaseCaller);
	}
};

}
}
