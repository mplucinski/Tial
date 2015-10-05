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

#define TIAL_MODULE "Test/Tial::Utility::String"

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {
namespace [[Testing::Suite]] Strings {

class [[Testing::Case]] EscapeBytes {
public:
	struct Data {
		std::string input, output;
	};

	[[Testing::Data]] void data() {
		[[Testing::Data("no escapes")]] Data{"abcd", "abcd"};
		[[Testing::Data("mixed")]] Data{
				"\x0\x1\x2\x1d\x1e\x1f 0123abcdABCD\x7F\x80\x81\xFD\xFE\xFF"s,
				"\\x0\\x1\\x2\\x1d\\x1e\\x1f 0123abcdABCD\\x7f\\x80\\x81\\xfd\\xfe\\xff"
		};
		[[Testing::Data("only escapes")]] Data{
				"\x0\x1\x2\x1d\x1e\x1f\x7F\x80\x81\xFD\xFE\xFF"s,
				"\\x0\\x1\\x2\\x1d\\x1e\\x1f\\x7f\\x80\\x81\\xfd\\xfe\\xff"
		};
	}

	void operator()(const Data &data) {
		{
			std::string string = data.input;
			escapeBytes(string);
			LOGD << data.input << " -> " << string;
			[[Check::Verify]] string == data.output;
		}{
			[[Check::Verify]] escapedBytes(data.input) == data.output;
		}
	}
};

}
}
}
