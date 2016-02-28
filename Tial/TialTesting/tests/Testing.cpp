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

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] TestingOfTesting {
namespace [[Testing::Suite]] BasicTests
{
	class [[Testing::Case]] Fail {
		void operator()() {
			[[Check::ExpectFail]];
			[[Check::Fail]];
		}
	};

	class [[Testing::Case]] Verify
	{
		void operator()() {
			[[Check::Verify]] true;

			[[Check::ExpectFail]];
			[[Check::Verify]] false;
		}
	};

	template<typename Case>
	class [[Testing::CaseBase]] VerifyEqual {
	public:
		void operator()() {
			auto x = Case::get();
			(void)x;

			[[Check::Verify]] x == Case::same;

			[[Check::ExpectFail]];
			[[Check::Verify]] x == Case::other;

			[[Check::ExpectFail]];
			[[Check::Verify]] x != Case::same;

			[[Check::Verify]] x != Case::other;

			[[Check::ExpectFail]];
			[[Check::Verify]] Case::other < x;

			[[Check::Verify]] x < Case::other;

			[[Check::ExpectFail]];
			[[Check::Verify]] Case::other <= x;

			[[Check::Verify]] x <= Case::other;
			[[Check::Verify]] x <= Case::same;

			[[Check::ExpectFail]];
			[[Check::Verify]] x > Case::other;

			[[Check::ExpectFail]];
			[[Check::Verify]] x > Case::other;

			[[Check::ExpectFail]];
			[[Check::Verify]] x >= Case::other;

			[[Check::Verify]] x >= Case::same;
		}

		static constexpr auto get() {
			return Case::same;
		}
	};

	class [[Testing::Case]] VerifyEqualInt: public VerifyEqual<VerifyEqualInt> {
	public:
		static constexpr int same = 3;
		static constexpr int other = 4;
	};
	constexpr int VerifyEqualInt::same;
	constexpr int VerifyEqualInt::other;

	class [[Testing::Case]] VerifyEqualString: public VerifyEqual<VerifyEqualString> {
	public:
		static constexpr std::experimental::string_view same = "bar"_sv;
		static constexpr std::experimental::string_view other = "foo"_sv;
	};
	constexpr std::experimental::string_view VerifyEqualString::same;
	constexpr std::experimental::string_view VerifyEqualString::other;

	class [[Testing::Case]] VerifyArgumentsAccessedOnce {
	public:
		class Argument {
			int value;
		public:
			int accessed = 0;

			explicit Argument(int value): value(value) {}

			int get() {
				++accessed;
				return value;
			}
		};

		void operator()() {
			{
				Argument x(2), y(3);
				[[Check::ExpectFail]];
				[[Check::Verify]] x.get() == y.get();
				[[Check::Verify]] x.accessed == 1;
				[[Check::Verify]] y.accessed == 1;
			}{
				Argument x(2), y(3);
				[[Check::Verify]] x.get() != y.get();
				[[Check::Verify]] x.accessed == 1;
				[[Check::Verify]] y.accessed == 1;
			}{
				Argument x(2), y(3);
				[[Check::Verify]] x.get() < y.get();
				[[Check::Verify]] x.accessed == 1;
				[[Check::Verify]] y.accessed == 1;
			}{
				Argument x(2), y(3);
				[[Check::Verify]] x.get() <= y.get();
				[[Check::Verify]] x.accessed == 1;
				[[Check::Verify]] y.accessed == 1;
			}{
				Argument x(2), y(3);
				[[Check::ExpectFail]];
				[[Check::Verify]] x.get() > y.get();
				[[Check::Verify]] x.accessed == 1;
				[[Check::Verify]] y.accessed == 1;
			}{
				Argument x(2), y(3);
				[[Check::ExpectFail]];
				[[Check::Verify]] x.get() >= y.get();
				[[Check::Verify]] x.accessed == 1;
				[[Check::Verify]] y.accessed == 1;
			}
		}
	};
}

namespace [[Testing::Suite]] SpecificTests {
	class ComparableException: public std::exception {
		std::string message;
	public:
		ComparableException(const std::string &message): message(message) {}
		virtual const char *what() const noexcept override {
			return message.c_str();
		}

		bool operator==(const ComparableException &other) const {
			return message == other.message;
		}

		bool operator!=(const ComparableException &other) const {
			return !((*this) == other);
		}
	};

	class [[Testing::Case]] Throws {
		void operator()() {
			[[Check::Throw(std::exception)]] throwException();

			[[Check::ExpectFail]];
			[[Check::Throw(std::exception)]] noThrowException();

			[[Check::ExpectFail]];
			[[Check::NoThrow]] throwException();

			[[Check::NoThrow]] noThrowException();

			[[Check::ExpectFail]];
			[[Check::ThrowExact(std::exception)]] throwException();

			[[Check::ExpectFail]];
			[[Check::ThrowExact(std::exception)]] noThrowException();

			[[Check::ThrowExact(ComparableException)]] throwException();

			[[Check::ExpectFail]];
			[[Check::ThrowExact(ComparableException)]] noThrowException();

			[[Check::ExpectFail]];
			[[Check::ThrowEqual(ComparableException("asd"))]] noThrowException();

			[[Check::ExpectFail]];
			[[Check::ThrowEqual(ComparableException("asd"))]] throwException();

			[[Check::ExpectFail]];
			[[Check::ThrowEqual(ComparableException("foo"))]] noThrowException();

			[[Check::ThrowEqual(ComparableException("foo"))]] throwException();

			{
				int r = [[Check::NoThrow]] noThrowReturn();
				[[Check::Verify]] r == 42;
			}

			{
				[[Check::ExpectFail]];
				int r = [[Check::NoThrow]] throwReturn();
				[[Check::Verify]] r == int();
			}
		}

		static void throwException() {
			throw ComparableException("foo");
		}

		static void noThrowException() {}

		static int noThrowReturn() {
			return 42;
		}

		static int throwReturn() {
			throw ComparableException("baz");
		}
	};

	class [[Testing::Case]] Thread {
		void operator()() {
			Testing::Thread first([&](){
				[[Check::Verify]] executions >= 0;
				for(int i = 0; i < 1000; ++i)
					executions++;
			});

			auto second(first);

			first.operator()("first");
			second.operator()("second");

			first.join();
			second.join();

			[[Check::Verify]] executions == 2000;
		}

		std::atomic<int> executions{0};
	};
}

namespace [[Testing::Suite]] DataTests {
	class [[Testing::Case]] Data {
		struct D {
			std::string first;
			std::string second;
			bool same;
		};

		[[Testing::Data]] void data() {
			[[Testing::Data("two foos")]]    D{"foo"s, "foo", true};
			[[Testing::Data("foo and bar")]] D{"foo"s, "bar", false};
		}

		void operator()(const D &d) {
			if(!d.same)
				[[Check::ExpectFail]];
			[[Check::Verify]] d.first == d.second;
		}
	};
}

namespace [[Testing::Suite]] Internals {
	class NonStringable {};

	class [[Testing::Case]] ArgToString {
		void operator()() {
			[[Check::Verify]] Check::argToString("123") == "123";
			[[Check::Verify]] Check::argToString(123) == "123";
			[[Check::Verify]] Check::argToString(NonStringable()) == "<cannot convert to string>";
		}
	};

	class [[Testing::Case]] LineNoInGeneratedFile {
		void operator()() {
			[[Check::Verify]] __LINE__ == 299;
		}
	};
}

namespace [[Testing::Suite]] PreprocessorPerformance {
	class [[Testing::Case]] ManyVerifies {
		void operator()() {
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
			[[Check::Verify]] true;
		}
	};
}

}
