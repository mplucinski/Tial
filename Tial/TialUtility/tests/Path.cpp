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
namespace [[Testing::Suite]] TestPath {

class [[Testing::Case]] Construction {
	void operator()() {
		{
			UnixPath path;
			[[Check::Verify]] path.empty();
			[[Check::Verify]] path.size() == 0u;
			[[Check::Verify]] std::string(path) == "";
			[[Check::Verify]] path.subpath(0) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 1);
			[[Check::Throw(std::out_of_range)]] path[0];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("");
			[[Check::Verify]] path.empty();
			[[Check::Verify]] path.size() == 0u;
			[[Check::Verify]] std::string(path) == "";
			[[Check::Verify]] path.subpath(0) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 1);
			[[Check::Throw(std::out_of_range)]] path[0];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "/";
			[[Check::Verify]] path.subpath(0) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 2);
			[[Check::Verify]] path.subpath(1) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Throw(std::out_of_range)]] path[1];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "/";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("//");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "/";
			[[Check::Verify]] path.subpath(0) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 2);
			[[Check::Verify]] path.subpath(1) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Throw(std::out_of_range)]] path[1];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "/";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("Foo");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "Foo";
			[[Check::Verify]] path.subpath(0) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 2);
			[[Check::Verify]] path.subpath(1) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 1);
			[[Check::Verify]] path[0] == "Foo";
			[[Check::Throw(std::out_of_range)]] path[1];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "Foo";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/Foo");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/Foo";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Foo";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("//Foo");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/Foo";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Foo";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/Foo/");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/Foo";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Foo";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("//Foo//");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/Foo";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Foo";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("Foo/Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "Foo/Bar";
			[[Check::Verify]] path.subpath(0) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "Foo";
			[[Check::Verify]] path[1] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] path.startsWith("Foo");
			[[Check::Verify]] path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/Foo/Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo/Bar");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 3) == UnixPath("/Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 2) == UnixPath("Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(2, 1) == UnixPath("Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/Foo//Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo/Bar");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 3) == UnixPath("/Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 2) == UnixPath("Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(2, 1) == UnixPath("Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("//Foo/Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo/Bar");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 3) == UnixPath("/Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 2) == UnixPath("Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(2, 1) == UnixPath("Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("//Foo//Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo/Bar");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 3) == UnixPath("/Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 2) == UnixPath("Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(2, 1) == UnixPath("Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/Foo//Bar/");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo/Bar");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 3) == UnixPath("/Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 2) == UnixPath("Foo/Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(2, 1) == UnixPath("Bar");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("//Foo//Bar//Asd");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 4u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar/Asd";
			[[Check::Verify]] path.subpath(0) == UnixPath("/Foo/Bar/Asd");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/Foo");
			[[Check::Verify]] path.subpath(0, 3) == UnixPath("/Foo/Bar");
			[[Check::Verify]] path.subpath(0, 4) == UnixPath("/Foo/Bar/Asd");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 5);
			[[Check::Verify]] path.subpath(1) == UnixPath("Foo/Bar/Asd");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("Foo");
			[[Check::Verify]] path.subpath(1, 2) == UnixPath("Foo/Bar");
			[[Check::Verify]] path.subpath(1, 3) == UnixPath("Foo/Bar/Asd");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 4);
			[[Check::Verify]] path.subpath(2) == UnixPath("Bar/Asd");
			[[Check::Verify]] path.subpath(2, 1) == UnixPath("Bar");
			[[Check::Verify]] path.subpath(2, 2) == UnixPath("Bar/Asd");
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 3);
			[[Check::Verify]] path.subpath(3) == UnixPath("Asd");
			[[Check::Verify]] path.subpath(3, 1) == UnixPath("Asd");
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 2);
			[[Check::Verify]] path.subpath(4) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(4, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Verify]] path[3] == "Asd";
			[[Check::Throw(std::out_of_range)]] path[4];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Asd";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("*");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "*";
			[[Check::Verify]] path.subpath(0) == UnixPath("*");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("*");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 2);
			[[Check::Verify]] path.subpath(1) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 1);
			[[Check::Verify]] path[0] == "*";
			[[Check::Throw(std::out_of_range)]] path[1];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "*";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/*");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/*";
			[[Check::Verify]] path.subpath(0) == UnixPath("/*");
			[[Check::Verify]] path.subpath(0, 1) == UnixPath("/");
			[[Check::Verify]] path.subpath(0, 2) == UnixPath("/*");
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == UnixPath("*");
			[[Check::Verify]] path.subpath(1, 1) == UnixPath("*");
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == UnixPath("");
			[[Check::Throw(std::out_of_range)]] path.subpath(3);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "*";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "*";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path(std::string("//Foo//Bar//Asd"));
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Verify]] path[3] == "Asd";
			[[Check::Throw(std::out_of_range)]] path[4];
			[[Check::Verify]] std::string(path) == "/Foo/Bar/Asd";
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Asd";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo/Bar");
			[[Check::Verify]] path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			UnixPath path("/Foo\\Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo\\Bar";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] std::string(path) == "/Foo\\Bar";
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Foo\\Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("/");
			[[Check::Verify]] !path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo\\Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar");
			[[Check::Verify]] !path.startsWith("/Foo/Bar/Asd");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] !path.startsWith("Foo/Bar/Asd");
		}{
			WindowsPath path("/Foo\\Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path[0] == "/Foo";
			[[Check::Verify]] path[1] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] std::string(path) == "/Foo\\Bar";
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] path.startsWith("/Foo");
			[[Check::Verify]] path.startsWith("/Foo\\Bar");
		}{
			WindowsPath path("Foo\\Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path[0] == "Foo";
			[[Check::Verify]] path[1] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] std::string(path) == "Foo\\Bar";
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] path.startsWith("Foo");
			[[Check::Verify]] !path.startsWith("Foo/Bar");
			[[Check::Verify]] path.startsWith("Foo\\Bar");
		}{
			WindowsPath path("C:\\Foo\\Bar");
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path[0] == "C:";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] std::string(path) == "C:\\Foo\\Bar";
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] path.startsWith("");
			[[Check::Verify]] path.startsWith("C:");
			[[Check::Verify]] !path.startsWith("/");
			[[Check::Verify]] !path.startsWith("Foo");
			[[Check::Verify]] path.startsWith("C:\\Foo");
			[[Check::Verify]] path.startsWith("C:\\Foo\\Bar");
		}
	}
};

class [[Testing::Case]] CopyConstruction {
	void operator()() {
		UnixPath path("/Foo//Bar");
		[[Check::Verify]] !path.empty();
		[[Check::Verify]] path.size() == 3u;
		[[Check::Verify]] std::string(path) == "/Foo/Bar";
		[[Check::Verify]] path[0] == "/";
		[[Check::Verify]] path[1] == "Foo";
		[[Check::Verify]] path[2] == "Bar";
		[[Check::Throw(std::out_of_range)]] path[3];

		UnixPath path2 = path;
		[[Check::Verify]] !path2.empty();
		[[Check::Verify]] path2.size() == 3u;
		[[Check::Verify]] std::string(path2) == "/Foo/Bar";
		[[Check::Verify]] path2[0] == "/";
		[[Check::Verify]] path2[1] == "Foo";
		[[Check::Verify]] path2[2] == "Bar";
		[[Check::Throw(std::out_of_range)]] path2[3];

		UnixPath path3(path);
		[[Check::Verify]] !path3.empty();
		[[Check::Verify]] path3.size() == 3u;
		[[Check::Verify]] std::string(path3) == "/Foo/Bar";
		[[Check::Verify]] path3[0] == "/";
		[[Check::Verify]] path3[1] == "Foo";
		[[Check::Verify]] path3[2] == "Bar";
		[[Check::Throw(std::out_of_range)]] path3[3];
	}
};

class [[Testing::Case]] Assignment {
	void operator()() {
		UnixPath path("/Foo//Bar");
		[[Check::Verify]] !path.empty();
		[[Check::Verify]] path.size() == 3u;
		[[Check::Verify]] std::string(path) == "/Foo/Bar";
		[[Check::Verify]] path[0] == "/";
		[[Check::Verify]] path[1] == "Foo";
		[[Check::Verify]] path[2] == "Bar";
		[[Check::Throw(std::out_of_range)]] path[3];

		UnixPath path2("Asd");
		[[Check::Verify]] !path2.empty();
		[[Check::Verify]] path2.size() == 1u;
		[[Check::Verify]] std::string(path2) == "Asd";
		[[Check::Verify]] path2[0] == "Asd";
		[[Check::Throw(std::out_of_range)]] path2[1];

		path2 = path;
		[[Check::Verify]] !path2.empty();
		[[Check::Verify]] path2.size() == 3u;
		[[Check::Verify]] std::string(path2) == "/Foo/Bar";
		[[Check::Verify]] path2[0] == "/";
		[[Check::Verify]] path2[1] == "Foo";
		[[Check::Verify]] path2[2] == "Bar";
		[[Check::Throw(std::out_of_range)]] path2[3];
	}
};

class [[Testing::Case]] Concat {
	void operator()() {
		{
			UnixPath p1;
			UnixPath p2;
			UnixPath path = p1 / p2;
			[[Check::Verify]] path.empty();
			[[Check::Verify]] path.size() == 0u;
			[[Check::Verify]] std::string(path) == "";
		}{
			UnixPath p1 = "/";
			UnixPath p2 = "";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "/";
		}{
			UnixPath p1 = "/";
			UnixPath p2 = "/";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "/";
		}{
			UnixPath p1 = "Foo";
			UnixPath p2 = "";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "Foo";
		}{
			UnixPath p1 = "";
			UnixPath p2 = "Foo";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 1u;
			[[Check::Verify]] std::string(path) == "Foo";
		}{
			UnixPath p1 = "Foo";
			UnixPath p2 = "Bar";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "Foo/Bar";
		}{
			UnixPath p1 = "/";
			UnixPath p2 = "Foo";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/Foo";
		}{
			UnixPath p1 = "/";
			UnixPath p2 = "/Foo";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "/Foo";
		}{
			UnixPath p1 = "/Foo";
			UnixPath p2 = "Bar";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
		}{
			UnixPath p1 = "Foo";
			UnixPath p2 = "/Bar";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "Foo/Bar";
		}{
			UnixPath p1 = "/Foo";
			UnixPath p2 = "/Bar";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
		}{
			UnixPath p1 = "Foo/Bar";
			UnixPath p2 = "Asd";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "Foo/Bar/Asd";
		}{
			UnixPath p1 = "/Foo/Bar";
			UnixPath p2 = "Asd";
			UnixPath path = p1 / p2;
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 4u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar/Asd";
		}
	}
};

class [[Testing::Case]] Append {
	void operator()() {
		UnixPath path = "/Foo/Bar";
		UnixPath p2 = "Asd";
		path /= p2;
		[[Check::Verify]] !path.empty();
		[[Check::Verify]] path.size() == 4u;
		[[Check::Verify]] std::string(path) == "/Foo/Bar/Asd";
	}
};

class [[Testing::Case]] OperatorLessThan {
	void operator()() {
		//we don't care about the real order, but it must be constant
		{
			UnixPath path = "/Foo/Bar";
			UnixPath prefix = "/Foo/Bar";
			bool result1, result2;
			result1 = path < prefix;
			result2 = path < prefix;
			[[Check::Verify]] result1 == result2;
		}{
			UnixPath path = "/Foo/Bar";
			UnixPath prefix = "/Foo/Baz";
			bool result1, result2;
			result1 = path < prefix;
			result2 = path < prefix;
			[[Check::Verify]] result1 == result2;
		}
	}
};

class [[Testing::Case]] Parent {
	void operator()() {
		{
			UnixPath path = "/Foo/Bar/Baz";
			UnixPath parent = path.parent();
			[[Check::Verify]] parent.size() == 3u;
			[[Check::Verify]] std::string(parent) == "/Foo/Bar";
		}{
			UnixPath path = "Foo/Bar/Baz";
			UnixPath parent = path.parent();
			[[Check::Verify]] parent.size() == 2u;
			[[Check::Verify]] std::string(parent) == "Foo/Bar";
		}{
			WindowsPath path = "C:\\Foo\\Bar\\Baz";
			WindowsPath parent = path.parent();
			[[Check::Verify]] parent.size() == 3u;
			[[Check::Verify]] std::string(parent) == "C:\\Foo\\Bar";
		}{
			WindowsPath path = "Foo\\Bar\\Baz";
			WindowsPath parent = path.parent();
			[[Check::Verify]] parent.size() == 2u;
			[[Check::Verify]] std::string(parent) == "Foo\\Bar";
		}
	}
};

class [[Testing::Case]] ConstIterate {
	void operator()() {
		{
			UnixPath path = "/Foo/Bar/Baz";
			UnixPath::ConstIterator i = path.cbegin();
			[[Check::Verify]] *i == "/";
			i++;
			[[Check::Verify]] *i == "Foo";
			++i;
			[[Check::Verify]] *i == "Bar";
			i++;
			[[Check::Verify]] *i == "Baz";
			++i;
			[[Check::Verify]] i == path.cend();
		}{
			UnixPath path = [[Check::NoThrow]] "Foo/Bar/Baz";
			UnixPath::ConstIterator i = path.cbegin();
			[[Check::Verify]] *i == "Foo";
			i++;
			[[Check::Verify]] *i == "Bar";
			++i;
			[[Check::Verify]] *i == "Baz";
			i++;
			[[Check::Verify]] i == path.cend();
		}{
			WindowsPath path = "C:\\Foo\\Bar\\Baz";
			WindowsPath::ConstIterator i = path.cbegin();
			[[Check::Verify]] *i == "C:";
			i++;
			[[Check::Verify]] *i == "Foo";
			++i;
			[[Check::Verify]] *i == "Bar";
			i++;
			[[Check::Verify]] *i == "Baz";
			++i;
			[[Check::Verify]] i == path.cend();
		}{
			WindowsPath path = "Foo\\Bar\\Baz";
			WindowsPath::ConstIterator i = path.cbegin();
			[[Check::Verify]] *i == "Foo";
			i++;
			[[Check::Verify]] *i == "Bar";
			++i;
			[[Check::Verify]] *i == "Baz";
			i++;
			[[Check::Verify]] i == path.cend();
		}
	}
};

class [[Testing::Case]] Convert {
	void operator()() {
		{
			UnixPath path = "/Foo/Bar/Baz";
			[[Check::Throw(std::invalid_argument)]] WindowsPath second = path;
		}{
			WindowsPath path = "C:\\Foo\\Bar\\Baz";
			[[Check::Throw(std::invalid_argument)]] UnixPath second = path;
		}{
			UnixPath path = "Foo/Bar/Baz";

			WindowsPath second = path;
			[[Check::Verify]] second.size() == 3u;
			[[Check::Verify]] std::string(second) == "Foo\\Bar\\Baz";

			UnixPath third = second;
			[[Check::Verify]] third.size() == 3u;
			[[Check::Verify]] std::string(third) == "Foo/Bar/Baz";
		}
	}
};

class [[Testing::Case]] Native {
	void operator()() {
		{
			UnixPath path = "Foo/Bar/Baz";

			NativePath second = path;
			[[Check::Verify]] second.size() == 3u;
	#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
			[[Check::Verify]] std::string(second) == "Foo/Bar/Baz";
	#elif BOOST_OS_WINDOWS
			[[Check::Verify]] std::string(second) == "Foo\\Bar\\Baz";
	#else
	#error "Platform not supported"
	#endif
		}
	}
};

class [[Testing::Case]] Runtime {
	void operator()() {
		{
			Path path("/Foo/Bar", PathFormat::Unix);
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "/Foo/Bar";
			[[Check::Verify]] path.subpath(0) == Path("/Foo/Bar", PathFormat::Unix);
			[[Check::Verify]] path.subpath(0, 1) == Path("/", PathFormat::Unix);
			[[Check::Verify]] path.subpath(0, 2) == Path("/Foo", PathFormat::Unix);
			[[Check::Verify]] path.subpath(0, 3) == Path("/Foo/Bar", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == Path("Foo/Bar", PathFormat::Unix);
			[[Check::Verify]] path.subpath(1, 1) == Path("Foo", PathFormat::Unix);
			[[Check::Verify]] path.subpath(1, 2) == Path("Foo/Bar", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == Path("Bar", PathFormat::Unix);
			[[Check::Verify]] path.subpath(2, 1) == Path("Bar", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == Path("", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "/";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] (path.specific<UnixPath>()) == UnixPath("/Foo/Bar");
			[[Check::Throw(std::invalid_argument)]] path.specific<WindowsPath>();
			[[Check::Verify]] path.startsWith(Path("", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("/", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("/Foo", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("/Foo/Bar", PathFormat::Unix));
			[[Check::Verify]] !path.startsWith(Path("Foo", PathFormat::Unix));
			[[Check::Verify]] !path.startsWith(Path("Foo/Bar", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("C:", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("C:\\Foo", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("C:\\Foo\\Bar", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo\\Bar", PathFormat::Windows));
		}{
			Path path("Foo/Bar", PathFormat::Unix);
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "Foo/Bar";
			[[Check::Verify]] path.subpath(0) == Path("Foo/Bar", PathFormat::Unix);
			[[Check::Verify]] path.subpath(0, 1) == Path("Foo", PathFormat::Unix);
			[[Check::Verify]] path.subpath(0, 2) == Path("Foo/Bar", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == Path("Bar", PathFormat::Unix);
			[[Check::Verify]] path.subpath(1, 1) == Path("Bar", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == Path("", PathFormat::Unix);
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "Foo";
			[[Check::Verify]] path[1] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] (path.specific<UnixPath>()) == UnixPath("Foo/Bar");
			[[Check::Verify]] (path.specific<WindowsPath>()) == WindowsPath("Foo\\Bar");
			[[Check::Verify]] path.startsWith(Path("", PathFormat::Unix));
			[[Check::Verify]] !path.startsWith(Path("/", PathFormat::Unix));
			[[Check::Verify]] !path.startsWith(Path("/Foo", PathFormat::Unix));
			[[Check::Verify]] !path.startsWith(Path("/Foo/Bar", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("Foo", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("Foo/Bar", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("C:", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("C:\\Foo", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("C:\\Foo\\Bar", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo", PathFormat::Windows));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo\\Bar", PathFormat::Windows));
		}{
			Path path("C:\\Foo\\Bar", PathFormat::Windows);
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 3u;
			[[Check::Verify]] std::string(path) == "C:\\Foo\\Bar";
			[[Check::Verify]] path.subpath(0) == Path("C:\\Foo\\Bar", PathFormat::Windows);
			[[Check::Verify]] path.subpath(0, 1) == Path("C:", PathFormat::Windows);
			[[Check::Verify]] path.subpath(0, 2) == Path("C:\\Foo", PathFormat::Windows);
			[[Check::Verify]] path.subpath(0, 3) == Path("C:\\Foo\\Bar", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 4);
			[[Check::Verify]] path.subpath(1) == Path("Foo\\Bar", PathFormat::Windows);
			[[Check::Verify]] path.subpath(1, 1) == Path("Foo", PathFormat::Windows);
			[[Check::Verify]] path.subpath(1, 2) == Path("Foo\\Bar", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 3);
			[[Check::Verify]] path.subpath(2) == Path("Bar", PathFormat::Windows);
			[[Check::Verify]] path.subpath(2, 1) == Path("Bar", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 2);
			[[Check::Verify]] path.subpath(3) == Path("", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(3, 1);
			[[Check::Verify]] path[0] == "C:";
			[[Check::Verify]] path[1] == "Foo";
			[[Check::Verify]] path[2] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[3];
			[[Check::Verify]] path.absolute();
			[[Check::Verify]] !path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Throw(std::invalid_argument)]] path.specific<UnixPath>();
			[[Check::Verify]] (path.specific<WindowsPath>()) == WindowsPath("C:\\Foo\\Bar");
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("/", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("/Foo", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("/Foo/Bar", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo/Bar", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("", PathFormat::Windows));
			[[Check::Verify]] path.startsWith(Path("C:", PathFormat::Windows));
			[[Check::Verify]] path.startsWith(Path("C:\\Foo", PathFormat::Windows));
			[[Check::Verify]] path.startsWith(Path("C:\\Foo\\Bar", PathFormat::Windows));
			[[Check::Verify]] !path.startsWith(Path("Foo", PathFormat::Windows));
			[[Check::Verify]] !path.startsWith(Path("Foo\\Bar", PathFormat::Windows));
		}{
			Path path("Foo\\Bar", PathFormat::Windows);
			[[Check::Verify]] !path.empty();
			[[Check::Verify]] path.size() == 2u;
			[[Check::Verify]] std::string(path) == "Foo\\Bar";
			[[Check::Verify]] path.subpath(0) == Path("Foo\\Bar", PathFormat::Windows);
			[[Check::Verify]] path.subpath(0, 1) == Path("Foo", PathFormat::Windows);
			[[Check::Verify]] path.subpath(0, 2) == Path("Foo\\Bar", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(0, 3);
			[[Check::Verify]] path.subpath(1) == Path("Bar", PathFormat::Windows);
			[[Check::Verify]] path.subpath(1, 1) == Path("Bar", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(1, 2);
			[[Check::Verify]] path.subpath(2) == Path("", PathFormat::Windows);
			[[Check::Throw(std::out_of_range)]] path.subpath(2, 1);
			[[Check::Verify]] path[0] == "Foo";
			[[Check::Verify]] path[1] == "Bar";
			[[Check::Throw(std::out_of_range)]] path[2];
			[[Check::Verify]] !path.absolute();
			[[Check::Verify]] path.relative();
			[[Check::Verify]] path.basename() == "Bar";
			[[Check::Verify]] (path.specific<UnixPath>()) == UnixPath("Foo/Bar");
			[[Check::Verify]] (path.specific<WindowsPath>()) == WindowsPath("Foo\\Bar");
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("/", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("/Foo", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("/Foo/Bar", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo", PathFormat::Unix));
			[[Check::Throw(std::invalid_argument)]] path.startsWith(Path("Foo/Bar", PathFormat::Unix));
			[[Check::Verify]] path.startsWith(Path("", PathFormat::Windows));
			[[Check::Verify]] !path.startsWith(Path("C:", PathFormat::Windows));
			[[Check::Verify]] !path.startsWith(Path("C:\\Foo", PathFormat::Windows));
			[[Check::Verify]] !path.startsWith(Path("C:\\Foo\\Bar", PathFormat::Windows));
			[[Check::Verify]] path.startsWith(Path("Foo", PathFormat::Windows));
			[[Check::Verify]] path.startsWith(Path("Foo\\Bar", PathFormat::Windows));
		}{
			UnixPath path("/Foo/Bar");
			[[Check::Verify]] Path(path) == Path("/Foo/Bar", PathFormat::Unix);
		}{
			UnixPath path("Foo/Bar");
			[[Check::Verify]] Path(path) == Path("Foo/Bar", PathFormat::Unix);
		}{
			WindowsPath path("C:\\Foo\\Bar");
			[[Check::Verify]] Path(path) == Path("C:\\Foo\\Bar", PathFormat::Windows);
		}{
			WindowsPath path("C:\\Foo\\Bar");
			[[Check::Verify]] Path(path) == Path("C:\\Foo\\Bar", PathFormat::Windows);
		}
	}
};

class [[Testing::Case]] CanonicalPath {
	void operator()() {
		{
			UnixPath path = "./foo/./../bar";
			[[Check::Verify]] !path.canonical();
			[[Check::Verify]] path.canonicalized() == UnixPath("bar");
			[[Check::Verify]] path.canonicalized().canonical();

			Path path2(path);
			[[Check::Verify]] !path2.canonical();
			[[Check::Verify]] path2.canonicalized() == Path("bar", PathFormat::Unix);
			[[Check::Verify]] path2.canonicalized().canonical();
		}{
			WindowsPath path = ".\\foo\\.\\..\\bar";
			[[Check::Verify]] !path.canonical();
			[[Check::Verify]] path.canonicalized() == UnixPath("bar");
			[[Check::Verify]] path.canonicalized().canonical();

			Path path2(path);
			[[Check::Verify]] !path2.canonical();
			[[Check::Verify]] path2.canonicalized() == Path("bar", PathFormat::Windows);
			[[Check::Verify]] path2.canonicalized().canonical();
		}{
			UnixPath path = "./.././foo/./../bar";
			[[Check::Verify]] !path.canonical();
			[[Check::Verify]] path.canonicalized() == UnixPath("../bar");
			[[Check::Verify]] path.canonicalized().canonical();

			Path path2(path);
			[[Check::Verify]] !path2.canonical();
			[[Check::Verify]] path2.canonicalized() == Path("../bar", PathFormat::Unix);
			[[Check::Verify]] path2.canonicalized().canonical();
		}{
			WindowsPath path = ".\\..\\.\\foo\\.\\..\\bar";
			[[Check::Verify]] !path.canonical();
			[[Check::Verify]] path.canonicalized() == UnixPath("..\\bar");
			[[Check::Verify]] path.canonicalized().canonical();

			Path path2(path);
			[[Check::Verify]] !path2.canonical();
			[[Check::Verify]] path2.canonicalized() == Path("..\\bar", PathFormat::Windows);
			[[Check::Verify]] path2.canonicalized().canonical();
		}
	}
};

class [[Testing::Case]] Wildcards {
	void operator()() {
		{
			UnixPath path = "China/Nanjing";
			[[Check::Verify]]  path.match("China/Nanjing");
			[[Check::Verify]] !path.match("China/Beijing");
		}{
			UnixPath path = "China/*jing";
			[[Check::Verify]]  path.match("China/Nanjing");
			[[Check::Verify]]  path.match("China/Beijing");
			[[Check::Verify]] !path.match("China/Baoding");
			[[Check::Verify]] !path.match("China/Jingzhou");
		}{
			UnixPath path = "China/Nanch?ng";
			[[Check::Verify]]  path.match("China/Nanchang");
			[[Check::Verify]]  path.match("China/Nanchong");
			[[Check::Verify]] !path.match("China/Xuchang");
			[[Check::Verify]] !path.match("China/Nanjing");
		}{
			UnixPath path = "China/D*y?ng";
			[[Check::Verify]] !path.match("China/Beijing");
			[[Check::Verify]] !path.match("China/Dalian");
			[[Check::Verify]] !path.match("China/Dandong");
			[[Check::Verify]]  path.match("China/Danyang");
			[[Check::Verify]] !path.match("China/Daqing");
			[[Check::Verify]] !path.match("China/Datong");
			[[Check::Verify]] !path.match("China/Dengzhou");
			[[Check::Verify]] !path.match("China/Dezhou");
			[[Check::Verify]] !path.match("China/Dingzhou");
			[[Check::Verify]] !path.match("China/Dongguan");
			[[Check::Verify]]  path.match("China/Dongying");
		}{
			UnixPath path = "*/*kland";
			[[Check::Verify]]  path.match("New Zealand/Auckland");
			[[Check::Verify]]  path.match("United States/Oakland");
			[[Check::Verify]] !path.match("United States/Portland");
			[[Check::Verify]] !path.match("United States/Orlando");
		}{
			UnixPath path = "*a*a/Sydney";
			[[Check::Verify]]  path.match("Australia/Sydney");
			[[Check::Verify]]  path.match("Canada/Sydney");
		}{
			UnixPath path = "*/Gr?nada";
			[[Check::Verify]]  path.match("Spain/Granada");
			[[Check::Verify]] !path.match("Grenada/St. George's");
		}{
			UnixPath path = "**/Gr?nada/**";
			[[Check::Verify]]  path.match("Spain/Granada");
			[[Check::Verify]]  path.match("Grenada/St. George's");
		}
	}
};

}
}
}
