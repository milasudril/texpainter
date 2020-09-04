.PHONY: all
all:
	maike

.PHONY: format
format:
	devtools/include_guard_fix.py
	find -name '*.hpp' -or -name '*.cpp' | xargs clang-format -i
	find -name '*.json' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;

.PHONY:
imgproc_id:
	devtools/make_imgproc_id.sh

.PHONY: linecount
linecount:
	find \( \( -name *.hpp -or -name *.cpp \) -and -not -path './pcg-cpp*' \) -exec wc -l \{\} \; | sort -nr