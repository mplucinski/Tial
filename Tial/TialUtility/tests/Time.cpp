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

#define TIAL_MODULE "Tial::Utility::Time/Test"

#if BOOST_OS_WINDOWS
const auto timegm = _mkgmtime;
const auto timelocal = mktime;
#endif

[[Tial::Testing::Typedef]] namespace Testing = Tial::Testing;
[[Tial::Testing::Typedef]] namespace Check = Tial::Testing::Check;

namespace [[Testing::Suite]] Tial {
namespace [[Testing::Suite]] Utility {
namespace [[Testing::Suite]] Time {

struct DateTime {
	int year, month, day, hour, minute, second, microsecond;
};

class [[Testing::Case]] TimeCast {
	void testTimeCastToString(const DateTime &dt, const std::string &expected) {
		LOGI << "Test for " << expected;
		struct tm tt;
		tt.tm_sec = dt.second;
		tt.tm_min = dt.minute;
		tt.tm_hour = dt.hour;
		tt.tm_mday = dt.day;
		tt.tm_mon = dt.month - 1;
		tt.tm_year = dt.year - 1900;
		tt.tm_wday = -1;
		tt.tm_yday = -1;
		tt.tm_isdst = -1;

		// with UTC
		auto t = std::chrono::system_clock::from_time_t(timegm(&tt));
		t += std::chrono::microseconds(dt.microsecond);
		[[Check::Verify]] Tial::Utility::Time::timeCast(t, Tial::Utility::Time::Zone::UTC) == expected;

		// with local tz
		t = std::chrono::system_clock::from_time_t(timelocal(&tt));
		t += std::chrono::microseconds(dt.microsecond);
		[[Check::Verify]] Tial::Utility::Time::timeCast(t, Tial::Utility::Time::Zone::Local) == expected;
	}

	void operator()() {
		testTimeCastToString({1971,  1,  1,  0,  0,  0,      0}, "1971-01-01T00:00:00.000000");
		testTimeCastToString({1971,  1,  1,  0,  0,  0,     10}, "1971-01-01T00:00:00.000010");
		testTimeCastToString({1971,  1,  1,  0,  0,  0,    100}, "1971-01-01T00:00:00.000100");
		testTimeCastToString({1971,  1,  1,  0,  0,  0,   1000}, "1971-01-01T00:00:00.001000");
		testTimeCastToString({1971,  1,  1,  0,  0,  0,  10000}, "1971-01-01T00:00:00.010000");
		testTimeCastToString({1971,  1,  1,  0,  0,  0, 100000}, "1971-01-01T00:00:00.100000");
		testTimeCastToString({1971,  1,  1,  0,  0,  1,      0}, "1971-01-01T00:00:01.000000");
		testTimeCastToString({1971,  1,  1,  0,  0, 10,      0}, "1971-01-01T00:00:10.000000");
		testTimeCastToString({1971,  1,  1,  0,  1,  0,      0}, "1971-01-01T00:01:00.000000");
		testTimeCastToString({1971,  1,  1,  0, 10,  0,      0}, "1971-01-01T00:10:00.000000");
		testTimeCastToString({1971,  1,  1,  1,  0,  0,      0}, "1971-01-01T01:00:00.000000");
		testTimeCastToString({1971,  1,  1, 10,  0,  0,      0}, "1971-01-01T10:00:00.000000");
		testTimeCastToString({1971,  1,  2,  0,  0,  0,      0}, "1971-01-02T00:00:00.000000");
		testTimeCastToString({1971,  1, 10,  0,  0,  0,      0}, "1971-01-10T00:00:00.000000");
		testTimeCastToString({1971,  2,  1,  0,  0,  0,      0}, "1971-02-01T00:00:00.000000");
		testTimeCastToString({1971, 10,  1,  0,  0,  0,      0}, "1971-10-01T00:00:00.000000");
		testTimeCastToString({1972,  1,  1,  0,  0,  0,      0}, "1972-01-01T00:00:00.000000");
		testTimeCastToString({2015,  1,  1,  0,  0,  0,      0}, "2015-01-01T00:00:00.000000");
		testTimeCastToString({2015, 12, 31, 23, 59, 59, 999999}, "2015-12-31T23:59:59.999999");
	}
};

}
}
}
