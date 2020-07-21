.PHONY: all
all:
	maike

.PHONY: format
format:
	find -name '*.hpp' -or -name '*.cpp' | xargs clang-format -i
	find -name '*.json' -exec bash -c 'jq --tab --sort-keys . "{}" > .fmt && mv .fmt "{}"' \;
