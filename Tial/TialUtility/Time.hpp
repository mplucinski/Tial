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

#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <boost/predef.h>

#define TIAL_MODULE "Tial::Utility::Time"

using namespace std::literals;

namespace Tial {
namespace Utility {
namespace Time {

enum class Zone {
	Local,
	UTC
};

template<typename TimePoint>
std::string timeCast(const TimePoint &time, Zone zone = Zone::Local) {
	std::time_t tt = TimePoint::clock::to_time_t(time);
	std::tm tm;

	switch(zone) {
	case Zone::Local:
#if BOOST_OS_WINDOWS
		localtime_s(&tm, &tt);
#else
		localtime_r(&tt, &tm);
#endif
		break;
	case Zone::UTC:
#if BOOST_OS_WINDOWS
		gmtime_s(&tm, &tt);
#else
		gmtime_r(&tt, &tm);
#endif
		break;
	default:
		THROW Exception("Invalid time zone argument");
	}

	std::ostringstream oss;
	oss << std::setw(4) << std::setfill('0') << tm.tm_year+1900 << "-"
		<< std::setw(2) << std::setfill('0') << tm.tm_mon+1     << "-"
		<< std::setw(2) << std::setfill('0') << tm.tm_mday      << "T"
		<< std::setw(2) << std::setfill('0') << tm.tm_hour      << ":"
		<< std::setw(2) << std::setfill('0') << tm.tm_min       << ":"
		<< std::setw(2) << std::setfill('0') << tm.tm_sec       << "."
		<< std::setw(6) << std::setfill('0')
			<< std::abs(std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count()) % 1000000;
	return oss.str();
}

}
}
}

#undef TIAL_MODULE
