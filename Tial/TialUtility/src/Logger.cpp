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
#include "Logger.hpp"
#include "ABI.hpp"
#include "Thread.hpp"
#include "Time.hpp"

#include <codecvt>
#include <iostream>
#include <fstream>
#include <locale>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>

using namespace std::literals;

static Tial::Utility::Logger::Level globalLevel = Tial::Utility::Logger::Level::Nice3;

std::ostream&
Tial::Utility::Logger::operator<<(std::ostream &os, Level level) {
	switch(level) {
	case Level::Critical: os << "Critical"; break;
	case Level::Warning: os << "Warning"; break;
	case Level::Info: os << "Info"; break;
	case Level::Debug: os << "Debug"; break;
	case Level::Nice1: os << "Nice1"; break;
	case Level::Nice2: os << "Nice2"; break;
	case Level::Nice3: os << "Nice3"; break;
	default: os << "<unknown>"; break;
	}
	return os;
}

Tial::Utility::Logger::Stream::operator bool() const {
	return true;
}

std::string Tial::Utility::Logger::Stream::output() const {
	return oss.str();
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const char *string) {
	s.oss << string;
	return s;
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, int value) {
	s.oss << value;
	return s;
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, unsigned int value) {
	s.oss << value;
	return s;
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, long value) {
	s.oss << value;
	return s;
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, unsigned long value) {
	s.oss << value;
	return s;
}


Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, long long value) {
	s.oss << value;
	return s;
}


Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, unsigned long long value) {
	s.oss << value;
	return s;
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::string &string) {
	return s << "\"" << string.c_str() << "\"";
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::u16string &string) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return s << convert.to_bytes(string);
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::u32string &string) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return s << convert.to_bytes(string);
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const void *ptr) {
	std::ostringstream hexPtr;
	hexPtr << std::hex << reinterpret_cast<uintptr_t>(ptr);
	return s << "@0x" << hexPtr.str().c_str();
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, bool value) {
	return s << (value ? "true" : "false");
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::type_info &info) {
	return s << ABI::demangle(info.name()).c_str();
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::type_index &index) {
	return s << ABI::demangle(index.name()).c_str();
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::ios_base::seekdir &seekdir) {
	switch(seekdir) {
	case std::ios_base::beg: s << "beg"; break;
	case std::ios_base::cur: s << "cur"; break;
	case std::ios_base::end: s << "end"; break;
	default: s << "<unknown>"; break;
	}
	return s;
}

Tial::Utility::Logger::Stream&
Tial::Utility::Logger::operator<<(Stream &s, const std::exception &exception) {
	return s << exception.what();
}

Tial::Utility::Logger::Stream &
Tial::Utility::Logger::operator<<(Stream &s, const std::experimental::string_view &string_view) {
	return s << string_view.to_string();
}

namespace Tial {
namespace Utility {
namespace Logger {
template<> TIALUTILITY_EXPORT std::experimental::string_view timePeriod<std::chrono::hours::period> = "h"_sv;
template<> TIALUTILITY_EXPORT std::experimental::string_view timePeriod<std::chrono::minutes::period> = "m"_sv;
template<> TIALUTILITY_EXPORT std::experimental::string_view timePeriod<std::chrono::seconds::period> = "s"_sv;
template<> TIALUTILITY_EXPORT std::experimental::string_view timePeriod<std::chrono::milliseconds::period> = "ms"_sv;
template<> TIALUTILITY_EXPORT std::experimental::string_view timePeriod<std::chrono::microseconds::period> = u8"\u1D6CD"_sv;
template<> TIALUTILITY_EXPORT std::experimental::string_view timePeriod<std::chrono::nanoseconds::period> = "ns"_sv;
}
}
}

Tial::Utility::Logger::Message::Message(
	const std::string &file, unsigned int line, const std::string &function,
	const std::string &prettyFunction, Level level, const std::string &module,
	const TimePoint &time
): file(file), line(line), function(function), prettyFunction(prettyFunction),
	level(level), module(module), time(time) {}

static std::string dots = "[...]";

static std::string limitString(const std::string &s, size_t maxSize = 32) {
	if(s.length() > maxSize) {
		size_t halfSize = (maxSize+dots.size())/2;
		return s.substr(0, halfSize)+dots+s.substr(s.size()-halfSize);
	}
	return s;
}

static std::string limitFileName(const std::string &name) {
#ifdef TIAL_SOURCE_BASE_DIRECTORY
	static const std::string base(TIAL_SOURCE_BASE_DIRECTORY);
	if(boost::algorithm::starts_with(name, base)) {
		std::string r = name.substr(base.length());
		if(!r.empty()) {
			if(r[0] == '/' || r[0] == '\\')
				r = r.substr(1);
			if(!r.empty())
				return r;
		}
	}
#endif
	return limitString(name);
}

std::vector<std::unique_ptr<Tial::Utility::Logger::Handler>> handlers;

Tial::Utility::Logger::Message::~Message() {
	for(auto &&handler: handlers)
		handler->log(*this);
}

Tial::Utility::Logger::Stream &Tial::Utility::Logger::Message::out() {
	return stream;
}

std::unordered_map<std::string, Tial::Utility::Logger::Level> levels;

bool Tial::Utility::Logger::toBeLoggedRuntime(Level level, const std::string &module) {
	auto moduleLevel = levels.find(module);
	if(moduleLevel == levels.end()) {
		auto sep = module.rfind("::");
		if(sep == module.npos)
			return level >= globalLevel;

		return toBeLoggedRuntime(level, module.substr(0, sep));
	}
	return level >= moduleLevel->second;
}

void Tial::Utility::Logger::setLoggingLevel(Level level) {
	globalLevel = level;
}

void Tial::Utility::Logger::setLoggingLevel(Level level, const std::string &module) {
	levels[module] = level;
}

Tial::Utility::Logger::Handler::Handler(Level level): level(level) {}

void Tial::Utility::Logger::Handler::log(const Message &message) {
	if(message.level >= level)
		write(message);
}

void Tial::Utility::Logger::addHandler(std::unique_ptr<Handler> &&handler) {
	handlers.push_back(std::move(handler));
}

Tial::Utility::Logger::Handlers::StandardOutput::StandardOutput(Level level, bool colors)
	: Handler(level), colors(colors) {}

static std::string printLevel(Tial::Utility::Logger::Level level, bool adjustSpaces = true) {
	switch(level) {
	case Tial::Utility::Logger::Level::Critical: return "CRITICAL"s + (adjustSpaces ?  "" : ""); break;
	case Tial::Utility::Logger::Level::Warning:  return "WARNING"s + (adjustSpaces ?  " " : ""); break;
	case Tial::Utility::Logger::Level::Info:     return "INFO"s + (adjustSpaces ?  "    " : ""); break;
	case Tial::Utility::Logger::Level::Debug:    return "DEBUG"s + (adjustSpaces ?  "   " : ""); break;
	case Tial::Utility::Logger::Level::Nice1:    return "NICE1"s + (adjustSpaces ?  "   " : ""); break;
	case Tial::Utility::Logger::Level::Nice2:    return "NICE2"s + (adjustSpaces ?  "   " : ""); break;
	case Tial::Utility::Logger::Level::Nice3:    return "NICE3"s + (adjustSpaces ?  "   " : ""); break;
	default:                                        return "UNKNOWN"s + (adjustSpaces ?  " " : ""); break;
	}
}

#define ColorReset         "\x1B[0m"
#define ColorBlack         "\x1B[30m"
#define ColorBrightBlack   "\x1B[30;1m"
#define ColorRed           "\x1B[31m"
#define ColorGreen         "\x1B[32m"
#define ColorYellow        "\x1B[33m"
#define ColorBlue          "\x1B[34m"
#define ColorMagenta       "\x1B[35m"
#define ColorCyan          "\x1B[36m"
#define ColorWhite         "\x1B[37m"
#define ColorBrightWhite   "\x1B[37;1m"

static std::string levelColorCode(Tial::Utility::Logger::Level level) {
	switch(level) {
	case Tial::Utility::Logger::Level::Critical: return ColorRed;
	case Tial::Utility::Logger::Level::Warning:  return ColorYellow;
	case Tial::Utility::Logger::Level::Info:     return ColorGreen;
	case Tial::Utility::Logger::Level::Debug:    return ColorCyan;
	case Tial::Utility::Logger::Level::Nice1:    return ColorBrightWhite;
	case Tial::Utility::Logger::Level::Nice2:    return ColorWhite;
	case Tial::Utility::Logger::Level::Nice3:    return ColorBrightBlack;
	default: return ColorReset; break;
	}
}

static std::string colorCodeReset() {
	return ColorReset;
}

void Tial::Utility::Logger::Handlers::StandardOutput::write(const Message &message) {
	std::ostringstream oss;
	oss << Utility::Time::timeCast(message.time) << " ";
	oss << printLevel(message.level) << " ";
	oss << Tial::Utility::Thread::name() << " ";
	std::string column0 = oss.str();
	while(column0.length() < 50)
		column0 += " ";

	oss = std::ostringstream();
	oss << limitString(message.module) << " ";
	std::string column1 = oss.str();
	while(column1.length() < 30)
		column1 += " ";

	oss = std::ostringstream();
	oss << limitFileName(message.file) << ":";
	oss << message.line << ":";
	oss << limitString(message.function) << " ";
	std::string column2 = oss.str();
	while(column2.length() < 50)
		column2 += " ";

	oss = std::ostringstream();
	oss << message.stream.output();
	std::string column3 = oss.str();

	oss = std::ostringstream();
	if(colors)
		oss << levelColorCode(message.level);
	oss << column0 << column1 << column2 << column3;
	if(colors)
		oss << colorCodeReset();
	oss << std::endl;

	std::string line = oss.str();

	{
		std::unique_lock<std::mutex> lock(output);

		std::cerr.write(line.data(), line.size());
		std::cerr.flush();
	}
}

static std::string processLogFileName(std::string pattern) {
	boost::algorithm::replace_all(pattern, "%t", Tial::Utility::Time::timeCast(std::chrono::system_clock::now()));
	return pattern;
}

Tial::Utility::Logger::Handlers::File::File(Level level, const std::string &fileName)
	: Handler(level), fileName(processLogFileName(fileName)) {}

const std::string fieldSeparator = "\t";

std::string escape(std::string str) {
	boost::algorithm::replace_all(str, "\\", "\\\\");
	boost::algorithm::replace_all(str, fieldSeparator, "\\t");
	boost::algorithm::replace_all(str, "\n", "\\n");
	return str;
}

void Tial::Utility::Logger::Handlers::File::write(const Message &message) {

	std::ostringstream oss;
	oss << std::chrono::duration_cast<std::chrono::microseconds>(message.time.time_since_epoch()).count() << fieldSeparator;
	oss << escape(printLevel(message.level, false)) << fieldSeparator;
	oss << escape(Tial::Utility::Thread::name()) << fieldSeparator;
	oss << escape(message.module) << fieldSeparator;
	oss << escape(message.file) << fieldSeparator;
	oss << message.line << fieldSeparator;
	oss << escape(message.function) << fieldSeparator;
	oss << escape(message.stream.output());

	{
		std::unique_lock<std::mutex> lock(output);
		std::ofstream of(fileName, std::ios::app);
		of << oss.str() << std::endl;
	}
}
