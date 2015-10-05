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

#include <unordered_map>
#include <unordered_set>
#include <vector>

#define TIAL_MODULE "Test/Tial::Utility::StreamOperator"

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {

class [[Testing::Case]] VectorToOstream {
public:
	struct Data {
		std::vector<std::string> input;
		std::string result;
	};


	[[Testing::Data]] void data() {
		[[Testing::Data("no elements")]] Data{{}, ""};
		[[Testing::Data("one elements")]] Data{{"Alan"}, "Alan"};
		[[Testing::Data("two elements")]] Data{{"Alan", "Alvey"}, "Alan, Alvey"};
		[[Testing::Data("three elements")]] Data{{"Alan", "Alvey", "Johnson"}, "Alan, Alvey, Johnson"};
	}

	void operator()(const Data &data) {
		std::ostringstream oss;
		oss << data.input;
		[[Check::Verify]] oss.str() == data.result;
	}
};

class [[Testing::Case]] UnorderedMapToOstream {
public:
	struct Data {
		std::unordered_map<std::string, std::string> input;
		std::unordered_set<std::string> results;
	};

	[[Testing::Data]] void data() {
		[[Testing::Data("no elements")]] Data{
				{},
				{""}
		};
		[[Testing::Data("one elements")]] Data{
				{{"Great Britain", "London"}},
				{"Great Britain: London"}
		};
		[[Testing::Data("two elements")]] Data{
				{{"Great Britain", "London"}, {"Germany", "Berlin"}},
				{"Great Britain: London, Germany: Berlin", "Germany: Berlin, Great Britain: London"}
		};
		[[Testing::Data("three elements")]] Data{
				{{"Great Britain", "London"}, {"Germany", "Berlin"}, {"France", "Paris"}},
				{
					"Great Britain: London, Germany: Berlin, France: Paris",
					"Great Britain: London, France: Paris, Germany: Berlin",
					"Germany: Berlin, Great Britain: London, France: Paris",
					"Germany: Berlin, France: Paris, Great Britain: London",
					"France: Paris, Germany: Berlin, Great Britain: London",
					"France: Paris, Great Britain: London, Germany: Berlin"
				}
		};
	}

	void operator()(const Data &data) {
		std::ostringstream oss;
		oss << data.input;
		auto pred = [actual = oss.str()](const std::string &expected) {
			return expected == actual;
		};
		[[Check::Verify]] std::any_of(data.results.begin(), data.results.end(), pred);
	}
};

class [[Testing::Case]] Intendation {
	struct Data {
		unsigned int count;
		std::string result;
	};

	[[Testing::Data]] void data() {
		[[Testing::Data("0")]] Data{0, ""};
		[[Testing::Data("1")]] Data{1, " "};
		[[Testing::Data("2")]] Data{2, "  "};
		[[Testing::Data("3")]] Data{3, "   "};
	}

	void operator()(const Data &data) {
		{
			std::ostringstream oss;
			oss << intend(data.count, 1);
			[[Check::Verify]] oss.str() == data.result;
		}{
			std::ostringstream oss;
			oss << intend(data.count, 2);
			[[Check::Verify]] oss.str() == data.result+data.result;
		}
	}
};

}
}
