class Paragraph:
	def __init__(self):
		self.paragraphs = dict()  # Insertion order preserved as of Python 3.7
		self.text = ''
		self.level = 0

def headingLevel(md_line):
	level = 0
	for ch in md_line:
		if ch == '#':
			level = level + 1
		else:
			if level != 0 and ch == ' ':
				return level
			return 0

def loadParagraphs(md_lines):
	current_heading_level = 0
	root = Paragraph()
	current_paragraph = root
	contexts = []
	para_count = 0
	is_code_block = False
	for line in md_lines:
		if line.startswith('```'):
			is_code_block = not is_code_block
		else:
			heading_level = headingLevel(line)
			if heading_level == 0:
				current_paragraph.text += line
			elif heading_level > current_heading_level:
				current_paragraph.paragraphs[line[heading_level:]] = Paragraph()
				current_paragraph.level = 1
				contexts.append(current_paragraph)
				current_paragraph = current_paragraph.paragraphs[line[heading_level:]]
				current_heading_level = heading_level
			else:
				for k in range(0,  current_heading_level - heading_level + 1):
					current_paragraph = contexts.pop()

				current_paragraph.paragraphs[line[heading_level:]] = Paragraph()
				contexts.append(current_paragraph)
				current_paragraph = current_paragraph.paragraphs[line[heading_level:]]
				current_heading_level = heading_level

	return root


