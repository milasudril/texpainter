import re

class Paragraph:
	def __init__(self):
		self.paragraphs = dict()  # Insertion order preserved as of Python 3.7
		self.text = []

def makeHeader(md_line, current_level):
#	match = re.match(r'__[^:]*:__', md_line)
#	if match:
#		return (current_level + 1, md_line[2:md_line.find(':')], md_line[match.end():])

	level = 0
	for ch in md_line:
		if ch == '#':
			level = level + 1
		else:
			if level != 0 and ch == ' ':
				return (level, md_line[level:].strip())
			return (level, md_line, md_line)

def loadParagraphs(md_lines):
	current_heading_level = 0
	root = Paragraph()
	current_paragraph = root
	contexts = []
	is_code_block = False
	para_text = ''
	for line in md_lines:
		if line.startswith('```'):
			if is_code_block:
				current_paragraph.text.append(para_text)
				para_text = ''
			is_code_block = not is_code_block
		elif is_code_block:
			para_text += line
		else:
			line = line.strip()
			if len(line) != 0:
				para_text += line
			else:
				line = para_text
				para_text = ''
				res = makeHeader(line, current_heading_level)
				if res != None:
					heading_level = res[0]
					if heading_level == 0:
						current_paragraph.text.append(res[2])
					elif heading_level > current_heading_level:
						header = res[1] #line[heading_level + 1:].strip()
						current_paragraph.paragraphs[header] = Paragraph()
						contexts.append(current_paragraph)
						current_paragraph = current_paragraph.paragraphs[header]
						current_heading_level = heading_level
					else:
						for k in range(0,  current_heading_level - heading_level + 1):
							current_paragraph = contexts.pop()

						header = header = res[1] # line[heading_level + 1:].strip()
						current_paragraph.paragraphs[header] = Paragraph()
						contexts.append(current_paragraph)
						current_paragraph = current_paragraph.paragraphs[header]
						current_heading_level = heading_level

	return root


