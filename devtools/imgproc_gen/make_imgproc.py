#!/usr/bin/env python3

import sys
import md_doc
import cppsource
import image_processor

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

def makeImgproc(doc):
	for title, content in doc.paragraphs.items():
		# Only take first record
		return image_processor.ImageProcessor(title,
			body = ''.join(content.paragraphs['Implementation'].paragraphs['Source code'].text),
			processor_id = content.paragraphs['Tags'].paragraphs['Id'].text[0],
			input_ports = getPorts(content.paragraphs['Input ports'].paragraphs),
			output_ports = getPorts(content.paragraphs['Output ports'].paragraphs),
			params = getParams(content.paragraphs['Parameters'].paragraphs),
			user_includes = content.paragraphs['Implementation'].paragraphs['Includes'].text,
			description = '\\n\\n'.join(content.text),
			category = content.paragraphs['Tags'].paragraphs['Category'].text[0])

	print()
	return  cppsource.makeCppSource(imgproc)

def loadDocument(filename):
	with open(filename) as f:
		return md_doc.loadParagraphs(f)

def writeImgproc(imgproc, filename):
	str = cppsource.makeCppSource(imgproc)
	with open(filename, 'w') as f:
		f.write(str)

doc = loadDocument(sys.argv[1])
imgproc = makeImgproc(doc)
writeImgproc(imgproc, sys.argv[2])



