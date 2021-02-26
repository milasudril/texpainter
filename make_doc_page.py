#!/usr/bin/python3

import os
import sys
import subprocess

def collect_paths(dir, depth = 1):
	ret = []
	for f in os.listdir(dir):
		fullpath = os.path.join(dir, f)

		if depth == 2:
			if f == 'index.md' and os.path.isfile(fullpath) and f.endswith('.md'):
				path_split=dir.split('/')
				skip = 1 + len(path_split) - depth
				ret.append((dir, f, '/'.join(path_split[skip:]), depth))
				return ret
			continue


		if os.path.islink(fullpath):
			continue

		if os.path.isfile(fullpath) and f.endswith('.md'):
			path_split=dir.split('/')
			skip = 1 + len(path_split) - depth
			ret.append((dir, f, '/'.join(path_split[skip:]), depth))
			continue

		if os.path.isdir(fullpath) and f != '.' and f != '..':
			ret.extend(collect_paths(fullpath, depth + 1))
			continue

	return ret

def get_header(file):
	with open(os.path.join(file)) as f:
		return f.readline()[2:].strip()

def get_chapter(section):
	try:
		return get_header(os.path.join(section[0], 'index.md'))
	except:
		return None

def resolve_sections(paths, current_index):
	resolved_sections = []

	for f in paths:
		filename = os.path.join(f[0], f[1])
		if filename == current_index:
			continue

		if f[1] == 'README.md':
			continue

		chapter = get_chapter(f)
		if chapter == None:
			continue

		section = get_header(filename)
		resolved_sections.append((chapter, section, f[3], f[0], os.path.join(f[2], f[1])))

	return resolved_sections

def gen_outline(sections):
	lines = []
	prev_chapter = ''
	for section in sections:
		indent = section[2] - 1
		if section[0] != prev_chapter and prev_chapter != '':
			line = ''
			for k in range(indent - 1):
				line += '  '
			line += '* [%s](%s)\n'%(section[0], os.path.join(section[3], 'index.html'))
			lines.append(line)

		if os.path.split(section[4])[1] == 'index.md':
			continue

		line = ''
		for k in range(indent):
			line += '  '
		line += '* [%s](%s.html)\n'%(section[1], os.path.splitext(section[4])[0])
		lines.append(line)

		prev_chapter = section[0]

	return lines

def make_index_page(index):
	with open(index) as f:
		dir = os.path.dirname(index)
		lines = []
		if dir == '':
			dir = '.'
		else:
			lines.append('[« To index](../index.html)\n\n')

		lines.extend(f.readlines())

		outline = gen_outline(resolve_sections(collect_paths(dir), index))

		if len(outline) > 0:
			lines.append('## Sections\n\n')
			lines.extend(outline)

		return lines

def make_content_page(page):
	with open(page) as f:
		lines = ['[« To index](index.html)\n\n']
		lines.extend(f.readlines())
		return lines

def make_doc(filename):
	if os.path.split(filename)[-1] == 'index.md':
		return make_index_page(filename)
	else:
		return make_content_page(filename)

def convert(lines, pandoc_args):
	cmd = ['pandoc']
	cmd.extend(pandoc_args)
	with subprocess.Popen(cmd, stdin=subprocess.PIPE) as proc:
		for line in lines:
			proc.stdin.write(line.encode('utf-8'))

convert(make_doc(sys.argv[1]), sys.argv[2:])