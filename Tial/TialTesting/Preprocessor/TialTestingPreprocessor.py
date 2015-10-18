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
import re
import sys

def escape_string(s):
	return s.replace('"', '\\"')

def escape_comment(s):
	return s.replace('/*', '/ *').replace('*/', '* /')

class Suite:
	def __init__(self, name, full_name):
		self.name = name
		self.full_name = full_name
		self.subsuites = []
		self.cases = []

	def close_block(self, replacement):
		return replacement

class Case:
	def __init__(self, name, full_name, base):
		self.name = name
		self.full_name = full_name
		self.has_data = None
		self.base = base

	def close_block(self, replacement):
		if self.has_data is not None:
			run_method = r'''
public:
	void _runTestCase() {
		'''+self.has_data+'''();
}'''
		else:
			run_method = r'''
public:
	void _runTestCase() {
		_caller([this](){
			(*this)();
		}, _caseName());
}
'''
		if not self.base:
			replacement = run_method + replacement + ''';
constexpr ::std::experimental::string_view '''+self.name+'''::_name
'''

		return replacement

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
	def __init__(self, regeces):
		self.regeces = regeces
		for i in self.regeces:
			i.regex = re.compile(i.regex, re.DOTALL)

	def search(self, source, pos):
		match = None
		regex = None
		for r in self.regeces:
			m = r.regex.search(source, pos)
			if m and (not match or match.start() > m.start()):
				match = m
				regex = r
		return (match, regex)

class Preprocessor:
	attr_typedef = 'Tial::Testing::Typedef'
	attr_suite = 'Tial::Testing::Suite'
	attr_case = 'Tial::Testing::Case'
	attr_casebase = 'Tial::Testing::CaseBase'
	attr_expect_fail = 'Tial::Testing::Check::ExpectFail'
	attr_fail = 'Tial::Testing::Check::Fail'
	attr_verify = 'Tial::Testing::Check::Verify'
	attr_throw = 'Tial::Testing::Check::Throw'
	attr_throw_exact = 'Tial::Testing::Check::ThrowExact'
	attr_throw_equal = 'Tial::Testing::Check::ThrowEqual'
	attr_no_throw = 'Tial::Testing::Check::NoThrow'
	attr_data = 'Tial::Testing::Data'

	res = RegexCollection([
		Regex('attr_ns_alias',
			r'\[\[\s*'+attr_typedef+'\s*\]\]\s*namespace\s+(?P<alias>\S+)\s*=\s*(?P<original>\S+)\s*;',
			r'/* '+attr_typedef+' */ namespace \g<alias> = \g<original>;'
		),
		Regex('attr_ns',
			r'namespace\s*\[\[\s*(?P<attr>[^\]]+?)\s*\]\]\s*(?P<ns_name>\S+)\s*{',
			r'namespace /* [[\g<attr>]] */ \g<ns_name> {'
		),
		Regex('attr_cl',
			r'class\s*\[\[\s*(?P<attr>[^\]]+?Case)\s*\]\]\s*(?P<cl_name>[^\s:]+)(?P<suffix>[^{]*){',
			r'''class /* [[\g<attr>]] */ \g<cl_name> \g<suffix> {
public:
static constexpr ::std::experimental::string_view _name = "\g<cl_name>"_sv;
::Tial::Testing::Main::SingleCaseCaller _caller = nullptr;
static constexpr ::std::experimental::string_view _caseName() {
	return _name;
}
'''
		),
		Regex('attr_clb',
			r'class\s*\[\[\s*(?P<attr>[^\]+]+?CaseBase)\s*\]\]\s*(?P<cl_name>[^\s:]+)(?P<suffix>[^{]*){',
			r'''class /* [[\g<attr>]] */ \g<cl_name> \g<suffix> {
public:
::Tial::Testing::Main::SingleCaseCaller _caller = nullptr;
static constexpr ::std::experimental::string_view _caseName() {
	return Case::_name;
}
'''
		),
		Regex('brace_open',
			r'{',
			r'{'
		),
		Regex('brace_close',
			r'}',
			r'}'
		),
		Regex('expect_fail',
			r'\[\[(?P<attr>[^\]]+ExpectFail)\]\]\s*;',
			r'/* [[\g<attr>]] */ ::Tial::Testing::Check::expectFail(TIAL_TESTING_POINTINFO);'
		),
		Regex('fail',
			r'\[\[(?P<attr>[^\]]+Fail)\]\]\s*;',
			r'/* [[\g<attr>]] */ ::Tial::Testing::Check::fail(TIAL_TESTING_POINTINFO);'
		),
		Regex('verify',
			r'\[\[(?P<attr>[^\]]+Verify)\]\](?P<expr>[^;]+);',
			lambda m: r'/* [[{attr}]] {expr} */ {code}'.format(
				attr=escape_comment(m.group('attr')),
				expr=escape_comment(m.group('expr')),
				code=Preprocessor.handle_verify(m.group('expr'), '')
			)
		),
		Regex('throw',
			r'\[\[(?P<attr>[^\]]+Throw)\((?P<type>[^\)]+)\)\]\](?P<expr>[^;]+);',
			lambda m: r'''/* [[{attr}({type_c})]] {expr_c} */
::Tial::Testing::Check::throws<{type}>(TIAL_TESTING_POINTINFO, "{expr_s}", [&](){{ {expr}; }});'''.format(
				attr=escape_comment(m.group('attr')),
				type_c=escape_comment(m.group('type')),
				type=m.group('type'),
				expr_c=escape_comment(m.group('expr')),
				expr_s=escape_string(m.group('expr')),
				expr=m.group('expr')
			)
		),
		Regex('throw_exact',
			r'\[\[(?P<attr>[^\]]+ThrowExact)\((?P<type>[^\)]+)\)\]\](?P<expr>[^;]+);',
			r'''/* [[\g<attr>(\g<type>)]] \g<expr> */
::Tial::Testing::Check::throws<\g<type>>(TIAL_TESTING_POINTINFO, "\g<expr>", [&](){ \g<expr>; }, true);'''
		),
		Regex('throw_equal',
			r'\[\[(?P<attr>[^\]]+ThrowEqual)\((?P<inst>.+?)\)\]\](?P<expr>[^;]+);',
			lambda m: r'''/* [[{attr}({inst})]] {expr} */
::Tial::Testing::Check::throwsEqual(TIAL_TESTING_POINTINFO, "{inst_s}", {inst}, "{expr_s}", [&](){{ {expr}; }});'''.format(
				attr=m.group('attr'),
				inst=m.group('inst'),
				inst_s=escape_string(m.group('inst')),
				expr=m.group('expr'),
				expr_s=escape_string(m.group('expr'))
			)
		),
		Regex('no_throw',
			r'\[\[(?P<attr>[^\]]+NoThrow)\]\](?P<expr>[^;]+);',
			lambda m: r'''/* [[{attr}]] {expr_c} */
::Tial::Testing::Check::noThrows<typename std::decay<decltype({expr})>::type>(TIAL_TESTING_POINTINFO, "{expr_s}", [&]()->typename std::decay<decltype({expr})>::type{{ return {expr}; }});'''.format(
				attr=m.group('attr'),
				expr=m.group('expr'),
				expr_c=escape_comment(m.group('expr')),
				expr_s=escape_string(m.group('expr'))
			)
		),
		Regex('data_with_name',
			r'\[\[(?P<attr>[^\]]+Data)\("(?P<name>[^"]*)"\)\]\]\s+(?P<expr>[^;]+);',
			r'/* [[\g<attr>("\g<name>")]] */ _runWithData("\g<name>"_sv, \g<expr>);'
		),
		Regex('data',
			r'\[\[(?P<attr>[^\]]+Data)\]\]\s+(?P<ret>\S+)\s+(?P<name>\S+)\(\)',
			r'''

template<typename DATA>
void _runWithData(const std::experimental::string_view &name, const DATA &data) {
	_caller([this, &data](){
		(*this)(data);
	}, std::string(this->_name) + "/" + name.to_string());
}

/* [[\g<attr>]] */ \g<ret> \g<name>()'''
		)
	])

	def __init__(self, infile, oufile):
		self.infile = infile
		self.oufile = oufile

		if not self.oufile.parent.exists():
			self.oufile.parent.mkdir(parents=True)

		self.source = open(str(self.infile), 'rb').read().decode('utf-8')

		self.aliases = {}
		self.suites = []
		self.blocks_stack = []

	def search_all(self, regex):
		begin = 0
		matches = []
		while True:
			match = regex.search(self.source, begin)
			if match is None:
				break
			matches.append(match)
			begin = match.end()+1
		return matches

	def get_line(self, pos):
		line = 1
		for i in range(pos):
			if self.source[i:i+5] == '#line':
				line = int(self.source[i+6:self.source.find('\n', i+6)])-1
			if self.source[i] == '\n':
				line += 1
		return line

	def re_replace(self, match, regex, replacement):
		a = match.start()
		b = match.end()
		line_b = self.get_line(b)
		replacement = regex.regex.sub(replacement, self.source[a:b])
		if self.pos == b:
			self.pos = a + len(replacement)

		replacement += '''
#line {}
'''.format(line_b)

		#self.source[a:b] = replacement
		left = self.source[:a]
		right = self.source[b:]
		self.source = left + replacement + right

	def check_attr(self, attr, spec):
		attr = attr.split('::')
		for i in range(len(attr)):
			if attr[i] in self.aliases:
				attr[i] = self.aliases[attr[i]]
		attr = '::'.join(attr)
		return attr == spec

	def get_next(self):
		(match, regex) = self.res.search(self.source, self.pos)
		if match is not None:
			self.pos = match.end()
		return (match, regex)

	def full_name(self, element=None):
		ns = [ i.name for i in self.blocks_stack if i is not None ]
		if element is not None:
			ns.append(element)
		return '::'+'::'.join(ns)

	@staticmethod
	def handle_verify(expression, replacement):
		expression = expression.strip()
		paren = 0
		right = False
		e = None
		for i in range(len(expression)):
			if expression[i] == '(':
				paren += 1
			elif expression[i] == ')':
				paren -= 1
			elif paren == 0:
				if e is None:
					if (i+1 < len(expression) and
							(expression[i] == '=' or expression[i] == '!' or
							expression[i] == '<' or expression[i] == '>') and expression[i+1] == '='):
						e = (i, 2)
					elif expression[i] == '<' or expression[i] == '>':
						e = (i, 1)

		if paren != 0:
			raise Exception('Non-matching parenthesis')

		op = [
			('==', '::Tial::Testing::Check::ExpressionOperation::Equal'),
			('!=', '::Tial::Testing::Check::ExpressionOperation::NotEqual'),
			('<=', '::Tial::Testing::Check::ExpressionOperation::LessOrEqual'),
			('<' , '::Tial::Testing::Check::ExpressionOperation::Less'),
			('>=', '::Tial::Testing::Check::ExpressionOperation::GreaterOrEqual'),
			('>' , '::Tial::Testing::Check::ExpressionOperation::Greater')
		]
		handler = '::Tial::Testing::Check::expression'
		if e:
			l = expression[:e[0]]
			r = expression[e[0]+e[1]:]
			e = expression[e[0]:e[0]+e[1]]
			handler += '<'+[ i[1] for i in op if i[0] == e ][0]+'>'
		handler += '(TIAL_TESTING_POINTINFO, '
		handler += '"'+escape_string(expression)+'"'
		if not e:
			handler += ', ('+expression+')'
		else:
			handler += ', "' + escape_string(e) + '"'
			handler += ', "' + escape_string(l) + '", ' + l
			handler += ', "' + escape_string(r) + '", ' + r
		handler += ')'
		replacement = replacement + handler
		return replacement + ';'

	def process(self):
		self.pos = 0
		while True:
			(match, regex) = self.get_next()
			if match is None or regex is None:
				break

			try:
				replacement = regex.replacement
				if regex.name == 'attr_ns_alias':
					self.aliases[match.group('alias')] = match.group('original')
				elif regex.name == 'attr_ns':
					if self.check_attr(match.group('attr'), self.attr_suite):
						suite = Suite(match.group('ns_name'), self.full_name(match.group('ns_name')))
						self.suites.append(suite)
						self.blocks_stack.append(suite)
					else:
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'attr_cl' or regex.name == 'attr_clb':
					if (self.check_attr(match.group('attr'), self.attr_case) or
							self.check_attr(match.group('attr'), self.attr_casebase)
						):
						case = Case(match.group('cl_name'), self.full_name(match.group('cl_name')), regex.name == 'attr_clb')
						if not case.base:
							self.blocks_stack[-1].cases.append(case)
						self.blocks_stack.append(case)
					else:
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'expect_fail':
					if not self.check_attr(match.group('attr'), self.attr_expect_fail):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'fail':
					if not self.check_attr(match.group('attr'), self.attr_fail):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'verify':
					if not self.check_attr(match.group('attr'), self.attr_verify):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'throw':
					if not self.check_attr(match.group('attr'), self.attr_throw):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'throw_exact':
					if not self.check_attr(match.group('attr'), self.attr_throw_exact):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'throw_equal':
					if not self.check_attr(match.group('attr'), self.attr_throw_equal):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'no_throw':
					if not self.check_attr(match.group('attr'), self.attr_no_throw):
						raise Exception('Unknown attribute: '+match.group('attr'))
				elif regex.name == 'data' or regex.name == 'data_with_name':
					if not self.check_attr(match.group('attr'), self.attr_data):
						raise Exception('Unknown attribute: '+match.group('attr'))
					if self.blocks_stack[-1] is not None:
						self.blocks_stack[-1].has_data = match.group('name')
				elif regex.name == 'brace_open':
					self.blocks_stack.append(None)
				elif regex.name == 'brace_close':
					if self.blocks_stack[-1] is not None:
						replacement = self.blocks_stack[-1].close_block(regex.replacement)
					del self.blocks_stack[-1]
				else:
					raise Exception('Unknown regex: '+regex.name)
				self.re_replace(match, regex, replacement)
			except Exception as e:
				print(e)
				print('In line: {}'.format(self.get_line(match.start())))
				print('Expression: {}'.format(match.group(0)))
				raise e

		self.source += '\n'
		self.source += 'namespace {\n';
		for suite in self.suites:
			self.source += '::Tial::Testing::Suite<\n'
			if len(suite.cases) > 0:
				self.source += '\t' + ',\n\t'.join([ case.full_name for case in suite.cases ]) + '\n'
			self.source += '> __testSuite_{name_raw}("{name}");\n'.format(
				name=suite.full_name, name_raw=suite.full_name.replace('::', '__')
			)
		self.source += '}\n';

		open(str(self.oufile), 'wb').write(self.source.encode('utf-8'))

def main():
	parser = argparse.ArgumentParser(description='Preprocessor for test sources')
	parser.add_argument('input', nargs=1, help='Input file')
	parser.add_argument('-o', '--output', required=True, nargs=1, help='Output file')

	args = parser.parse_args()
	infile = pathlib.Path(args.input[0])
	oufile = pathlib.Path(args.output[0])

	p = Preprocessor(infile, oufile)
	p.process()
if __name__ == '__main__':
	main()