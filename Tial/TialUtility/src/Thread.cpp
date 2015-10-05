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
#include "Thread.hpp"

#include <thread>
#include <unordered_map>

#include <boost/lexical_cast.hpp>
#include <boost/predef.h>

#define TIAL_MODULE "Tial::Utility::Thread"

static thread_local std::string currentThreadName = boost::lexical_cast<std::string>(std::this_thread::get_id());

Tial::Utility::Thread::Exceptions::InvalidHandle::InvalidHandle(): Exception("invalid handle") {}

void Tial::Utility::Thread::setName(const std::string &name) {
	currentThreadName = name;
}

std::string Tial::Utility::Thread::name() {
	return currentThreadName;
}

void Tial::Utility::Thread::kill(const std::thread::native_handle_type &handle) {
#if (BOOST_OS_UNIX || BOOST_OS_MACOS)
	if(handle == std::thread::native_handle_type())
		THROW Exceptions::InvalidHandle();
	pthread_cancel(handle);
#else
#error "Platform not supported"
#endif
}
