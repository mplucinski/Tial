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

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {
namespace [[Testing::Suite]] Algorithm {

static std::vector<char> stringToVector(const std::string &string) {
	std::vector<char> vector;
	for(const auto character: string)
		vector.push_back(character);
	return vector;
}

bool myIsSpace(const char &ch) {
	return ch == ' ';
}

class [[Testing::Case]] Stripping {
public:
	struct Data {
		std::string input, result;
		std::function<bool(const char &ch)> predicate;
	};

	[[Testing::Data]] void data() {
		[[Testing::Data("no spaces")]] Data{"Alan", "Alan", defaultStripPredicate<char>};
		[[Testing::Data("left spaces")]] Data{"  Brad", "Brad", myIsSpace};
		[[Testing::Data("right spaces")]] Data{"Charlie     ", "Charlie", defaultStripPredicate<char>};
		[[Testing::Data("both spaces")]] Data{"   Dolores  ", "Dolores", defaultStripPredicate<char>};
	}

	void operator()(const Data &data) {
		{
			std::string stringInput = data.input;
			strip(stringInput, data.predicate);
			[[Check::Verify]] stringInput == data.result;
		}{
			std::string stringInput = data.input;
			[[Check::Verify]] stripped(stringInput, data.predicate) == data.result;
		}{
			std::experimental::string_view stringViewInput = data.input;
			strip(stringViewInput, data.predicate);
			[[Check::Verify]] stringViewInput == data.result;
		}{
			std::experimental::string_view stringViewInput = data.input;
			[[Check::Verify]] stripped(stringViewInput, data.predicate) == data.result;
		}{
			std::vector<char> vectorInput = stringToVector(data.input);
			strip(vectorInput, data.predicate);
			[[Check::Verify]] vectorInput == stringToVector(data.result);
		}{
			std::vector<char> vectorInput = stringToVector(data.input);
			[[Check::Verify]] stripped(vectorInput, data.predicate) == stringToVector(data.result);
		}
	}
};

class [[Testing::Case]] Splitting {
public:
	struct Data {
		std::string input;
		char separator;
		std::vector<std::string> result;
		std::vector<std::string> resultWithSkipping;

//FIXME: workaround for compiler's crash in Clang/C2
#if (BOOST_COMP_CLANG && BOOST_OS_WINDOWS)
		Data(
			const decltype(Data::input) &input,
			const decltype(Data::separator) &separator,
			const decltype(Data::result) &result,
			const decltype(Data::resultWithSkipping) &resultWithSkipping
		): input(input), separator(separator), result(result), resultWithSkipping(resultWithSkipping) {}
#endif
	};

//FIXME: workaround for compiler's crash in Clang/C2
#if (BOOST_COMP_CLANG && BOOST_OS_WINDOWS)
#define DATA(...) Data(__VA_ARGS__)  
#else
#define DATA(...) Data{__VA_ARGS__}
#endif
	[[Testing::Data]] void data() {
		[[Testing::Data("empty")]] DATA("", ' ',
		                                {},
		                                {});
		[[Testing::Data("space only")]] DATA(" ", ' ',
		                                     {"", ""},
		                                     {});
		[[Testing::Data("spaces only")]] DATA("  ", ' ',
		                                      {"", "", ""},
		                                      {});
		[[Testing::Data("single letter")]] DATA("A", ' ',
		                                        {"A"},
		                                        {"A"});
		[[Testing::Data("single letter with space on left")]] DATA(" A", ' ',
		                                                           {"", "A"},
		                                                           {"A"});
		[[Testing::Data("single letter with space on right")]] DATA("A ", ' ',
		                                                            {"A", ""},
		                                                            {"A"});
		[[Testing::Data("single letter with spaces on both sides")]] DATA(" A ", ' ',
		                                                                  {"", "A", ""},
		                                                                  {"A"});
		[[Testing::Data("two letters with space on left")]] DATA(" AA", ' ',
		                                                         {"", "AA"},
		                                                         {"AA"});
		[[Testing::Data("two letters with space on right")]] DATA("AA ", ' ',
		                                                          {"AA", ""},
		                                                          {"AA"});
		[[Testing::Data("two letters with spaces on both sides")]] DATA(" AA ", ' ',
		                                                                {"", "AA", ""},
		                                                                {"AA"});
		[[Testing::Data("simple splitting")]] DATA("Alan Alvey", ' ',
		                                           {"Alan", "Alvey"},
		                                           {"Alan", "Alvey"});
		[[Testing::Data("with single prefix")]] DATA(" Alan Alvey", ' ',
		                                             {"", "Alan", "Alvey"},
		                                             {"Alan", "Alvey"});
		[[Testing::Data("with double prefix")]] DATA("  Alan Alvey", ' ',
		                                             {"", "", "Alan", "Alvey"},
		                                             {"Alan", "Alvey"});
		[[Testing::Data("with single suffix")]] DATA("Alan Alvey ", ' ',
		                                             {"Alan", "Alvey", ""},
		                                             {"Alan", "Alvey"});
		[[Testing::Data("with double suffix")]] DATA("Alan Alvey  ", ' ',
		                                             {"Alan", "Alvey", "", ""},
		                                             {"Alan", "Alvey"});
		[[Testing::Data("with middle empty")]] DATA("Alan  Alvey", ' ',
		                                            {"Alan", "", "Alvey"},
		                                            {"Alan", "Alvey"});
		[[Testing::Data("three elements")]] DATA("Alan Alvey Johnson", ' ',
		                                         {"Alan", "Alvey", "Johnson"},
		                                         {"Alan", "Alvey", "Johnson"});
		[[Testing::Data("three elements with space on left")]] DATA(" Alan Alvey Johnson", ' ',
		                                                            {"", "Alan", "Alvey", "Johnson"},
		                                                            {"Alan", "Alvey", "Johnson"});
		[[Testing::Data("three elements with space on right")]] DATA("Alan Alvey Johnson ", ' ',
		                                                             {"Alan", "Alvey", "Johnson", ""},
		                                                             {"Alan", "Alvey", "Johnson"});
		[[Testing::Data("three elements with spaces on both sides")]] DATA(" Alan Alvey Johnson ", ' ',
		                                                                   {"", "Alan", "Alvey", "Johnson", ""},
		                                                                   {"Alan", "Alvey", "Johnson"});
		[[Testing::Data("three elements with multiple spaces")]] DATA("Alan    Alvey  Johnson", ' ',
		                                                              {"Alan", "", "", "", "Alvey", "", "Johnson"},
		                                                              {"Alan", "Alvey", "Johnson"});
	}
#undef DATA

	void operator()(const Data &data) {
		{
			[[Check::Verify]] (splitted<std::vector<std::string>>(data.input, data.separator)) == data.result;
		}{
			[[Check::Verify]] (splitted<std::vector<std::string>, SplitFlags::SkipEmpty>(data.input, data.separator)) == data.resultWithSkipping;
		}{
			std::experimental::string_view input = data.input;
			auto result = splitted<std::vector<std::experimental::string_view>>(input, data.separator);
			std::vector<std::experimental::string_view> expectedResult;
			for(auto &i: data.result)
				expectedResult.push_back(i);
			[[Check::Verify]] result == expectedResult;
		}{
			std::experimental::string_view input = data.input;
			auto result = splitted<std::vector<std::experimental::string_view>, SplitFlags::SkipEmpty>(input, data.separator);
			std::vector<std::experimental::string_view> expectedResult;
			for(auto &i: data.resultWithSkipping)
				expectedResult.push_back(i);
			[[Check::Verify]] result == expectedResult;
		}
	}
};

}
}
}
