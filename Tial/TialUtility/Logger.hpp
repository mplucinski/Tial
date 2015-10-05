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
#include "tialutility_export.h"
#include "Language.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <experimental/string_view>

namespace Tial {
namespace Utility {
namespace Logger {

enum class Level: uint8_t {
	Critical = 100,
	Warning = 75,
	Info = 50,
	Debug = 25,
	Nice1 = 20,
	Nice2 = 15,
	Nice3 = 10
};

std::ostream &operator<<(std::ostream &os, Level level);

class TIALUTILITY_EXPORT Stream {
	std::ostringstream oss;
public:
	operator bool() const;
	std::string output() const;

	friend class Message;
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const char *string);
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, int value);
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, unsigned int value);
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, long value);
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, unsigned long value);
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, long long value);
	friend TIALUTILITY_EXPORT Stream &operator<<(Stream &s, unsigned long long value);
};

TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::string &string);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::u16string &string);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::u32string &string);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const void *ptr);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, bool value);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::type_info &info);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::type_index &index);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::ios_base::seekdir &seekdir);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::exception &exception);
TIALUTILITY_EXPORT Stream &operator<<(Stream &s, const std::experimental::string_view &string_view);

template<typename T>
Stream &operator<<(Stream &s, const std::shared_ptr<T> &ptr) {
	s << "std::shared_ptr{";
	if(ptr)
		s << *ptr;
	else
		s << "nullptr";
	s << "}";
	return s;
}

template<typename T1, typename T2>
Stream &operator<<(Stream &s, const std::pair<T1, T2> &pair) {
	return s << "std::pair{" << pair.first << ", " << pair.second << "}";
}

template<
	typename T,
	typename = decltype(std::begin(T())->first),
	typename = decltype(std::begin(T())->second)
>
Stream &operator<<(Stream &s, const T &t) {
	s << typeid(t) << "{";
	bool first = true;
	for(const auto &i: t) {
		if(first)
			first = false;
		else
			s << ", ";
		s << i.first << " = " << i.second;
	}
	s << "}";
	return s;
}

template<typename Period> extern std::experimental::string_view timePeriod;

template<typename Rep, typename Period>
Stream &operator<<(Stream &s, const std::chrono::duration<Rep, Period> &duration) {
	s << duration.count() << timePeriod<Period>.to_string().c_str();
	return s;
}

class TIALUTILITY_EXPORT Message {
public:
	typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

	Stream stream;
	std::string file;
	unsigned int line;
	std::string function;
	std::string prettyFunction;
	Level level;
	std::string module;
	TimePoint time;

	Message(const std::string &file, unsigned int line, const std::string &function,
		const std::string &prettyFunction, Level level, const std::string &module,
		const TimePoint &time);
	~Message();
	Stream &out();
};

constexpr bool toBeLoggedCompile(Level level) {
#if (defined TIAL_UTILITY_LOGGER_COMPILE_LEVEL) && (TIAL_UTILITY_LOGGER_COMPILE_LEVEL > 0)
	return static_cast<int>(level) >= TIAL_UTILITY_LOGGER_COMPILE_LEVEL;
#else
	(void)level;
	return true;
#endif
}

TIALUTILITY_EXPORT bool toBeLoggedRuntime(Level level, const std::string &module);
TIALUTILITY_EXPORT void setLoggingLevel(Level level);
TIALUTILITY_EXPORT void setLoggingLevel(Level level, const std::string &module);

class TIALUTILITY_EXPORT Handler {
	Level level;
public:
	explicit Handler(Level level);
	void log(const Message &message);
	virtual void write(const Message &message) = 0;
};

TIALUTILITY_EXPORT void addHandler(std::unique_ptr<Handler> &&handler);

namespace Handlers {

class TIALUTILITY_EXPORT StandardOutput: public Handler {
	std::mutex output;
	bool colors;
public:
	explicit StandardOutput(Level level, bool colors = true);
	virtual void write(const Message &message) override;
};

class TIALUTILITY_EXPORT File: public Handler {
	std::mutex output;
	std::string fileName;
public:
	explicit File(Level level, const std::string &fileName);
	virtual void write(const Message &message) override;
};

}

}
}
}


// helper macros

#define TIAL_UTILITY_LOGGER_LOG_INTERNAL(file, line, function, prettyFunction, level, module) \
	::Tial::Utility::Logger::toBeLoggedCompile(level) && \
	::Tial::Utility::Logger::toBeLoggedRuntime(level, module) && \
	::Tial::Utility::Logger::Message( \
		file, line, function, prettyFunction, level, module, ::std::chrono::system_clock::now() \
	).out()

#define TIAL_UTILITY_LOGGER_LOG(level) \
	TIAL_UTILITY_LOGGER_LOG_INTERNAL(__FILE__, __LINE__, __FUNCTION__, TIAL_UTILITY_LANGUAGE_FUNCTION_SIGNATURE, level, TIAL_MODULE)

#define TIAL_UTILITY_LOGGER_LOG_CRITICAL \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Critical)
#define TIAL_UTILITY_LOGGER_LOG_WARNING \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Warning)
#define TIAL_UTILITY_LOGGER_LOG_INFO \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Info)
#define TIAL_UTILITY_LOGGER_LOG_DEBUG \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Debug)
#define TIAL_UTILITY_LOGGER_LOG_NICE_1 \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Nice1)
#define TIAL_UTILITY_LOGGER_LOG_NICE_2 \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Nice2)
#define TIAL_UTILITY_LOGGER_LOG_NICE_3 \
		TIAL_UTILITY_LOGGER_LOG(::Tial::Utility::Logger::Level::Nice3)

// short macros
#ifndef TIAL_UTILITY_LOGGER_DISABLE_SHORT_MACROS

#define LOGC TIAL_UTILITY_LOGGER_LOG_CRITICAL
#define LOGW TIAL_UTILITY_LOGGER_LOG_WARNING
#define LOGI TIAL_UTILITY_LOGGER_LOG_INFO
#define LOGD TIAL_UTILITY_LOGGER_LOG_DEBUG
#define LOGN1 TIAL_UTILITY_LOGGER_LOG_NICE_1
#define LOGN2 TIAL_UTILITY_LOGGER_LOG_NICE_2
#define LOGN3 TIAL_UTILITY_LOGGER_LOG_NICE_3

#endif
