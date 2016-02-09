#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2015, Mariusz Plucinski
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

import argparse
import pathlib
import sys

import tial_testing_preprocessor as preprocessor

def main():
	parser = argparse.ArgumentParser(description='Preprocessor for test sources')
	parser.add_argument('input', nargs=1, help='Input file')
	parser.add_argument('-o', '--output', required=True, nargs=1, help='Output file')
	parser.add_argument('-v', '--verbose', action='store_true', help='Show some internal processing information')
	parser.add_argument('--disable-line', action='store_true', help='Do not produce #line directives in output file')

	args = parser.parse_args()
	infile = pathlib.Path(args.input[0])
	oufile = pathlib.Path(args.output[0])

	p = preprocessor.Preprocessor(infile, oufile, args.verbose, not args.disable_line)
	try:
		p.process()
	except:
		sys.stderr.write('Dumping progess to TialTesting.dump\n')
		with open('TialTesting.dump', 'w') as f:
			f.write(str(p.source))
		raise

if __name__ == '__main__':
	main()