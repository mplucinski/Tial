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

#define TIAL_MODULE "Test/Tial::Utility::Directory"

const std::string sample = "TestTialUtility_sample";

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {

class [[Testing::Case]] IterateDirectoryContent {
	void operator()() {
		Tial::Utility::NativeDirectory directory = Tial::Utility::NativeDirectory::current();
		bool found = false;
		do {
			LOGD << "Looking for \"" << sample << "\" in \"" << directory.path() << "\"";
			for(const auto child: directory.content())
				if(child->path().basename() == sample) {
					found = true;
					directory = directory.path()/sample;
				}

			if(!found)
				directory = directory.path().parent();
		} while(!found);
		auto content = directory.content();

		Tial::Utility::NativeDirectory::Entries entries;

		auto iterator = content.cbegin();
		[[Check::Verify]] iterator != content.cend();
		std::shared_ptr<Tial::Utility::NativeDirectory::Entry> entry = *iterator;
		entries.push_back(entry);
		++iterator;

		entry = *iterator;
		entries.push_back(entry);
		iterator++;

		entry = *iterator;
		entries.push_back(entry);
		++iterator;

		[[Check::Verify]] iterator == content.cend();

		auto sorter = [](
			const std::shared_ptr<Tial::Utility::NativeDirectory::Entry>& e1,
			const std::shared_ptr<Tial::Utility::NativeDirectory::Entry>& e2
		) {
			return e1->path() < e2->path();
		};

		std::sort(entries.begin(), entries.end(), sorter);

		[[Check::Verify]] std::string(entries[0]->path().basename()) == "bar";
		[[Check::Verify]] (!std::dynamic_pointer_cast<Tial::Utility::NativeDirectory>(entries[0]));
		[[Check::Verify]] std::string(entries[1]->path().basename()) == "doh";
		[[Check::Verify]] (static_cast<bool>(std::dynamic_pointer_cast<Tial::Utility::NativeDirectory>(entries[1])));
		[[Check::Verify]] std::string(entries[2]->path().basename()) == "foo";
		[[Check::Verify]] (!std::dynamic_pointer_cast<Tial::Utility::NativeDirectory>(entries[2]));
	}
};

}
}
