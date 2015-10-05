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
#include <TialTesting/TialTesting.hpp>
#include <TialUtility/TialUtility.hpp>

#include <stdexcept>

#define TIAL_MODULE "Test/Tial::Utility::Exception"

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {

void throwException() {
		THROW Tial::Utility::Exception("sample exception");
}

void throwStdException() {
		THROW std::runtime_error("sample exception");
}

class [[Testing::Case]] ThrowAndCatchDirectly {
	void operator()() {
		try {
			throwException();
		} catch(const Tial::Utility::Exception &e) {
			[[Check::Verify]] e.component() == TIAL_MODULE;
			[[Check::Verify]] e.file().substr(e.file().size()-13) == "Exception.cpp";
			[[Check::Verify]] e.line() == 37u;
			[[Check::Verify]] e.function() == "void Tial::Utility::throwException()";
			[[Check::Verify]] e.message() == "sample exception";
		}
	}
};

class [[Testing::Case]] ThrowAndCatchAsStd {
	void operator()() {
		try {
			throwException();
		} catch(const std::exception &e) {
			std::string message(e.what());
			[[Check::Verify]] message.substr(message.size()-16) == "sample exception";
		}
	}
};

class [[Testing::Case]] ThrowStdAndCatch {
	void operator()() {
		try {
			throwStdException();
		} catch(const Tial::Utility::Exception &e) {
			[[Check::Verify]] e.component() == TIAL_MODULE;
			[[Check::Verify]] e.file().substr(e.file().size()-13) == "Exception.cpp";
			[[Check::Verify]] e.line() == 41u;
			[[Check::Verify]] e.function() == "void Tial::Utility::throwStdException()";
			[[Check::Verify]] e.message() == "sample exception";
		}
	}
};

class [[Testing::Case]] ThrowStdAndCatchStd {
	void operator()() {
		try {
			throwStdException();
		} catch(const std::exception &e) {
			std::string message(e.what());
			[[Check::Verify]] message.substr(message.size()-16) == "sample exception";
		}
	}
};

}
}
