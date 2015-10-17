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
#pragma once
#include "TialUtilityExport.hpp"
#include "Exception.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <utility>

#include <boost/algorithm/string/join.hpp>

#define TIAL_MODULE "Tial::Utility::ArgumentParser"

namespace Tial {
namespace Utility {
namespace ArgumentParser {

namespace Exceptions {

class TIALUTILITY_EXPORT SingleArgument: public Utility::Exception {
	std::string _argument;
public:
	SingleArgument(const std::string &message, const std::string &argument);
	bool operator==(const SingleArgument &other) const;
	bool operator!=(const SingleArgument &other) const;
	const std::string &argument() const;
};

class TIALUTILITY_EXPORT NoRequiredArgument: public SingleArgument {
public:
	explicit NoRequiredArgument(const std::string &argument);
};

class TIALUTILITY_EXPORT NoValueForArgument: public SingleArgument {
public:
	explicit NoValueForArgument(const std::string &argument);
};

class TIALUTILITY_EXPORT ArgumentNotSpecified: public SingleArgument {
public:
	explicit ArgumentNotSpecified(const std::string &argument);
};

class TIALUTILITY_EXPORT UnmatchedArguments: public Utility::Exception {
	std::vector<std::string> _leftArguments;
public:
	explicit UnmatchedArguments(const std::vector<std::string> &leftArguments);
	bool operator==(const UnmatchedArguments &other) const;
	bool operator!=(const UnmatchedArguments &other) const;
	const std::vector<std::string> &leftArguments() const;
};

class TIALUTILITY_EXPORT WrongCMainArgsFormat: public Utility::Exception {
public:
	WrongCMainArgsFormat();
};
}

namespace Action {

struct BaseAction {
	static constexpr bool skipLeftArguments = false;

	template<typename ArgumentDescriptor, typename R>
	static typename std::enable_if<ArgumentDescriptor::required>::type handleNotFound(R &) {
		THROW Exceptions::NoRequiredArgument(
			ArgumentDescriptor::option[0] ? ArgumentDescriptor::option : ArgumentDescriptor::longOption
		);
	}

	template<typename ArgumentDescriptor, typename R>
	static typename std::enable_if<!ArgumentDescriptor::required>::type handleNotFound(R &r) {
		r.template getRaw<ArgumentDescriptor>() = ArgumentDescriptor::defaultValue;
	}
};

template<typename T, T value>
struct StoreConst: public BaseAction {
	template<
		typename ArgumentDescriptor,
		typename R,
		typename ArgumentsCollection,
		typename ArgumentIterator,
		typename OutputStream
	> static ArgumentIterator handle(
		R &r, ArgumentsCollection &arguments, ArgumentIterator current, ArgumentIterator, OutputStream &
	) {
		r.template getRaw<ArgumentDescriptor>() = value;
		return arguments.erase(current);
	}
};

struct StoreValue: public BaseAction {
	template<
		typename ArgumentDescriptor,
		typename R,
		typename ArgumentsCollection,
		typename ArgumentIterator,
		typename OutputStream
	> static ArgumentIterator handle(
		R &r, ArgumentsCollection &arguments, ArgumentIterator current, ArgumentIterator end, OutputStream &
	) {
		if(current+1 != end) {
			r.template getRaw<ArgumentDescriptor>().read(*(current+1));
			return arguments.erase(current, current+2);
		} else
			THROW Exceptions::NoValueForArgument(ArgumentDescriptor::option);
	}
};

}

namespace Count {

struct BaseCount {};

template<int begin, int end>
struct Range: public BaseCount {};

template<int count>
struct Fixed: public Range<count, count+1> {
	template<typename Arg>
	static void visualizeForHelp(std::ostream &os) {
		std::string placeholder = Arg::argumentPlaceholder;
		if(!placeholder.empty())
			for(int i = 0; i != count; ++i)
				os << " " << placeholder;
	}
};

typedef Fixed<0> None;

}

struct TIALUTILITY_EXPORT Parameter {
	static constexpr bool required = false;
	static constexpr char option[] = {0};
	static constexpr char longOption[] = {0};
	static constexpr char argumentPlaceholder[] = {0};
};

namespace Special {

struct TIALUTILITY_EXPORT Switch: public Parameter {
	typedef bool ValueType;
	typedef Count::None ValueCount;
	typedef ArgumentParser::Action::StoreConst<bool, true> Action;
	static constexpr bool defaultValue = false;
};

struct TIALUTILITY_EXPORT Help: public Switch {

	struct Action: public ArgumentParser::Action::StoreConst<bool, true> {
		static constexpr bool skipLeftArguments = true;
		template<
			typename Current,
			typename R,
			typename ArgumentsCollection,
			typename ArgumentIterator,
			typename OutputStream
		> static ArgumentIterator handle(
			R &r, ArgumentsCollection &args, ArgumentIterator current, ArgumentIterator end, OutputStream &errorStream
		) {
			r.printHelp(errorStream);
			return ArgumentParser::Action::StoreConst<bool, true>::handle<Current>(r, args, current, end, errorStream);
		}
	};

	static constexpr char command[] = {0};
	static constexpr bool defaultValue = false;
	static constexpr char option[] = "-h";
	static constexpr char longOption[] = "--help";
	static constexpr char description[] = "print the help and exit";
};

}

class ensureSeparation;

template<typename WrappedOutputStream>
class ostream;

template<typename WrappedOutputStream>
ostream<WrappedOutputStream> &operator<<(ostream<WrappedOutputStream> &os, ensureSeparation &&es);

template<typename WrappedOutputStream, typename T>
ostream<WrappedOutputStream> &operator<<(ostream<WrappedOutputStream> &os, T &&value);

template<typename WrappedOutputStream>
ostream<WrappedOutputStream> &operator<<(
	ostream<WrappedOutputStream> &os,
	WrappedOutputStream& (*function)(WrappedOutputStream&)
);

template<typename WrappedOutputStream>
class ostream {
	bool emptyLine = true;
	bool ensureSeparation = true;
	WrappedOutputStream &wrapped;

public:
	explicit ostream(WrappedOutputStream &wrapped): wrapped(wrapped) {}

	template<typename W>
	friend ostream<W> &operator<<(ostream<W> &os, class ensureSeparation &&es);

	template<typename W, typename T>
	friend ostream<W> &operator<<(ostream<W> &os, T &&value);

	template<typename W>
	friend ostream<W> &operator<<(ostream<W> &os, W& (*function)(W&));
};

class ensureSeparation {};

template<typename WrappedOutputStream>
ostream<WrappedOutputStream> &operator<<(ostream<WrappedOutputStream> &os, class ensureSeparation &&) {
	os.ensureSeparation = true;
	return os;
}

template<typename WrappedOutputStream, typename T>
ostream<WrappedOutputStream> &operator<<(ostream<WrappedOutputStream> &os, T &&value) {
	if(os.ensureSeparation && !os.emptyLine)
		os.wrapped << " ";
	os.wrapped << std::forward<T&>(value);
	os.emptyLine = false;
	os.ensureSeparation = false;
	return os;
}

template<typename WrappedOutputStream>
ostream<WrappedOutputStream> &operator<<(
	ostream<WrappedOutputStream> &os,
	WrappedOutputStream& (*function)(WrappedOutputStream&)
) {
	if(function == static_cast<WrappedOutputStream&(*)(WrappedOutputStream&)>(&std::endl)) {
		os.wrapped << std::endl;
		os.emptyLine = true;
	} else {
		os.wrapped << function;
		os.emptyLine = false;
	}
	return os;
}

template<typename Find>
constexpr int findPosition(int pos) {
	return pos;
}

template<typename Find, typename Head, typename... Tail>
constexpr int findPosition(int pos = 0) {
	return std::is_same<Find, Head>::value ? pos : findPosition<Find, Tail...>(pos+1);
}

template<typename... Args>
struct GetHelpOption;

template<>
struct GetHelpOption<> {
	typedef void type;
};

template<typename Current, typename... Left>
struct GetHelpOption<Current, Left...> {
	typedef typename std::conditional<
		std::is_base_of<Special::Help, Current>::value,
		Current,
		typename GetHelpOption<Left...>::type
	>::type type;
};

template<typename T>
constexpr bool exists(T* = nullptr) {
	return true;
}

constexpr bool exists() {
	return false;
}

template<bool full, typename OutputStream, typename Arg>
void usage(OutputStream &os) {
	std::ostringstream line;
	if(full)
		line << "    ";
	if(!full && !Arg::required)
		line << "[";
	if(Arg::option[0])
		line << Arg::option;
	if(Arg::option[0] && Arg::longOption[0])
		line << "|";
	if(Arg::longOption[0])
		line << Arg::longOption;
	Arg::ValueCount::template visualizeForHelp<Arg>(line);
	if(!full && !Arg::required)
		line << "]";
	if(full) {
		line << " ";
		while(line.tellp() < 40)
			line << " ";
		line << Arg::description;
	}
	os << line.str();
	if(full)
		os << std::endl;
}

template<bool full, bool required, bool named, typename OutputStream, typename Arg>
void usageMatching(OutputStream &os) {
	if(required == Arg::required)
		usage<full, OutputStream, Arg>(os);
}

template<bool full, bool required, bool named, typename OutputStream, typename Current>
void collectUsage(OutputStream &os) {
	usageMatching<full, required, named, OutputStream, Current>(os);
}

template<
	bool full,
	bool required,
	bool named,
	typename OutputStream,
	typename Current,
	typename... Left,
	typename = typename std::enable_if<sizeof...(Left) != 0>::type
> void collectUsage(OutputStream &os) {
	usageMatching<full, required, named, OutputStream, Current>(os);
	os << ensureSeparation();
	collectUsage<full, required, named, OutputStream, Left...>(os);
	os << ensureSeparation();
}

template<bool required, bool named, typename ArgumentDescriptor>
bool hasAny() {
	return required == ArgumentDescriptor::required;
}

template<
	bool required,
	bool named,
	typename ArgumentDescriptor,
	typename... Left,
	typename = typename std::enable_if<sizeof...(Left) != 0>::type
> bool hasAny() {
	return hasAny<required, named, ArgumentDescriptor>() || hasAny<required, named, Left...>();
}

template<typename ValueType>
inline ValueType readValue(const std::string &string) {
	std::istringstream iss(string);
	ValueType value;
	iss >> value;
	return value;
}

template<>
inline std::string readValue(const std::string &string) {
	return string;
}

template<typename ArgumentDescriptor>
class ValueWrapper {
	typedef typename ArgumentDescriptor::ValueType ValueType;
	ValueType value;
	bool specified = false;

public:
	ValueWrapper &operator=(const ValueType &other) {
		value = other;
		specified = true;
		return *this;
	}


	void read(const std::string &string) {
		value = readValue<ValueType>(string);
		specified = true;
	}

	const ValueType &operator()() {
		if(!specified)
			THROW Exceptions::ArgumentNotSpecified(
				ArgumentDescriptor::option[0] ? ArgumentDescriptor::option : ArgumentDescriptor::longOption
			);
		return value;
	};
};

template<typename HelpOption>
std::string getCommand(const std::string &passedName) {
	return HelpOption::command[0] ? HelpOption::command : passedName;
}

template<typename... Args>
class Result {
	std::string passedName;
	std::tuple<ValueWrapper<Args>...> values;

	template<typename ArgumentDescriptor>
	ValueWrapper<ArgumentDescriptor> &getRaw() {
		return std::get<findPosition<ArgumentDescriptor, Args...>()>(values);
	}

public:
	explicit Result(const std::string &passedName = std::string()): passedName(passedName) {}

	template<typename ArgumentDescriptor>
	const typename ArgumentDescriptor::ValueType &get() {
		return std::get<findPosition<ArgumentDescriptor, Args...>()>(values)();
	}

	template<typename OutputStream>
	void printHelp(OutputStream &os) {
		typedef typename GetHelpOption<Args...>::type HelpOption;
		os << HelpOption::commandName << std::endl;
		os << "    Usage: " << getCommand<HelpOption>(passedName) << ensureSeparation();
		collectUsage<false, true, true, OutputStream, Args...>(os);
		os << ensureSeparation();
		collectUsage<false, false, true, OutputStream, Args...>(os);
		os << std::endl;
		os << std::endl;
		if(hasAny<true, true, Args...>()) {
			os << "Required named arguments:" << std::endl;
			collectUsage<true, true, true, OutputStream, Args...>(os);
			os << std::endl;
		}
		if(hasAny<false, true, Args...>()) {
			os << "Optional named arguments:" << std::endl;
			collectUsage<true, false, true, OutputStream, Args...>(os);
			os << std::endl;
		}
	}

	template<typename R, typename ArgumentsCollection, typename OutputStream>
	friend void parseArguments(R &r, ArgumentsCollection &arguments, OutputStream &errorStream);

	friend struct Action::BaseAction;

	template<typename T, T value>
	friend struct Action::StoreConst;

	friend struct Action::StoreValue;
};

template<typename R, typename ArgumentsCollection, typename OutputStream>
void parseArguments(R &, ArgumentsCollection &arguments, OutputStream &) {
	if(!arguments.empty()) {
		THROW Exceptions::UnmatchedArguments(arguments);
	}
}

template<typename ArgumentDescriptor>
bool optionMatches(const std::string &argument) {
	if(argument == std::string(ArgumentDescriptor::option))
		return true;
	if(ArgumentDescriptor::longOption[0] && argument == std::string(ArgumentDescriptor::longOption))
		return true;
	return false;
}

template<typename R, typename ArgumentsCollection, typename OutputStream, typename Current, typename... Rest>
void parseArguments(R &r, ArgumentsCollection &arguments, OutputStream &errorStream) {
	bool found = false;
	for(auto i = arguments.begin(); i != arguments.end();) {
		if(optionMatches<Current>(*i)) {
			i = Current::Action::template handle<Current>(r, arguments, i, arguments.end(), errorStream);
			found = true;
		} else
			++i;
	}
	if(!found)
		Current::Action::template handleNotFound<Current>(r);

	if(found && Current::Action::skipLeftArguments)
		return;

	parseArguments<R, ArgumentsCollection, OutputStream, Rest...>(r, arguments, errorStream);
}

template<typename... Args>
class Parser {
	template<typename ArgumentsCollection, typename OutputStream>
	Result<Args...> parseCommon(ArgumentsCollection &&arguments, OutputStream &errorStream) {
		std::vector<std::string> v(std::forward<ArgumentsCollection&>(arguments));
		ostream<decltype(errorStream)> o(errorStream);
		Result<Args...> result;
		parseArguments<Result<Args...>, decltype(v), decltype(o), Args...>(result, v, o);
		return result;
	}
public:
	Result<Args...> parse(const std::initializer_list<std::string> &arguments, std::ostream &errorStream = std::cerr) {
		return parseCommon(arguments, errorStream);
	}

	Result<Args...> parse(int argc, char **argv, std::ostream &errorStream = std::cerr) {
		std::vector<std::string> arguments;
		if(argc < 1)
			THROW Exceptions::WrongCMainArgsFormat();

		for(int i = 1; i != argc; ++i)
			arguments.push_back(argv[i]);

		return parseCommon(std::forward<decltype(arguments)&>(arguments), errorStream);
	}
};

}
}
}

#undef TIAL_MODULE
