.PHONY: all
all:
	maike --configfiles=maikeconfig.json,maikeconfig-rel.json

.PHONY: debug
debug:
	maike --configfiles=maikeconfig.json,maikeconfig-dbg.json

.PHONY: clean
clean:
	rm -rf __targets*

.PHONY: format
format:
	find \( -name '*.hpp' -or -name '*.cpp' \) -and -not -path './__targets*' | xargs clang-format -i
	find -name '*.hpp' -and -not -path './__targets*' | xargs devtools/include_guard_fix.py
	find -name '*.json' -and -not -path './__targets*' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
	find -name '*.imgproc.md' | xargs devtools/imgproc_gen/format_imgproc.py
	find -name '*.py' | xargs yapf3 -i

.PHONY:
imgproc_id:
	devtools/make_imgproc_id.sh

.PHONY: linecount
linecount:
	find \( \( -name *.hpp -or -name *.cpp \) -and -not -path './pcg-cpp*' \) -exec wc -l \{\} \; | sort -nr