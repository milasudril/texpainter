import re


class Paragraph:
	def __init__(self):
		self.paragraphs = dict()  # Insertion order preserved as of Python 3.7
		self.text = []
		self.line_no = 0


def makeHeader(md_line):
	match = re.match(r'^__[^:]*:__', md_line)
	if match:
		return (0, md_line[2:md_line.find(':')], md_line[match.end():].strip())

	level = 0
	for ch in md_line:
		if ch == '#':
			level = level + 1
		else:
			if level != 0 and ch == ' ':
				return (level, md_line[level:].strip())
			return (level, '', md_line)


def processLine(line, line_no, current_heading_level, current_paragraph, contexts, has_inline_header):
	res = makeHeader(line)
	if res != None:
		heading_level = res[0]
		if heading_level == 0:
			if len(res[1]) != 0:
				if has_inline_header:
					current_paragraph = contexts.pop()
				header = res[1]
				current_paragraph.paragraphs[header] = Paragraph()
				contexts.append(current_paragraph)
				current_paragraph = current_paragraph.paragraphs[header]
				current_paragraph.line_no = line_no
				has_inline_header = True
			if len(res[2]) != 0:
				current_paragraph.text.append(res[2])

		elif heading_level > current_heading_level:
			header = res[1]
			current_paragraph.paragraphs[header] = Paragraph()
			contexts.append(current_paragraph)
			current_paragraph = current_paragraph.paragraphs[header]
			current_paragraph.line_no = line_no
			current_heading_level = heading_level
		else:
			if has_inline_header:
				has_inline_header = False
				current_paragraph = contexts.pop()

			for k in range(0, current_heading_level - heading_level + 1):
				current_paragraph = contexts.pop()

			header = header = res[1]
			current_paragraph.paragraphs[header] = Paragraph()
			contexts.append(current_paragraph)
			current_paragraph = current_paragraph.paragraphs[header]
			current_paragraph.line_no = line_no
			current_heading_level = heading_level

	return (current_heading_level, current_paragraph, contexts, has_inline_header)


def loadParagraphs(md_lines):
	current_heading_level = 0
	root = Paragraph()
	current_paragraph = root
	contexts = []
	is_code_block = False
	has_inline_header = False
	para_text = ''
	line_no = 1
	for line in md_lines:
		if line.startswith('```'):
			if is_code_block:
				current_paragraph.text.append(para_text)
				para_text = ''
			else:
				current_paragraph.line_no = line_no
			is_code_block = not is_code_block
		elif is_code_block:
			para_text += line
		else:
			line = line.strip()
			if len(line) != 0:
				if len(para_text) == 0:
					para_text = line
				else:
					para_text += ' ' + line
			else:
				line = para_text.strip()
				para_text = ''
				current_heading_level, current_paragraph, contexts, has_inline_header = processLine(
					line, line_no, current_heading_level, current_paragraph, contexts, has_inline_header)
		line_no = line_no + 1

	processLine(para_text.strip(), line_no, current_heading_level, current_paragraph, contexts,
		has_inline_header)

	return root
