#!/usr/bin/env python3

import sys
import md_doc
import image_processor
import subprocess
import io

def loadDocument(filename):
	with open(filename) as f:
		return md_doc.loadParagraphs(f)

def firstRec(doc):
	for title, content in doc.paragraphs.items():
		return (title, content)

def formatSrc(string):
	pipe = subprocess.Popen(['clang-format'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	return pipe.communicate(input = string.encode('utf-8'))[0].decode('utf-8').strip()

def printDocument(doc, level = 0, f = sys.stdout):
	for title, content in doc.paragraphs.items():
		if content.is_inline_header:
			print('__%s:__ '%title, end ='', file = f)

			if len(content.text) != 0:
				if content.is_code_block:
					print('\n\n```c++', file = f)
				print('\n'.join(content.text), file = f)
				if content.is_code_block:
					print('```', file = f)
				print('', file = f)

		else:
			for k in range(0, level + 1):
				print('#', end = '', file = f)
			print(' %s\n'%title, file = f)
			if len(content.text) != 0:
				if content.is_code_block:
					print('```c++', file = f)
				print('\n'.join(content.text), file = f)
				if content.is_code_block:
					print('```', file = f)
				print('', file = f)

		printDocument(content, level + 1, f)

def formatFile(filename):
	doc = loadDocument(filename)
	title, content = firstRec(doc)
	src_old = ''.join(content.paragraphs['Implementation'].paragraphs['Source code'].text)
	includes_old = ''.join(content.paragraphs['Implementation'].paragraphs['Includes'].text)
	src_new = formatSrc(src_old)
	includes_new = formatSrc(includes_old)
	if src_old == src_new and includes_old == includes_new:
		return

	content.paragraphs['Implementation'].paragraphs['Source code'].text = src_new.split('\n')
	content.paragraphs['Implementation'].paragraphs['Includes'].text = includes_new.split('\n')

	f = io.StringIO()
	printDocument(doc, 0, f)
	with open(filename) as doc_orig:
		orig = doc_orig.read()
		if orig == f.getvalue():
			return

	with open(filename, 'w') as output:
		print(f.getvalue(), end = '', file = output)

for filename in sys.argv[1:]:
	formatFile(filename)
