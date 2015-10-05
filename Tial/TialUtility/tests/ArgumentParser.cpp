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

#include <vector>

#define TIAL_MODULE "Test/Utility::ArgumentParser"

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {
namespace [[Testing::Suite]] ArgumentParser {

std::pair<int, std::unique_ptr<char*, std::function<void(char**)>>> buildArgs(
	const std::vector<std::string> &args
) {
	int argc = args.size();
	std::unique_ptr<char*, std::function<void(char**)>> argv(
		new char*[argc](),
		[argc](char** ptr) {
			for(int i = 0; i < argc; ++i)
				delete ptr[i];
			delete[] ptr;
		}
	);
	for(int i = 0; i < argc; ++i) {
		argv.get()[i] = new char[args[i].size()+1];
		strcpy(argv.get()[i], args[i].c_str());
	}
	return std::make_pair(argc, std::move(argv));
}

struct Foo: public Parameter {
	typedef int ValueType;
	typedef Count::Fixed<1> ValueCount;
	typedef Action::StoreValue Action;
	static constexpr bool required = true;
	static constexpr char option[] = "-f";
	static constexpr char longOption[] = "--foo";
	static constexpr char argumentPlaceholder[] = "FOO";
	static constexpr char description[] = "foo to use";
};
constexpr char Foo::option[];
constexpr char Foo::longOption[];
constexpr char Foo::description[];
constexpr char Foo::argumentPlaceholder[];

struct Bar: public Special::Switch {
	static constexpr char longOption[] = "--bar";
	static constexpr char description[] = "use bar";
};
constexpr char Bar::longOption[];
constexpr char Bar::description[];

struct Help: public Special::Help {
	static constexpr char commandName[] = "Test command";
	static constexpr char command[] = "cmd";
};
constexpr char Help::commandName[];
constexpr char Help::command[];

class [[Testing::Case]] FullParser {
	void operator()() {
		{
			Parser<Foo> parser;

			{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({});
			}{
				[[Check::ThrowEqual(Exceptions::NoValueForArgument("-f"))]] parser.parse({"-f"});
			}{
				auto result = [[Check::NoThrow]] parser.parse({"-f", "42"});
				[[Check::Verify]] (result.get<Foo>()) == 42;
			}{
				[[Check::ThrowEqual(Exceptions::UnmatchedArguments({"-d"}))]] parser.parse({"-f", "42", "-d"});
			}{
				auto args = [[Check::NoThrow]] buildArgs({"cmd"});
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse(args.first, args.second.get());
			}{
				auto args = [[Check::NoThrow]] buildArgs({"cmd", "-f"});
				[[Check::ThrowEqual(Exceptions::NoValueForArgument("-f"))]] parser.parse(args.first, args.second.get());
			}{
				auto args = [[Check::NoThrow]] buildArgs({"cmd", "-f", "42"});
				auto result = [[Check::NoThrow]] parser.parse(args.first, args.second.get());
				[[Check::Verify]] (result.get<Foo>()) == 42;
			}{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({"-V"});
			}{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({"--version"});
			}{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({"-h"});
			}{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({"--help"});
			}
		}{
			Parser<Help, Foo> parser;
			std::string helpString = "Test command\n"
					"    Usage: cmd -f|--foo FOO [-h|--help]\n"
					"\n"
					"Required named arguments:\n"
					"    -f|--foo FOO                        foo to use\n"
					"\n"
					"Optional named arguments:\n"
					"    -h|--help                           print the help and exit\n"
					"\n";

			{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({});
			}{
				[[Check::ThrowEqual(Exceptions::NoValueForArgument("-f"))]] parser.parse({"-f"});
			}{
				auto result = [[Check::NoThrow]] parser.parse({"-f", "42"});
				[[Check::Verify]] (result.get<Help>()) == false;
				[[Check::Verify]] (result.get<Foo>()) == 42;
			}{
				[[Check::ThrowEqual(Exceptions::UnmatchedArguments({"-d"}))]] parser.parse({"-f", "42", "-d"});
			}{
				auto args = [[Check::NoThrow]] buildArgs({"cmd"});
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse(args.first, args.second.get());
			}{
				auto args = [[Check::NoThrow]] buildArgs({"cmd", "-f"});
				[[Check::ThrowEqual(Exceptions::NoValueForArgument("-f"))]] parser.parse(args.first, args.second.get());
			}{
				auto args = [[Check::NoThrow]] buildArgs({"cmd", "-f", "42"});
				auto result = [[Check::NoThrow]] parser.parse(args.first, args.second.get());
				[[Check::Verify]] (result.get<Help>()) == false;
				[[Check::Verify]] (result.get<Foo>()) == 42;
			}{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({"-V"});
			}{
				[[Check::ThrowEqual(Exceptions::NoRequiredArgument("-f"))]] parser.parse({"--version"});
			}{
				std::ostringstream oss;
				auto result = [[Check::NoThrow]] parser.parse({"-h"}, oss);
				[[Check::Verify]] (oss.str()) == helpString;
				[[Check::Verify]] (result.get<Help>()) == true;
				[[Check::ThrowEqual(Exceptions::ArgumentNotSpecified("-f"))]] result.get<Foo>();
			}{
				std::ostringstream oss;
				auto result = [[Check::NoThrow]] parser.parse({"--help"}, oss);
				[[Check::Verify]] (oss.str()) == helpString;
				[[Check::Verify]] (result.get<Help>()) == true;
				[[Check::ThrowEqual(Exceptions::ArgumentNotSpecified("-f"))]] result.get<Foo>();
			}
		}{
			Parser<Help, Bar> parser;
			std::string helpString = "Test command\n"
					"    Usage: cmd [-h|--help] [--bar]\n"
					"\n"
					"Optional named arguments:\n"
					"    -h|--help                           print the help and exit\n"
					"    --bar                               use bar\n"
					"\n";

			{
				auto result = [[Check::NoThrow]] parser.parse({});
				[[Check::Verify]] (result.get<Help>()) == false;
				[[Check::Verify]] (result.get<Bar>()) == false;
			}{
				auto result = [[Check::NoThrow]] parser.parse({"--bar"});
				[[Check::Verify]] (result.get<Help>()) == false;
				[[Check::Verify]] (result.get<Bar>()) == true;
			}{
				std::ostringstream oss;
				auto result = [[Check::NoThrow]] parser.parse({"-h"}, oss);
				[[Check::Verify]] (oss.str()) == helpString;
				[[Check::Verify]] (result.get<Help>()) == true;
				[[Check::ThrowEqual(Exceptions::ArgumentNotSpecified("--bar"))]] result.get<Bar>();
			}{
				std::ostringstream oss;
				auto result = [[Check::NoThrow]] parser.parse({"--help"}, oss);
				[[Check::Verify]] (oss.str()) == helpString;
				[[Check::Verify]] (result.get<Help>()) == true;
				[[Check::ThrowEqual(Exceptions::ArgumentNotSpecified("--bar"))]] result.get<Bar>();
			}
		}
	}
};

}
}
}
