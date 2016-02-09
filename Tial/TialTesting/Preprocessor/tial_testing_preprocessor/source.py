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

class Source:
	def __init__(self, text):
		self.__text = text
		self.lines = self._calculate_lines(text)

	def __str__(self):
		return self.__text

	def __len__(self):
		return len(self.__text)

	def __getitem__(self, key):
		return self.__text[key]

	def __setitem__(self, key, value):
		if not isinstance(key, slice):
			key = slice(key, key+1)

		self.__text = self.__text[:key.start] + value + self.__text[key.stop:]
		line = self.lines[key.start]
		self.lines[key] = [ line for i in range(len(value)) ]
		assert len(self.lines) == len(self.__text)

	def __iadd__(self, other):
		old_size = len(self.__text)
		self.__text += other

		new_size = len(self.__text)
		self.lines[old_size:new_size] = [ self.lines[-1] for i in range(new_size-old_size) ]
		assert len(self.lines) == len(self.__text)
		return self

	def encode(self, *args, **kwargs):
		return self.__text.encode(*args, **kwargs)

	@staticmethod
	def _calculate_lines(text):
		lines = []
		line = 0
		for i in text:
			lines.append(line)
			if i == '\n':
				line += 1
		assert len(lines) == len(text)
		return lines

if __name__ == '__main__':
	import unittest

	class TestCase(unittest.TestCase):
		def test_source_replacement(self):
			source = Source('''one
two
three
four
five
six''')
			self.assertEqual(source[0],  'o')
			self.assertEqual(source[1],  'n')
			self.assertEqual(source[-2], 'i')
			self.assertEqual(source[-1], 'x')
			self.assertEqual(source[9:15], 'hree\nf')
			self.assertEqual(source.lines, [
				0, 0, 0, 0, #one\n
				1, 1, 1, 1, #two\n
				2, 2, 2, 2, 2, 2, #three\n
				3, 3, 3, 3, 3, #four\n
				4, 4, 4, 4, 4, #five\n
				5, 5, 5 # six
			])

			source[1] = '\n\non'
			self.assertEqual(str(source), '''o

one
two
three
four
five
six''')
			self.assertEqual(source.lines, [
				0, 0, # o\n
				0, # \n
				0, 0, 0, 0, # one\n
				1, 1, 1, 1, # two\n
				2, 2, 2, 2, 2, 2, # three\n
				3, 3, 3, 3, 3, # four\n
				4, 4, 4, 4, 4, # five\n
				5, 5, 5 # six
			])

			source[12:18] = 'riangle\nsquare\n\nt'
			self.assertEqual(str(source), '''o

one
two
triangle
square

tour
five
six''')
			self.assertEqual(source.lines, [
				0, 0, # o\n
				0, # \n
				0, 0, 0, 0, # one\n
				1, 1, 1, 1, # two\n
				2, 2, 2, 2, 2, 2, 2, 2, 2, # triangle\n
				2, 2, 2, 2, 2, 2, 2, # square\n
				2, # \n
				2, 3, 3, 3, 3, # tour\n
				4, 4, 4, 4, 4, # five\n
				5, 5, 5 # six
			])

			source[30:39] = 'X'
			self.assertEqual(str(source), '''o

one
two
triangle
square

toXix''')
			self.assertEqual(source.lines, [
				0, 0, # o\n
				0, # \n
				0, 0, 0, 0, # one\n
				1, 1, 1, 1, # two\n
				2, 2, 2, 2, 2, 2, 2, 2, 2, # triangle\n
				2, 2, 2, 2, 2, 2, 2, # square\n
				2, # \n
				2, 3, 3, 5, 5 # toXix
			])

			source += '\nseven'
			self.assertEqual(str(source), '''o

one
two
triangle
square

toXix
seven''')
			self.assertEqual(source.lines, [
				0, 0, # o\n
				0, # \n
				0, 0, 0, 0, # one\n
				1, 1, 1, 1, # two\n
				2, 2, 2, 2, 2, 2, 2, 2, 2, # triangle\n
				2, 2, 2, 2, 2, 2, 2, # square\n
				2, # \n
				2, 3, 3, 5, 5, 5, # toXix\n
				5, 5, 5, 5, 5 # seven
			])
	unittest.main()
