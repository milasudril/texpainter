#!/usr/bin/env python3

import sys
import md_doc
import cppsource
import image_processor
import json
import subprocess


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
	if str == 'Image spectrum':
		return 'GrayscaleComplexPixels'
	if str == 'Grayscale image':
		return 'GrayscaleRealPixels'
	if str == 'RGBA image':
		return 'RgbaPixels'
	if str == 'Palette':
		return 'Palette'
	if str == 'Topography data':
		return 'TopographyData'
	if str == 'Point cloud':
		return 'PointCloud'
	if str == 'Polyline set':
		return 'PolylineSet'
	raise Exception('Unsupported port type ' + str)


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
		return image_processor.ImageProcessor(
			title,
			body=''.join(content.paragraphs['Implementation'].paragraphs['Source code'].text),
			processor_id=content.paragraphs['Tags'].paragraphs['Id'].text[0],
			release_state=content.paragraphs['Tags'].paragraphs['Release state'].text[0]
			if 'Release state' in content.paragraphs['Tags'].paragraphs else 'Experimental',
			input_ports=getPorts(content.paragraphs['Input ports'].paragraphs)
			if 'Input ports' in content.paragraphs else dict(),
			output_ports=getPorts(content.paragraphs['Output ports'].paragraphs),
			params=getParams(content.paragraphs['Parameters'].paragraphs if 'Parameters' in
			content.paragraphs else dict()),
			user_includes=content.paragraphs['Implementation'].paragraphs['Includes'].text
			if 'Includes' in content.paragraphs['Implementation'].paragraphs else [],
			user_includes_start=content.paragraphs['Implementation'].paragraphs['Includes'].line_no
			+ 1 if 'Includes' in content.paragraphs['Implementation'].paragraphs else 0,
			description='\\n\\n'.join(content.text),
			category=content.paragraphs['Tags'].paragraphs['Category'].text[0],
			impl_start=content.paragraphs['Implementation'].paragraphs['Source code'].line_no + 1)


def loadDocument(filename):
	with open(filename) as f:
		return md_doc.loadParagraphs(f)


def validate(cppsource, compiler):
	args = []
	args.append(compiler[0])
	args.extend(
		['-x', 'c++', '-std=c++20', '-Wall', '-Wextra', '-O3', '-Werror', '-c', '-o', '/dev/null'])
	for item in compiler[1]:
		args.append('-iquote%s' % item)
	args.append('-')

	pipe = subprocess.Popen(args, stdin=subprocess.PIPE)
	pipe.communicate(input=cppsource.encode('utf-8'))
	return pipe.returncode


def writeImgproc(cppsource, target):
	with open(target, 'w') as f:
		f.write(cppsource)


def getConfig(maikecfg):
	with open(maikecfg) as f:
		return json.load(f)


def getCompiler(config):
	for item in config['target_hooks']:
		if item['name'] == 'targetcxx_default':
			return (item['config']['objcompile']['name'], item['config']['iquote'])


config = getConfig(sys.argv[1] + '/maikeconfig.json')
compiler = getCompiler(config)
doc = loadDocument(sys.argv[2])
imgproc = makeImgproc(doc)
cppsource = cppsource.makeCppSource(imgproc, sys.argv[2])
validation_result = validate(cppsource, compiler)
if validation_result == 0:
	writeImgproc(cppsource, sys.argv[3])
else:
	print(cppsource)
exit(validation_result)
