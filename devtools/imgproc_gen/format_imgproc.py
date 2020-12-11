#!/usr/bin/env python3

import sys
import md_doc
import image_processor
import subprocess

def loadDocument(filename):
	with open(filename) as f:
		return md_doc.loadParagraphs(f)

def firstRec(doc):
	for title, content in doc.paragraphs.items():
		return (title, content)

def formatSrc(string):
	pipe = subprocess.Popen(['clang-format'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	return pipe.communicate(input = string.encode('utf-8'))[0].decode('utf-8').strip()

def printDocument(doc, level = 0):
	for title, content in doc.paragraphs.items():
		if content.is_inline_header:
			print('__%s:__ '%title, end ='')

			if len(content.text) != 0:
				if content.is_code_block:
					print('\n\n```c++')
				print('\n'.join(content.text))
				if content.is_code_block:
					print('```')
				print('')

		else:
			for k in range(0, level + 1):
				print('#', end = '')
			print(' %s\n'%title)
			if len(content.text) != 0:
				if content.is_code_block:
					print('```c++')
				print('\n'.join(content.text))
				if content.is_code_block:
					print('```')
				print('')

		printDocument(content, level + 1)

doc = loadDocument(sys.argv[1])
title, content = firstRec(doc)

src_old = ''.join(content.paragraphs['Implementation'].paragraphs['Source code'].text)
includes_old = ''.join(content.paragraphs['Implementation'].paragraphs['Includes'].text)
src_new = formatSrc(src_old)
includes_new = formatSrc(includes_old)

if src_old == src_new and includes_old == includes_new:
	exit(0)


content.paragraphs['Implementation'].paragraphs['Source code'].text = src_new.split('\n')
content.paragraphs['Implementation'].paragraphs['Includes'].text = includes_new.split('\n')
printDocument(doc)




