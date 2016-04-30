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

from . import regex
from . import utils

import copy
import io
import re
import string
import sys

class Suite:
	def __init__(self, name, full_name):
		self.name = name
		self.full_name = full_name
		self.subsuites = []
		self.cases = []

	def __repr__(self):
		return '<Suite "{}">'.format(self.full_name)

	def close_block(self, replacement):
		return replacement

class Quoting:
	def __init__(self, name, pos, line):
		self.name = name
		self.pos = pos
		self.line = line

	def __repr__(self):
		return '<{} from line {}>'.format(self.name, self.line)

class SingleQuoting(Quoting):
	def __init__(self, pos, line):
		super(SingleQuoting, self).__init__('SingleQuoting', pos, line)

class DoubleQuoting(Quoting):
	def __init__(self, pos, line):
		super(DoubleQuoting, self).__init__('DoubleQuoting', pos, line)

class Block:
	def __init__(self, start, end, line, text):
		self.start = start
		self.end = end
		self.line = line
		self.text = text

	def __repr__(self):
		return '<Block "{}" ({})>'.format(self.text, self.line)

class CppBlock:
	class SubBlock:
		def __init__(self, directive, content):
			self.directive = directive
			self.content = content

		def generate(self):
			return '#{}{}'.format(self.directive, self.content)

	class If(SubBlock):
		def __init__(self, content):
			super(CppBlock.If, self).__init__('if', content)

	class Elif(SubBlock):
		def __init__(self, content):
			super(CppBlock.Elif, self).__init__('elif', content)

	def __init__(self):
		self.sub_blocks = []

	def append(self, sub_block):
		self.sub_blocks.append(sub_block)

	def generate(self):
		return '\n'.join([i.generate() for i in self.sub_blocks])

	def generate_end(self):
		return '#endif'

	def __repr__(self):
		return '<CppBlock #{}.{}>'.format(self.directive, self.content)

class CppBlockStack(list):
	def generate(self):
		return '\n'.join([i.generate() for i in self]) + '\n'

	def generate_end(self):
		return '\n'.join([i.generate_end() for i in self]) + '\n'

class Case:
	def __init__(self, name, full_name, base, cpp_blocks_stack):
		self.name = name
		self.full_name = full_name
		self.has_data = None
		self.base = base
		self.cpp_blocks_stack = copy.deepcopy(cpp_blocks_stack)

	def __repr__(self):
		return '<Case "{}">'.format(self.full_name)

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
		}, caseName());
}
'''
		if not self.base:
			replacement = run_method + replacement + ''';
constexpr ::std::experimental::string_view '''+self.name+'''::_name
'''

		return replacement

	def generate_registration(self):
		return (
			self.cpp_blocks_stack.generate()
			+ self.full_name
			+ '\n'
			+ self.cpp_blocks_stack.generate_end()
		)

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
	attr_data_set = 'Tial::Testing::DataSet'
	attr_data_function = 'Tial::Testing::DataFunction'
	attr_data_base = 'Tial::Testing::DataBase'

	res = regex.RegexCollection([
		regex.Regex('hashline',
			r'# (?P<line>[0-9]+)\s+"(?P<file>[^"]*)"(?P<flags>[^\n]*)\n',
			None
		),
		regex.Regex('double_quote',
			r'"',
			r'"',
		),
		regex.Regex('single_quote',
			r'\'',
			r'\''
		),
		regex.Regex('inline_comment',
			r'//(?P<comment>[^\n]*)\n',
			r'/* \g<comment> */'
		),
		regex.Regex('block_comment',
			r'/\*(?P<comment>.*?)\*/',
			lambda match: r'/* ' + ' */\n/* '.join(match.group('comment').split('\n')) + ' */'
		),
		regex.Regex('attr_ns_alias',
			r'\[\[\s*'+attr_typedef+'\s*\]\]\s*namespace\s+(?P<alias>\S+)\s*=\s*(?P<original>\S+)\s*;',
			r'/* '+attr_typedef+' */ namespace \g<alias> = \g<original>;'
		),
		regex.Regex('attr_ns',
			r'namespace\s*\[\[\s*(?P<attr>[^\]]+?)\s*\]\]\s*(?P<ns_name>\S+)\s*{',
			r'namespace /* [[\g<attr>]] */ \g<ns_name> {'
		),
		regex.Regex('attr_cl',
			r'class\s*\[\[\s*(?P<attr>[^\]]+?Case)\s*\]\]\s*(?P<cl_name>[^\s:]+)(?P<suffix>[^{]*){',
			r'''class /* [[\g<attr>]] */ \g<cl_name> \g<suffix> {
public:
static constexpr ::std::experimental::string_view _name = "\g<cl_name>"_sv;
::Tial::Testing::Main::SingleCaseCaller _caller = nullptr;
static constexpr ::std::experimental::string_view caseName() {
	return _name;
}
'''
		),
		regex.Regex('attr_clb',
			r'class\s*\[\[\s*(?P<attr>[^\]+]+?CaseBase)\s*\]\]\s*(?P<cl_name>[^\s:]+)(?P<suffix>[^{]*){',
			r'''class /* [[\g<attr>]] */ \g<cl_name> \g<suffix> {
public:
::Tial::Testing::Main::SingleCaseCaller _caller = nullptr;
static constexpr ::std::experimental::string_view caseName() {
	return Case::_name;
}
'''
		),
		regex.Regex('cpp_if',
			r'#(?P<directive>(el|end)?if(def)?)(?P<content>[^\n]*)\n',
			r'#\g<directive>\g<content>'
		),
		regex.Regex('cpp',
			r'#(?P<directive>[^\n]*)\n',
			r'#\g<directive>'
		),
		regex.Regex('brace_open',
			r'{',
			None
		),
		regex.Regex('brace_close',
			r'}',
			r'}'
		),
		regex.Regex('expect_fail',
			r'\[\[(?P<attr>[^\]]+ExpectFail)\]\]\s*;',
			r'/* [[\g<attr>]] */ ::Tial::Testing::Check::expectFail({point_info});',
			True
		),
		regex.Regex('fail',
			r'\[\[(?P<attr>[^\]]+Fail)\]\]\s*;',
			r'/* [[\g<attr>]] */ ::Tial::Testing::Check::fail({point_info});',
			True
		),
		regex.Regex('verify',
			r'\[\[(?P<attr>[^\]]+Verify)\]\](?P<expr>[^;]+);',
			lambda match: r'/* [[{attr}]] {expr} */ {code}'.format(
				attr=utils.escape_comment(match.group('attr'), True),
				expr=utils.escape_comment(match.group('expr'), True),
				code=Preprocessor.handle_verify(match.group('expr'), '')
			),
			True
		),
		regex.Regex('throw',
			r'\[\[(?P<attr>[^\]]+Throw)\((?P<type>[^\)]+)\)\]\](?P<expr>[^;]+);',
			lambda match: r'''/* [[{attr}({type_c})]] {expr_c} */
::Tial::Testing::Check::throws<{type}>({{point_info}}, "{expr_s}", [&](){{{{ {expr}; }}}});'''.format(
				attr=utils.escape_comment(match.group('attr'), True),
				type_c=utils.escape_comment(match.group('type'), True),
				type=match.group('type'),
				expr_c=utils.escape_comment(match.group('expr'), True),
				expr_s=utils.escape_string(match.group('expr'), True),
				expr=utils.escape_expr(match.group('expr'), True)
			),
			True
		),
		regex.Regex('throw_exact',
			r'\[\[(?P<attr>[^\]]+ThrowExact)\((?P<type>[^\)]+)\)\]\](?P<expr>[^;]+);',
			r'''/* [[\g<attr>(\g<type>)]] \g<expr> */
::Tial::Testing::Check::throws<\g<type>>({point_info}, "\g<expr>", [&](){{ \g<expr>; }}, true);''',
			True
		),
		regex.Regex('throw_equal',
			r'\[\[(?P<attr>[^\]]+ThrowEqual)\((?P<inst>.+?)\)\]\](?P<expr>[^;]+);',
			lambda match: r'''/* [[{attr}({inst})]] {expr} */
::Tial::Testing::Check::throwsEqual({{point_info}}, "{inst_s}", {inst}, "{expr_s}", [&](){{{{ {expr}; }}}});'''.format(
				attr=utils.escape_comment(match.group('attr'), True),
				inst=utils.escape_expr(match.group('inst'), True),
				inst_s=utils.escape_string(match.group('inst'), True),
				expr=utils.escape_expr(match.group('expr'), True),
				expr_s=utils.escape_string(match.group('expr'), True)
			),
			True
		),
		regex.Regex('no_throw',
			r'\[\[(?P<attr>[^\]]+NoThrow)\]\](?P<expr>[^;]+);',
			lambda match: r'''/* [[{attr}]] {expr_c} */
::Tial::Testing::Check::noThrows<typename std::decay<decltype({expr})>::type>({{point_info}}, "{expr_s}", [&]()->typename std::decay<decltype({expr})>::type{{{{ return {expr}; }}}});'''.format(
				attr=match.group('attr'),
				expr=utils.escape_expr(match.group('expr'), True),
				expr_c=utils.escape_comment(match.group('expr'), True),
				expr_s=utils.escape_string(match.group('expr'), True)
			),
			True
		),
		regex.Regex('data_set',
			r'\[\[(?P<attr>[^\]]+DataSet)\((?P<name>.*?)\)\]\]\s+(?P<expr>[^;]+);',
			lambda match: r'/* [[{attr}({name})]] */ _runWithData({name}, {expr});'.format(
				attr=match.group('attr'),
				name=(match.group('name') if len(match.group('name')) != 0 else '""'),
				expr=match.group('expr')
			)
		),
		regex.Regex('data_base',
			r'\[\[(?P<attr>[^\]]+DataBase)\]\]\s+(?P<name>[^;]+);',
			r'''
template<typename DATA>
void _runWithData(const std::experimental::string_view &name, const DATA &data) {
	_caller([this, &data](){
		(*this)(data);
	}, std::string(this->caseName()) + "/" + name.to_string());
}'''
		),
		regex.Regex('data_function',
			r'\[\[(?P<attr>[^\]]+DataFunction)\]\]\s+(?P<ret>\S+)\s+(?P<name>\S+)\(\)',
			r'''

template<typename DATA>
void _runWithData(const std::experimental::string_view &name, const DATA &data) {
	_caller([this, &data](){
		(*this)(data);
	}, std::string(this->caseName()) + "/" + name.to_string());
}

/* [[\g<attr>]] */ \g<ret> \g<name>()'''
		)
	])

	def __init__(self, infile, oufile, verbose, track_original_lines):
		self.infile = infile
		self.oufile = oufile
		self.verbose = verbose
		self.track_original_lines = track_original_lines

		if not self.oufile.parent.exists():
			self.oufile.parent.mkdir(parents=True)

		self.source = open(str(self.infile), 'rb').read().decode('utf-8')
		self.source_line = 1
		self.hashline_flags = ''

		self.target = io.StringIO()

		self.aliases = {}
		self.suites = []
		self.blocks_stack = []
		self.cpp_blocks_stack = CppBlockStack()
		self.line_no_cache = [1]
		self.last_line_directive = 1

	def log(self, message):
		if self.verbose:
			sys.stderr.write('{}\n'.format(message))

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

	def write_line_directive(self):
		self.target.write('#line {} "{}"\n'.format(
			self.source_line,
			utils.escape_filename(str(self.infile), True)
		))

	def write_prefixed_line(self, line):
		if len(line) != 0:
			self.target.write('{}\n'.format(line))

	def write_prefixed(self, string, increment_source_line):
		self.write_line_directive()
		line = ''
		for i in range(len(string)):
			ch = string[i]
			if ch == '\n':
				self.write_prefixed_line(line)
				if increment_source_line:
					if len(line) == 0:
						self.write_prefixed_line('/* */')
					self.source_line += 1
				line = ''
			else:
				line += ch
		self.write_prefixed_line(line)

	def write_original(self, original):
		self.write_prefixed(original, True)

	def write_replacement(self, replacement, original):
		self.write_prefixed(replacement, False)
		self.source_line += original.count('\n')

	def re_replace(self, match, regex, replacement):
		try:
			a = match.start()
			b = match.end()
			assert b == self.pos
			original = self.source[a:b]
			self.write_original(self.source[self.last_pos:a])

			if replacement is None:
				self.write_original(original)
			else:
				replacement = regex.regex.sub(replacement, original)
				#self.source_line += self.source.count('\n', self.last_pos, b)
				point_info = '(::Tial::Testing::Check::PointInfo{{"{file}", {line}, caseName()}})'.format(
					file=utils.escape_filename(str(self.infile), True),
					line=self.source_line
				)
				if regex.post_process:
					try:
						replacement = replacement.format(point_info=point_info)
					except:
						sys.stderr.write('Replacement: "{}"\n'.format(replacement))
						raise
				self.write_replacement(replacement, original)
#				self.target.write('# {} "{}"{}\n'.format(
#					self.source_line,
#					utils.escape_filename(str(self.infile), True),
#					self.hashline_flags
#				))
			self.last_pos = self.pos
		except:
			sys.stderr.write('last_pos = {}, a = {}, b = {}'.format(self.last_pos, a, b))
			raise

	def check_attr(self, attr, spec):
		attr = attr.split('::')
		for i in range(len(attr)):
			if attr[i] in self.aliases:
				attr[i] = self.aliases[attr[i]]
		attr = '::'.join(attr)
		return attr == spec

	def get_next(self):
		(match, regex) = self.res.search(str(self.source), self.pos)
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
		handler += '({point_info}, '
		handler += '"'+utils.escape_string(expression, True)+'"'
		if not e:
			handler += ', ('+expression+')'
		else:
			handler += ', "' + utils.escape_string(e, True) + '"'
			handler += ', "' + utils.escape_string(l, True) + '", ' + l
			handler += ', "' + utils.escape_string(r, True) + '", ' + r
		handler += ')'
		replacement = replacement + handler
		return replacement + ';'

	def dump_stack(self):
		self.log('stack: '+repr(self.blocks_stack))

	def _escape_for_log(self, text):
		text = text.replace('\r', '\\r')
		text = text.replace('\n', '\\n')
		text = text.replace('\t', '\\t')
		return text

	def produce_output(self, fn_output):
		fn_output(self.target.getvalue())
		return

		line = 0
		i = 0
		last = 0
		source_length = len(self.source)
		while True:
			if line != self.source.lines[i]:
				fn_output(self.source[last:i])
				last = i
				original_line = self.source.lines[i]
				line_cmd = '\n# {} "{}"\n'.format(original_line+1, str(self.infile).replace('\\', '\\\\'))
				fn_output(line_cmd)
				line = original_line

			if self.source[i] == '\n':
				line += 1

			i += 1
			if i >= source_length:
				break
		fn_output(self.source[last:])

	def process(self):
		self.log('Processing...')

		self.pos = 0
		self.last_pos = 0
		while True:
			(match, regex) = self.get_next()
			if match is None or regex is None:
				break

			if self.verbose:
				self.log('Found match of {} (bytes {} - {})'.format(
					regex.name, match.start(), match.end()
				))
				source = str(self.source)
				left = source[match.start()-3:match.start()]
				middle = source[match.start():match.end()]
				right = source[match.end():match.end()+3]
				left = self._escape_for_log(left)
				middle = self._escape_for_log(middle)
				right = self._escape_for_log(right)
				self.log(left + middle + right)
				self.log((' '*len(left)) + '^' + ((len(middle)-2)*'-') + '^')

			try:
				replacement = regex.replacement
				if len(self.blocks_stack) > 0 and isinstance(self.blocks_stack[-1], DoubleQuoting):
					if regex.name == 'double_quote':
						del self.blocks_stack[-1]
						self.log('end-of-double_quote')
						self.dump_stack()
					else:
						self.pos = match.start()+1
						self.log('within double quoting ({}), this match is ignored'.format(self.blocks_stack[-1]))
				elif len(self.blocks_stack) > 0 and isinstance(self.blocks_stack[-1], SingleQuoting):
					if regex.name == 'single_quote':
						escaped_quote = False
						i = match.start()-1
						while self.source[i] == '\\':
							escaped_quote = not escaped_quote
							i -= 1
							if i < 0:
								raise Exception('Reached start of file while looking into escapers sequence')

						if not escaped_quote:
							del self.blocks_stack[-1]
							self.log('end-of-single-quote')
							self.dump_stack()
						else:
							self.log('end-of-single-quote was escaped, ignored')
					else:
						self.pos = match.start()+1
						self.log('within single quoting ({}), this match is ignored'.format(self.blocks_stack[-1]))
				else:
					if regex.name == 'hashline':
						self.infile = match.group('file')
						self.source_line = int(match.group('line'))
						self.hashline_flags = [ int(i) for i in  match.group('flags').split() ]
						self.hashline_flags = ' '+' '.join([ str(i) for i in self.hashline_flags if i in (3, 4)])
					elif regex.name == 'inline_comment':
						pass
					elif regex.name == 'block_comment':
						pass
					elif regex.name == 'double_quote':
						self.blocks_stack.append(DoubleQuoting(match.start(), self.source_line+1))
						self.dump_stack()
						continue
					elif regex.name == 'single_quote':
						self.blocks_stack.append(SingleQuoting(match.start(), self.source_line+1))
						self.dump_stack()
						continue
					elif regex.name == 'attr_ns_alias':
						self.aliases[match.group('alias')] = match.group('original')
					elif regex.name == 'attr_ns':
						if self.check_attr(match.group('attr'), self.attr_suite):
							suite = Suite(match.group('ns_name'), self.full_name(match.group('ns_name')))
							self.suites.append(suite)
							self.blocks_stack.append(suite)
							self.dump_stack()
						else:
							raise Exception('Unknown attribute: '+match.group('attr'))
					elif regex.name == 'attr_cl' or regex.name == 'attr_clb':
						if (self.check_attr(match.group('attr'), self.attr_case) or
								self.check_attr(match.group('attr'), self.attr_casebase)
							):
							case = Case(
								match.group('cl_name'),
								self.full_name(match.group('cl_name')),
								regex.name == 'attr_clb',
								self.cpp_blocks_stack
							)
							if not case.base:
								assert isinstance(self.blocks_stack[-1], Suite)
								self.blocks_stack[-1].cases.append(case)
							self.blocks_stack.append(case)
							self.dump_stack()
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
					elif regex.name == 'data_function':
						if not self.check_attr(match.group('attr'), self.attr_data_function):
							raise Exception('Unknown attribute: '+match.group('attr'))
						if not isinstance(self.blocks_stack[-1], Block):
							self.blocks_stack[-1].has_data = match.group('name')
					elif regex.name == 'data_set':
						if not self.check_attr(match.group('attr'), self.attr_data_set):
							raise Exception('Unknown attribute: '+match.group('attr'))
					elif regex.name == 'data_base':
						if not self.check_attr(match.group('attr'), self.attr_data_base):
							raise Exception('Unknown attribute: '+match.group('attr'))
						if not isinstance(self.blocks_stack[-1], Block):
							function = match.group('name').split('::')
							self.blocks_stack[-1].has_data = 'static_cast<{class_name}&>(*this)._caller = _caller;static_cast<{class_name}&>(*this).{function_name}'.format(
								class_name='::'.join(function[:-1]),
								function_name=function[-1]
							)
					elif regex.name == 'brace_open':
						self.blocks_stack.append(Block(match.start(), match.end(), self.source_line+1, match.groups()))
						self.dump_stack()
					elif regex.name == 'brace_close':
						if not isinstance(self.blocks_stack[-1], Block):
							replacement = self.blocks_stack[-1].close_block(regex.replacement)
						del self.blocks_stack[-1]
						self.dump_stack()
					elif regex.name == 'cpp_if':
						directive = match.group('directive')
						content = match.group('content')
						if directive == 'if':
							self.cpp_blocks_stack.append(CppBlock())
							self.cpp_blocks_stack[-1].append(CppBlock.If(content))
						elif directive == 'elif':
							self.cpp_blocks_stack[-1].append(CppBlock.Elif(content))
						elif directive == 'endif':
							self.cpp_blocks_stack.pop()
						else:
							raise Exception('Unknown preprocessor directive: {} (content: {})'.format(
								match.group('directive'), match.group('content')
							))
					elif regex.name == 'cpp':
						pass
					else:
						raise Exception('Unknown regex: '+regex.name)
					self.re_replace(match, regex, replacement)
			except Exception as e:
				sys.stderr.write('{}\n'.format(e))
				sys.stderr.write('In line: {}\n'.format(self.source_line+1))
				sys.stderr.write('Expression: {}\n'.format(match.group(0)))
				sys.stderr.write('Regex: {}\n'.format(regex))
				sys.stderr.write('Match: {}\n'.format(match))
				sys.stderr.write('Blocks stack: {}\n'.format(self.blocks_stack))
				raise

		self.target.write('\n')
		self.target.write('namespace {\n')
		for suite in self.suites:
			self.target.write('::Tial::Testing::Suite<\n')
			if len(suite.cases) > 0:
				self.target.write('\t' + ',\n\t'.join([ case.generate_registration() for case in suite.cases ]) + '\n')
			self.target.write('> __testSuite_{name_raw}("{name}");\n'.format(
				name=suite.full_name, name_raw=suite.full_name.replace('::', '__')
			))
		self.target.write('}\n')

		with open(str(self.oufile), 'wb') as output:
			if self.track_original_lines:
				self.produce_output(lambda x: output.write(x.encode('utf-8')))
