import make_imgproc.md_doc
import make_imgproc.imgproc
import make_imgproc.imgproc_skel

f = open('../imgproc_new/add_image_spectra.md')
doc = make_imgproc.md_doc.loadParagraphs(f)
f.close()

for title, content in doc.paragraphs.items():
	imgproc = make_imgproc.imgproc.ImageProcessor(title,
		body = ''.join(content.paragraphs['Implementation'].paragraphs['Source code'].text),
		processor_id = content.paragraphs['Tags'].paragraphs['Id'].text[0],
		input_ports = content.paragraphs['Input ports'].paragraphs,
		output_ports = content.paragraphs['Output ports'].paragraphs,
		params = content.paragraphs['Parameters'].paragraphs
		)

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

