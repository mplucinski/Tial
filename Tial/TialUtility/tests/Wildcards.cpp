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
namespace [[Testing::Suite]] Wildcards {

class [[Testing::Case]] NoWildcards {
	void operator()() {
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Kaaawa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Saarnaama", "Saarnaama");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("Sarnaama", "Saarnaama");
	}
};

class [[Testing::Case]] SingleCharacters {
	void operator()() {
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?aaawa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("K?aawa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Ka?awa", "Kaaawa");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("Ka?wa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Kaa?wa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Kaaa?a", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Kaaaw?", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("K?aaw?", "Kaaawa");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("B?aaw?", "Kaaawa");
	}
};

class [[Testing::Case]] RepeatingSequences {
	void operator()() {
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aawa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*awa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*wa", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*a", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aarnaam*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*arnaam*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*rnaam*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*naam*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aam*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*am*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*m*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aarnaam*", "Saarnaama");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("*aarmaam*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aarnaa*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aarna*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aarn*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aar*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aa*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*a*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aar*aa*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*ar*aa*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*r*aa*", "Saarnaama");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("*p*aa*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*r*a*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*aa*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*r*n*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*arna*", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Saarnaama", "Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Sa*aa*", "Saarnaama");
	}
};

class [[Testing::Case]] AsteriskInString {
	void operator()() {
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Slav*ija", "Slavija* Slavija");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("Slav*vija", "Slavija* Slavija");
	}
};

class [[Testing::Case]] Mixed {
	void operator()() {
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?", "A");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("?", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*?", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("*?*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?**", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?*aw*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?*aw?", "Kaaawa");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("?*av*", "Kaaawa");
		[[Check::Verify]] !Tial::Utility::Wildcards::match("?*av?", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?a*a", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?a?a*a", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("??*aw*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("???*aw*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("???***aw*", "Kaaawa");
		[[Check::Verify]]  Tial::Utility::Wildcards::match("?*aw***", "Kaaawa");
	}
};

class [[Testing::Case]] OtherCodings {
	void operator()() {
		[[Check::Verify]]  Tial::Utility::Wildcards::match(u"*aar*aa*", u"Saarnaama");
		[[Check::Verify]]  Tial::Utility::Wildcards::match(U"*aar*aa*", U"Saarnaama");
	}
};

}
}
}
