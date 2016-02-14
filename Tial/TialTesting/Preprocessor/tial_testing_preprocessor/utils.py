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

class Escaper:
	def __init__(self, repl = []):
		self.repl = repl

	def __call__(self, s, escape_curly_braces):
		for i in self.repl + ( [('{', '{{'), ('}', '}}')] if escape_curly_braces else [] ):
			s = s.replace(*i)
		return s

escape_string = Escaper([
	('"', '\\"')
])
escape_comment = Escaper([
	('/*', '/ *'),
	('*/', '* /')
])
escape_expr = Escaper()

if __name__ == '__main__':
	import unittest

	class TestCase(unittest.TestCase):
		def test_escape_string(self):
			self.assertEqual(escape_string('', False), '')
			self.assertEqual(escape_string('a', False), 'a')
			self.assertEqual(escape_string('a"b', False), 'a\\"b')
			self.assertEqual(escape_string('a"{b', False), 'a\\"{b')
			self.assertEqual(escape_string('', True), '')
			self.assertEqual(escape_string('a', True), 'a')
			self.assertEqual(escape_string('a"b', True), 'a\\"b')
			self.assertEqual(escape_string('a"{b', True), 'a\\"{{b')

		def test_escape_comment(self):
			self.assertEqual(escape_comment('', False), '')
			self.assertEqual(escape_comment('a', False), 'a')
			self.assertEqual(escape_comment('a/*b*/c', False), 'a/ *b* /c')
			self.assertEqual(escape_comment('a/*b{c*/d', False), 'a/ *b{c* /d')
			self.assertEqual(escape_comment('', True), '')
			self.assertEqual(escape_comment('a', True), 'a')
			self.assertEqual(escape_comment('a/*b*/c', True), 'a/ *b* /c')
			self.assertEqual(escape_comment('a/*b{c*/d', True), 'a/ *b{{c* /d')

		def test_escape_expr(self):
			self.assertEqual(escape_expr('', False), '')
			self.assertEqual(escape_expr('a', False), 'a')
			self.assertEqual(escape_expr('a{b', False), 'a{b')
			self.assertEqual(escape_expr('', True), '')
			self.assertEqual(escape_expr('a', True), 'a')
			self.assertEqual(escape_expr('a{b', True), 'a{{b')


	unittest.main()