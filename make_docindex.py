#!/usr/bin/python3

import os

def collect_paths(dir, depth = 1):
	ret = []
	for f in os.listdir(dir):
		fullpath = os.path.join(dir, f)

		if os.path.islink(fullpath):
			continue

		if os.path.isfile(fullpath) and f.endswith('.md'):
			ret.append((dir, f, depth))
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

def resolve_sections(paths):
	resolved_sections = []

	for f in paths:
		if f[1] == 'index.md' or f[1] == 'README.md':
			continue

		chapter = get_chapter(f)
		if chapter == None:
			continue

		section = get_header(os.path.join(f[0], f[1]))
		resolved_sections.append((chapter, section, f[2], f[0], os.path.join(f[0], f[1])))

	return resolved_sections

def gen_outline(sections):
	lines = []
	prev_chapter = ''
	for section in sections:
		if section[2] < 2:
			continue
		indent = section[2] - 1

		if section[0] != prev_chapter:
			prev_chapter = section[0]
			line = ''
			for k in range(indent - 1):
				line += '  '
			line += '* [%s](%s)'%(section[0], os.path.join(section[3], 'index.html'))
			lines.append(line)

		line = ''
		for k in range(indent):
			line += '  '
		line += '* [%s](%s.html)'%(section[1], os.path.splitext(section[4])[0])
		lines.append(line)

	return lines

def make_index_page(dir):
	with open(os.path.join(dir, 'index.md')) as f:
		lines = f.readlines()
		lines.append('## Sections')
		lines.extend(gen_outline(resolve_sections(collect_paths('.'))))
		return lines

print('\n'.join(make_index_page('.')))
