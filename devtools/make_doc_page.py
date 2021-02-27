#!/usr/bin/python3

import os
import sys
import subprocess
import string
import changelog
import json

def collect_paths(dir, depth=1):
	ret = []
	for f in os.listdir(dir):
		fullpath = os.path.join(dir, f)

		if depth == 2:
			if f == 'index.md' and os.path.isfile(fullpath):
				path_split = dir.split('/')
				skip = 1 + len(path_split) - depth
				ret.append((dir, f, '/'.join(path_split[skip:]), depth))
				return ret
			continue

		if os.path.islink(fullpath):
			continue

		if os.path.isfile(fullpath) and (f.endswith('.md') or f.endswith('.projinfo.json')):
			path_split = dir.split('/')
			skip = 1 + len(path_split) - depth
			ret.append((dir, f, '/'.join(path_split[skip:]), depth))
			continue

		if os.path.isdir(fullpath) and f != '.' and f != '..':
			ret.extend(collect_paths(fullpath, depth + 1))
			continue

	return ret


def get_header(file):
	if file.endswith('.projinfo.json'):
		return 'About'

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
			line += '* [%s](%s)\n' % (section[0], os.path.join(section[3], 'index.html'))
			lines.append(line)

		if os.path.split(section[4])[1] == 'index.md':
			continue

		line = ''
		for k in range(indent):
			line += '  '
		line += '* [%s](%s.html)\n' % (section[1], os.path.splitext(section[4])[0])
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
		lines = ['[« Back](javascript:history.back())\n\n']
		lines.extend(f.readlines())
		return lines


def make_changelog(page):
	with open(page) as f:
		lines = ['[« Back](javascript:history.back())\n\n']
		lines.extend(f.readlines())
		log = changelog.get()
		header = [''] * len(log[0])
		delimiter = ['--'] * len(log[0])
		lines.extend('\n|%s|\n' % '|'.join(header))
		lines.extend('|%s|\n' % '|'.join(delimiter))
		for item in log:
			lines.append('|%s|\n' % '|'.join(item))

	return lines

def make_about(page):
	data = dict()
	with open(page) as f:
		data = json.load(f)

	lines = ['[« Back](javascript:history.back())\n']
	lines.append('\n')
	lines.append('<header>\n')
	lines.append('<h1>%s %s</h1>\n' % (data['name'], changelog.describe()))
	lines.append('<summary class="title">%s</summary>\n'% data['description_short'])
	lines.append('</header>\n')
	lines.append('\n')

	lines.append('%s\n'%data['legal_info']['license_short'])
	lines.append('\n')

	lines.append('## Copyright\n')
	lines.append('\n')
	lines.append('| | |\n')
	lines.append('| - | - |\n')
	for entry in data['copyright']:
		years = []
		for year in entry['years']:
			years.append(str(year))
		lines.append('| %s | %s |\n' %(entry['author'], ', '.join(years)))
	lines.append('\n')

	lines.append('## Acknowledgement\n')
	lines.append('\n')
	lines.append('| | |\n')
	lines.append('| - | - |\n')
	for entry in data['acknowledgement']:
		lines.append('| %s | %s |\n' %(entry['who'], entry['what']))
	lines.append('\n')

	lines.append('## Build info\n')
	lines.append('\n')
	lines.append('| | |\n')
	lines.append('| - | - |\n')
	lines.append('| __Timestamp:__ | %s |\n' % os.environ['TIMESTAMP'])
	lines.append('| __Id:__ | %s |\n' % os.environ['BUILD_ID'])
	lines.append('| __Compiler:__ | %s | \n' % 'foobar')
	lines.append('| __Libraries:__ | %s | \n' % 'foobar')
	lines.append('\n')
	lines.append('\n')

	return lines


def make_doc(filename):
	if os.path.split(filename)[-1] == 'index.md':
		return make_index_page(filename)
	if os.path.split(filename)[-1] == 'changelog.git.md':
		return make_changelog(filename)
	if os.path.split(filename)[-1].endswith('.projinfo.json'):
		return make_about(filename)
	else:
		return make_content_page(filename)


def make_path_prefix(filename):
	ret = []
	for k in range(0, len(filename.split('/')) - 1):
		ret.append('..')
	return ret


def convert(lines, pandoc_args):
	cmd = ['pandoc']
	cmd.extend(pandoc_args)
	with subprocess.Popen(cmd, stdin=subprocess.PIPE) as proc:
		for line in lines:
			proc.stdin.write(line.encode('utf-8'))


def gen_webpage(src, pandoc_args):
	stylesheet = make_path_prefix(src)
	title = ['Texpainter: ', get_header(src)]
	stylesheet.append('format.css')
	pandoc_args.extend(
		['-s', '--css', '/'.join(stylesheet), '--metadata', 'pagetitle=' + ''.join(title)])
	convert(make_doc(src), pandoc_args)


gen_webpage(sys.argv[1], sys.argv[2:])
