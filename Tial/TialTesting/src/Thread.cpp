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
#include "../Thread.hpp"
#include "../Check.hpp"

#include <TialUtility/TialUtility.hpp>

#define TIAL_MODULE "Tial::Testing::Thread"

Tial::Testing::Thread::Thread(const std::function<void()> &function): function(function) {}

Tial::Testing::Thread::Thread(const Thread &other): function(other.function) {}

Tial::Testing::Thread::~Thread() {
	if(thread.joinable()) {
		Utility::Thread::kill(thread.native_handle());
		thread.join();
	}
}

void Tial::Testing::Thread::operator()(const std::experimental::string_view &name) {
	std::promise<void> result;
	this->result = result.get_future();
	thread = std::thread([name=name, function=function, checker=Check::checker()](std::promise<void> result){
		Utility::Thread::setName(name.to_string());
		Check::setChecker(checker);
		try {
			function();
			result.set_value_at_thread_exit();
		} catch(std::exception e) {
			LOGC << "Exception in subthread: " << e.what();
			throw;
		} catch(...) {
			LOGC << "Exception in subthread";
			result.set_exception_at_thread_exit(std::current_exception());
		}
	}, std::move(result));
}

void Tial::Testing::Thread::join() {
	try {
		result.get();
	} catch(...) {
		LOGC << "Exception from subthread";
		throw;
	}
}
