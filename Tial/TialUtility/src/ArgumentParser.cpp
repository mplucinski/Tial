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
#include "../ArgumentParser.hpp"

Tial::Utility::ArgumentParser::Exceptions::SingleArgument::SingleArgument(
	const std::string &message, const std::string &argument
): Utility::Exception(message+": "+argument), _argument(argument) {}

bool Tial::Utility::ArgumentParser::Exceptions::SingleArgument::SingleArgument::operator==(
	const SingleArgument &other
) const {
	return _argument == other._argument;
}

bool Tial::Utility::ArgumentParser::Exceptions::SingleArgument::SingleArgument::operator!=(
	const SingleArgument &other
) const {
	return !((*this) == other);
}

const std::string &Tial::Utility::ArgumentParser::Exceptions::SingleArgument::argument() const {
	return _argument;
}

Tial::Utility::ArgumentParser::Exceptions::NoRequiredArgument::NoRequiredArgument(const std::string &argument):
	SingleArgument("Required argument not found", argument) {}

Tial::Utility::ArgumentParser::Exceptions::NoValueForArgument::NoValueForArgument(const std::string &argument):
	SingleArgument("Required value for argument", argument) {}

Tial::Utility::ArgumentParser::Exceptions::ArgumentNotSpecified::ArgumentNotSpecified(const std::string &argument):
	SingleArgument("Argument not provided", argument) {}

Tial::Utility::ArgumentParser::Exceptions::UnmatchedArguments::UnmatchedArguments(
	const std::vector<std::string> &leftArguments
): Utility::Exception("Arguments not captured: "+boost::algorithm::join(leftArguments, " ")),
	_leftArguments(leftArguments) {}

bool Tial::Utility::ArgumentParser::Exceptions::UnmatchedArguments::operator==(
	const UnmatchedArguments &other
) const {
	return _leftArguments == other._leftArguments;
}

bool Tial::Utility::ArgumentParser::Exceptions::UnmatchedArguments::operator!=(
	const UnmatchedArguments &other
) const {
	return !((*this) == other);
}

const std::vector<std::string> &Tial::Utility::ArgumentParser::Exceptions::UnmatchedArguments::leftArguments() const {
	return _leftArguments;
}

Tial::Utility::ArgumentParser::Exceptions::WrongCMainArgsFormat::WrongCMainArgsFormat():
	Utility::Exception("Arguments passed are not compatible with C-main function arguments format") {}

constexpr char Tial::Utility::ArgumentParser::Parameter::option[];
constexpr char Tial::Utility::ArgumentParser::Parameter::longOption[];
constexpr char Tial::Utility::ArgumentParser::Parameter::argumentPlaceholder[];

constexpr bool Tial::Utility::ArgumentParser::Special::Switch::defaultValue;

constexpr char Tial::Utility::ArgumentParser::Special::Help::command[];
constexpr bool Tial::Utility::ArgumentParser::Special::Help::defaultValue;
constexpr char Tial::Utility::ArgumentParser::Special::Help::option[];
constexpr char Tial::Utility::ArgumentParser::Special::Help::longOption[];
constexpr char Tial::Utility::ArgumentParser::Special::Help::description[];
