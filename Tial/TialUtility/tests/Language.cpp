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
namespace [[Testing::Suite]] Language {

class [[Testing::Case]] TypeId {
	void operator()() {
		std::string str;
		[[Check::Verify]] typeId(str) == typeid(str);
	}
};

class [[Testing::Case]] StringViewLiteralTest {
	void operator()() {
		[[Check::Verify]] "This is a sample string"_sv.to_string() == "This is a sample string"s;
	}
};

namespace Objs {

class Base {
public:
	virtual ~Base() {} // to enable polymorphism
};

class Derived1 : public Base {};
class Derived2 : public Base {};

}

class [[Testing::Case]] StaticallyCastTest {
public:
	void operator()() {
		{ // lvalue cases
			Objs::Derived1 *rawPtr = new Objs::Derived1;
			std::unique_ptr<Objs::Derived1> ptr(rawPtr);
			[[Check::Verify]] (Cast::statically<Objs::Base>(ptr)).get() == static_cast<Objs::Base*>(rawPtr);
			[[Check::Verify]] ptr == nullptr;
		}{
			Objs::Derived2 *rawPtr = new Objs::Derived2;
			std::unique_ptr<Objs::Derived2> ptr(rawPtr);
			[[Check::Verify]] (Cast::statically<Objs::Base>(ptr)).get() == static_cast<Objs::Base*>(rawPtr);
			[[Check::Verify]] ptr == nullptr;
		}{
			Objs::Derived1 *rawPtr = new Objs::Derived1;
			std::unique_ptr<Objs::Derived1> ptr(rawPtr);
//			(Cast::statically<Objs::Derived2>(ptr)) == nullptr; // <- non-compileable
			[[Check::Verify]] ptr != nullptr;
		}{ // rvalue cases
			Objs::Derived1 *rawPtr = new Objs::Derived1;
			std::unique_ptr<Objs::Derived1> ptr(rawPtr);
			[[Check::Verify]] (Cast::statically<Objs::Base>(std::move(ptr))).get() == static_cast<Objs::Base*>(rawPtr);
			[[Check::Verify]] ptr == nullptr;
		}{
			Objs::Derived2 *rawPtr = new Objs::Derived2;
			std::unique_ptr<Objs::Derived2> ptr(rawPtr);
			[[Check::Verify]] (Cast::statically<Objs::Base>(std::move(ptr))).get() == static_cast<Objs::Base*>(rawPtr);
			[[Check::Verify]] ptr == nullptr;
		}{
			Objs::Derived1 *rawPtr = new Objs::Derived1;
			std::unique_ptr<Objs::Derived1> ptr(rawPtr);
//			(Cast::statically<Objs::Derived2>(std::move(ptr))) == nullptr; // <- non-compileable
			[[Check::Verify]] ptr != nullptr;
		}
	}
};

class [[Testing::Case]] DynamicallyCastTest {
public:
	void operator()() {
		{ // lvalue cases
			std::unique_ptr<Objs::Base> ptr(new Objs::Base);
			[[Check::Verify]] (Cast::dynamically<Objs::Derived1>(ptr)) == nullptr;
			[[Check::Verify]] (Cast::dynamically<Objs::Derived2>(ptr)) == nullptr;
			[[Check::Verify]] ptr != nullptr;
		}{
			Objs::Base *rawPtr = new Objs::Derived2;
			std::unique_ptr<Objs::Base> ptr(rawPtr);

			[[Check::Verify]] (Cast::dynamically<Objs::Derived1>(ptr)) == nullptr;
			[[Check::Verify]] ptr != nullptr;

			[[Check::Verify]] (Cast::dynamically<Objs::Derived2>(ptr)).get() == dynamic_cast<Objs::Derived2*>(rawPtr);
			[[Check::Verify]] ptr == nullptr;
		}{ // rvalue cases
			std::unique_ptr<Objs::Base> ptr(new Objs::Base);
			[[Check::Verify]] (Cast::dynamically<Objs::Derived1>(std::move(ptr))) == nullptr;
			[[Check::Verify]] (Cast::dynamically<Objs::Derived2>(std::move(ptr))) == nullptr;
			[[Check::Verify]] ptr != nullptr;
		}{
			Objs::Base *rawPtr = new Objs::Derived2;
			std::unique_ptr<Objs::Base> ptr(rawPtr);

			[[Check::Verify]] (Cast::dynamically<Objs::Derived1>(std::move(ptr))) == nullptr;
			[[Check::Verify]] ptr != nullptr;

			[[Check::Verify]] (Cast::dynamically<Objs::Derived2>(std::move(ptr))).get() == dynamic_cast<Objs::Derived2*>(rawPtr);
			[[Check::Verify]] ptr == nullptr;
		}
	}
};

}
}
}
