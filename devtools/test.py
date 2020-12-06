import make_imgproc.md_doc
import make_imgproc.imgproc
import make_imgproc.imgproc_skel

f = open('imgproc_new/add_image_spectra.md')
doc = make_imgproc.md_doc.loadParagraphs(f)
f.close()

def getDefaultParamValue(value):
	text = value.text[0]
	if text.startswith('(= '):
		return float(text[3:text.find(')')])
	return 0.0

def getParams(params):
	ret = dict()
	for key, value in params.items():
		ret[key] = getDefaultParamValue(value)
	return ret

def translateType(str):
	if str == 'Grayscale complex pixels':
		return 'GrayscaleComplexPixels'
	raise Exception('Unsupported port type')

def getType(value):
	text = value.text[0]
	if not text.startswith('('):
		raise Exception('Port has no type')
	return translateType(text[1:text.find(')')].strip())

def getPorts(ports):
	ret = dict()
	for key, value in ports.items():
		ret[key] = getType(value)
	return ret

for title, content in doc.paragraphs.items():
	imgproc = make_imgproc.imgproc.ImageProcessor(title,
		body = ''.join(content.paragraphs['Implementation'].paragraphs['Source code'].text),
		processor_id = content.paragraphs['Tags'].paragraphs['Id'].text[0],
		input_ports = getPorts(content.paragraphs['Input ports'].paragraphs),
		output_ports = getPorts(content.paragraphs['Output ports'].paragraphs),
		params = getParams(content.paragraphs['Parameters'].paragraphs),
		user_includes = content.paragraphs['Implementation'].paragraphs['Includes'].text)

	print(make_imgproc.imgproc_skel.makeCppSource(imgproc))



#def dump(para, level):
#	for title, content in para.items():
#		for k in range(0, level + 1):
#			print('#', end = '')
#		print(' (%s)\n'%title)
#		if len(content.text) != 0:
#			print(','.join(content.text))
#			print('')
#		dump(content.paragraphs, level + 1)
#dump(doc.paragraphs, 0)
#print(doc.paragraphs)
#print(doc.paragraphs[' Add image spectra\n'].paragraphs)

