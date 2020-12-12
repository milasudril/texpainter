#!/usr/bin/env python3

import sys
import os

def splitall(path):
	allparts = []
	while 1:
		parts = os.path.split(path)
		if parts[0] == path:  # sentinel for absolute paths
			allparts.insert(0, parts[0])
			break
		elif parts[1] == path:  # sentinel for relative paths
			allparts.insert(0, parts[1])
			break
		else:
			path = parts[0]
		allparts.insert(0, parts[1])
	return allparts


def makeIncludeGuard(filename):
	cwd = os.path.split(os.getcwd())[1]
	path = splitall(filename)
	if path[0] == '.':
		path = path[1:]
	path = [cwd] + path

	parts = []
	for item in path:
		res = item.replace('_', '')
		res = res.replace('-', '')
		res = res.replace('.', '_')
		res = res.upper()
		parts.append(res)

	return '_'.join(parts)


for filename in sys.argv[1:]:
	with open(filename) as f:
		guard = makeIncludeGuard(filename)
		first_ifndef = True
		lines = []
		for line in f:
			line_to_print = line
			if line.startswith('#ifndef ') and first_ifndef:
				line_to_print = '#ifndef %s\n#define %s\n' % (guard, guard)
				first_ifndef = False
				f.__next__()
			lines.append(line_to_print)

	result = ''.join(lines)
	original = open(filename).read()
	if result != original:
		with open(filename, 'w') as f:
			f.write(result)
