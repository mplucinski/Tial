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
#include "tialtesting_export.h"
#include "../Main.hpp"
#include "../Check.hpp"

#include <TialUtility/TialUtility.hpp>

#include <cassert>
#include <iostream>

#define TIAL_MODULE "Tial::Testing::Main"

Tial::Testing::Main *Tial::Testing::Main::_instance = nullptr;

Tial::Testing::Main &Tial::Testing::Main::instance() {
	if(!_instance)
		_instance = new Main();
	return *_instance;
}

void Tial::Testing::Main::registerTestSuiteFunction(
	TestSuiteFunction function, const std::experimental::string_view &name
) {
	testSuiteFunctions.push_back(std::make_pair(function, name));
}

namespace {
namespace CommandLine {
	struct Help: public Tial::Utility::ArgumentParser::Special::Help {
		static constexpr char commandName[] = "Tial testing binary";
	};
	constexpr char Help::commandName[];

	struct StopOnError: public Tial::Utility::ArgumentParser::Special::Switch {
		static constexpr char longOption[] = "--stop-on-error";
		static constexpr char description[] = "Stop, when encounter first error";
	};
	constexpr char StopOnError::longOption[];
	constexpr char StopOnError::description[];

	struct PrintAll: public Tial::Utility::ArgumentParser::Special::Switch {
		static constexpr char longOption[] = "--print-all";
		static constexpr char description[] = "Print all checks";
	};
	constexpr char PrintAll::longOption[];
	constexpr char PrintAll::description[];

	struct BacktraceOnException: public Tial::Utility::ArgumentParser::Special::Switch {
		static constexpr char longOption[] = "--backtrace-on-exception";
		static constexpr char description[] = "Print full backtrace on exception throw";
	};
	constexpr char BacktraceOnException::longOption[];
	constexpr char BacktraceOnException::description[];

	struct LoggerLevel: public Tial::Utility::ArgumentParser::Parameter {
		typedef unsigned int ValueType;
		typedef Tial::Utility::ArgumentParser::Count::Fixed<1> ValueCount;
		typedef Tial::Utility::ArgumentParser::Action::StoreValue Action;
		static constexpr unsigned int defaultValue = static_cast<unsigned int>(Tial::Utility::Logger::Level::Debug);
		static constexpr char longOption[] = "--logger-level";
		static constexpr char description[] = "Set logger level";
	};
	constexpr unsigned int LoggerLevel::defaultValue;
	constexpr char LoggerLevel::longOption[];
	constexpr char LoggerLevel::description[];

	struct TestCase: public Tial::Utility::ArgumentParser::Parameter {
		typedef std::string ValueType;
		typedef Tial::Utility::ArgumentParser::Count::Fixed<1> ValueCount;
		typedef Tial::Utility::ArgumentParser::Action::StoreValue Action;
		static const std::string defaultValue;
		static constexpr char longOption[] = "--test-case";
		static constexpr char option[] = "-t";
		static constexpr char description[] = "Run single test case";
	};
	const std::string TestCase::defaultValue;
	constexpr const char TestCase::longOption[];
	constexpr const char TestCase::option[];
	constexpr const char TestCase::description[];

	struct ListCases: public Tial::Utility::ArgumentParser::Special::Switch {
		static constexpr char longOption[] = "--list-cases";
		static constexpr char option[] = "-l";
		static constexpr char description[] = "List all test cases";
	};
	constexpr const char ListCases::longOption[];
	constexpr const char ListCases::option[];
	constexpr const char ListCases::description[];

	typedef Tial::Utility::ArgumentParser::Parser<
		Help, StopOnError, PrintAll, BacktraceOnException, LoggerLevel, TestCase, ListCases
	> Parser;
}
}

int Tial::Testing::Main::main(int argc, char **argv) {
	CommandLine::Parser parser;
	auto result = parser.parse(argc, argv);
	if(result.get<CommandLine::Help>())
		return 0;

	bool stopOnError = result.get<CommandLine::StopOnError>();
	bool printAll = result.get<CommandLine::PrintAll>();
	unsigned int loggerLevel = result.get<CommandLine::LoggerLevel>();
	std::string testCase = result.get<CommandLine::TestCase>();
	bool list = result.get<CommandLine::ListCases>();

	Utility::Logger::setLoggingLevel(static_cast<Utility::Logger::Level>(loggerLevel));

	if(result.get<CommandLine::BacktraceOnException>()) {
		Utility::installExceptionThrowHook([](const Utility::Exception &exception){
			std::string backtrace;
			Utility::ABI::currentStackTrace([&backtrace](const std::string &line){
				backtrace += line + "\n";
			});
			LOGW << "Now throwing exception " << Utility::ABI::demangle(typeid(exception).name());
			LOGW << backtrace;
		});
	}

	if(list)
		std::cerr << "Listing tests..." << std::endl;
	else
		std::cerr << "Executing tests..." << std::endl;
	Utility::Thread::setName("main");

	unsigned int performed = 0;
	unsigned int succeeded = 0;
	unsigned int failed = 0;
	unsigned int skipped = 0;
	bool skipRest = false;
	for(auto testSuiteFunction: testSuiteFunctions) {
		Check::startTestSuite(testSuiteFunction.second, printAll);
		testSuiteFunction.first([&](
			const SingleCaseFunction &singleCaseFunction,
			const std::experimental::string_view &name
		) {
			std::string caseName = testSuiteFunction.second.to_string() + "::" + name.to_string();
			if(list) {
				std::cout << caseName << std::endl;
			} else if(skipRest) {
				LOGW << "Skipping test case: " << caseName;
				skipped++;
			} else if(testCase.empty() || caseName == testCase) {
				LOGI << "Executing test case: " << caseName;
				performed++;
				if(Check::catchTestExceptions(caseName, [&singleCaseFunction](){ singleCaseFunction(); }))
					succeeded++;
				else {
					failed++;
					if(stopOnError) {
						std::cerr << "Test failed, skipping others..." << std::endl;
						skipRest = true;
					}
				}
			}
		});
	}

	std::cerr << "Executing tests done." << std::endl;
	std::cerr << "\tTotal number: " << performed+skipped << std::endl;
	std::cerr << "\tPerformed:    " << performed << std::endl;
	std::cerr << "\tSucceeded:    " << succeeded << std::endl;
	std::cerr << "\tSKIPPED:      " << skipped << std::endl;
	std::cerr << "\tFAILED:       " << failed << std::endl;

	delete this;
	return failed;
}

TIALTESTING_EXPORT int main(int argc, char **argv) {
	Tial::Utility::Logger::addHandler(std::make_unique<Tial::Utility::Logger::Handlers::StandardOutput>(
		Tial::Utility::Logger::Level::Nice3
	));

	try {
		return Tial::Testing::Main::instance().main(argc, argv);
	} catch(const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	} catch(...) {
		std::cerr << "Error: unknown" << std::endl;
	}
	return 1;
}
