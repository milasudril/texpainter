.PHONY: all
all:
	maike

.PHONY: format
format:
	find -name '*.hpp' -or -name '*.cpp' | xargs clang-format -i
	find -name '*.json' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
	find -name '*.imgproc.md' | xargs devtools/imgproc_gen/format_imgproc.py

.PHONY:
imgproc_id:
	devtools/make_imgproc_id.sh

.PHONY:
include_guard_fix:
	find -name '*.hpp' | xargs devtools/include_guard_fix.py

.PHONY: linecount
linecount:
	find \( \( -name *.hpp -or -name *.cpp \) -and -not -path './pcg-cpp*' \) -exec wc -l \{\} \; | sort -nr