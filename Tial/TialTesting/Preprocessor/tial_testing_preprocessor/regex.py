#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2016, Mariusz Plucinski
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions
#    and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of
#    conditions and the following disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import re

class Regex:
	def __init__(self, name, regex, replacement):
		self.name = name
		self.regex = regex
		self.replacement = replacement

	def __repr__(self):
		return 'Regex(name = {name}, regex = {regex}, replacement = {replacement})'.format(
			name=self.name, regex=self.regex, replacement=self.replacement
		)

class RegexCollection:
	enable_full_match_optimization = True
	re_group = re.compile(r'\?P<(.*?)>')

	def __init__(self, regeces):
		self.regeces = regeces
		if self.enable_full_match_optimization:
			self.full_regex = r'('
			for i in range(len(self.regeces)):
				regex = self.regeces[i]
				r = self.re_group.sub('', regex.regex)
				self.full_regex += r'(?P<'+regex.name+r'>'+r+r')'
				if i + 1 < len(self.regeces):
					self.full_regex += r'|'
			self.full_regex += r')'
			self.full_regex = re.compile(self.full_regex, re.DOTALL)

		for i in self.regeces:
			i.regex = re.compile(i.regex, re.DOTALL)

		if self.enable_full_match_optimization:
			self.regeces = { i.name: i for i in self.regeces }

	def search(self, source, pos):
		match = None
		regex = None
		m = None
		if self.enable_full_match_optimization:
			full_match = self.full_regex.search(source, pos)
			if full_match:
				m = [ i[0] for i in full_match.groupdict().items() if i[1] is not None ]
				assert len(m) == 1
				m = m[0]
				r = self.regeces[m]
				m = r.regex.search(source, full_match.start())
				if m and (not match or match.start() > m.start()):
					match = m
					regex = r
		else:
			for r in self.regeces:
				m = r.regex.search(source, pos)
				if m and (not match or match.start() > m.start()):
					match = m
					regex = r

		return (match, regex)
