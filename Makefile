.EXPORT_ALL_VARIABLES:

BUILD_ID=$(shell devtools/make_imgproc_id.sh)
TIMESTAMP=$(shell LC_ALL="C" date)
VCS_TAG=$(shell git describe --always --dirty)

.PHONY: all
all: release doc manpages

.PHONY: release
release:
	maike --configfiles=buildconfig/base.json,buildconfig/release.json

.PHONY: debug
debug:
	maike --configfiles=buildconfig/base.json,buildconfig/debug.json

.PHONY: doc
doc: release
	mkdir -p __targets_doc
	cp __targets_rel/externals.json __targets_doc/app_externals.json
	cp __targets_rel/maikeconfig.json __targets_doc/app_config.json
	cp __targets_rel/app/dumpmenu __targets_doc/dumpmenu
	maike --configfiles=buildconfig/doc.json
	rm -f __targets_doc/app_externals.json
	rm -f __targets_doc/app_config.json
	rm -f __targets_doc/dumpmenu
	mkdir -p __targets_rel/share/help/C/texpainter
	mkdir -p __targets_dbg/share/help/C/texpainter
	cp -r __targets_doc/* __targets_rel/share/help/C/texpainter
	cp -r __targets_doc/* __targets_dbg/share/help/C/texpainter

.PHONY: manpages
manpages:
	maike --configfiles=buildconfig/manpage.json

.PHONY: doc_check
doc_check: doc
	htmlproofer __targets_doc

.PHONY: archive
archive:
	devtools/vcs_info.py
	mkdir -p __targets_rel
	tar --exclude='__pycache__' --exclude='__targets*' --exclude='.git' --xform s:'\(.*\)':'texpainter_$(VCS_TAG)/\1': -zcf __targets_rel/texpainter_$(VCS_TAG).tar.gz *

.PHONY: clean
clean:
	rm -rf __targets*

.PHONY: clean_doc
clean_doc:
	rm -rf __targets_doc

.PHONY: format
format:
	find \( -name '*.hpp' -or -name '*.cpp' \) -and -not -path './__targets*' | xargs clang-format -i
	find -name '*.hpp' -and -not -path './__targets*' | xargs devtools/include_guard_fix.py
	find -name '*.json' -and -not -path './__targets*' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
	find -name '*.imgproc.md' | xargs devtools/imgproc_gen/format_imgproc.py
	find -name '*.py' | xargs yapf3 -i

DESTDIR?=""
PREFIX?="/usr"
.PHONY: install
install: release doc manpages
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/man/man1
	mkdir -p $(DESTDIR)$(PREFIX)/share/mime/packages
	cp __targets_rel/app/texpainter $(DESTDIR)$(PREFIX)/bin/texpainter
	cp __targets_rel/cli/texporter $(DESTDIR)$(PREFIX)/bin/texporter
	cp __targets_man/app/texpainter.man.1 $(DESTDIR)$(PREFIX)/man/man1/texpainter.1
	cp __targets_man/cli/texporter.man.1 $(DESTDIR)$(PREFIX)/man/man1/texporter.1
	cp xdg/application-x-texpainter.xml $(DESTDIR)$(PREFIX)/share/mime/packages
	cp xdg/texpainter.desktop $(DESTDIR)$(PREFIX)/share/applications
	cp __targets_rel/xdg/texpainter.png $(DESTDIR)$(PREFIX)/share/pixmaps
	mkdir -p $(DESTDIR)$(PREFIX)/share/help/C/texpainter
	cp -r __targets_rel/share/help/C/texpainter/* $(DESTDIR)$(PREFIX)/share/help/C/texpainter
	find $(DESTDIR)$(PREFIX)/share/help/C/texpainter -type d | xargs chmod o+x
	find $(DESTDIR)$(PREFIX)/share/help/C/texpainter | xargs chmod o+r


.PHONY:
imgproc_id:
	devtools/make_imgproc_id.sh

.PHONY: linecount
linecount:
	find \( \( -name *.hpp -or -name *.cpp \) -and -not -path './pcg-cpp*' \) -exec wc -l \{\} \; | sort -nr