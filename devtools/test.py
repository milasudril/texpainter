import make_imgproc
import make_imgproc.md_doc

f = open('../imgproc_new/add_image_spectra.md')
lines = f.readlines()
f.close()

doc = make_imgproc.md_doc.loadParagraphs(lines)

def dump(para, level):
	for title, content in para.items():
		for k in range(0, level + 1):
			print('#', end = '')
		print(title, end = '')
		print(content.text, end = '')
		dump(content.paragraphs, level + 1)
dump(doc.paragraphs, 0)
#print(doc.paragraphs)
#print(doc.paragraphs[' Add image spectra\n'].paragraphs)

