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
import subprocess
import sys
import tempfile

import tial_testing_preprocessor as preprocessor

def progress(verbose, message):
	if verbose:
		sys.stderr.write('{}\n'.format(message))

def execute(command, verbose, *args, **kwargs):
	progress(verbose, 'Running {}'.format(command))
	return subprocess.run(command, *args, **kwargs)

def _expand_options_list(options):
	if options[0] == '\\':
		options = options[1:]

	options = options.split(',')
	return options

def parse_compiler_options(options):
	return _expand_options_list(options)

def parse_compiler_definitions(options):
	return [ '-D{}'.format(i) for i in _expand_options_list(options) ]

def parse_compiler_include_directories(options):
	return [ '-I{}'.format(i) for i in _expand_options_list(options) ]

def main():
	parser = argparse.ArgumentParser(description='Preprocessor for test sources')
	parser.add_argument('input', nargs=1, help='Input file')
	parser.add_argument('-o', '--output', required=True, nargs=1, help='Output file')
	parser.add_argument('-v', '--verbose', action='store_true', help='Show some internal processing information')
	parser.add_argument('--disable-line', action='store_true', help='Do not produce #line directives in output file')
	parser.add_argument('--disable-cpp', action='store_true', help='Do not run CPP before processing input file')
	parser.add_argument('--compiler', nargs=1, help='Compiler binary that is used for building')
	parser.add_argument('--compiler-definitions', nargs=1, help='Compiler definitions')
	parser.add_argument('--compiler-options', nargs=1, help='Compiler options')
	parser.add_argument('--compiler-include-directories', nargs=1, help='Compiler include directories')
	parser.add_argument('--compiler-cxx-standard', nargs=1, help='Compiler C++ standard')

	args = parser.parse_args()
	verbose = args.verbose
	infile = pathlib.Path(args.input[0])

	if not args.disable_cpp:
		compiler = pathlib.Path(args.compiler[0])
		compiler_definitions = args.compiler_definitions[0]
		compiler_options = args.compiler_options[0]
		compiler_include_directories = args.compiler_include_directories[0]
		compiler_cxx_standard = args.compiler_cxx_standard[0]

		temp_dir = pathlib.Path(tempfile.mkdtemp(prefix='TialTesting'))
		oufile = temp_dir/(infile.name + '.E.cpp')

		compiler_options = parse_compiler_options(compiler_options)
		compiler_options += parse_compiler_definitions(compiler_definitions)
		compiler_options += parse_compiler_include_directories(compiler_include_directories)
		compiler_options += ['-std=c++{}'.format(compiler_cxx_standard)]

		execute([
				str(compiler),
				str(infile),
				'-E',
				'-o', str(oufile),
			] + compiler_options,
			verbose
		)

		sys.stderr.write('Created {}\n'.format(oufile))

		infile = oufile

	oufile = pathlib.Path(args.output[0])

	p = preprocessor.Preprocessor(infile, oufile, verbose, not args.disable_line)
	try:
		p.process()
	except:
		sys.stderr.write('Dumping progess to TialTesting.dump\n')
		with open('TialTesting.dump', 'w') as f:
			f.write(str(p.source))
		raise

if __name__ == '__main__':
	main()