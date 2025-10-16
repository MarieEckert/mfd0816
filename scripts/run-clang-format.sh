#!/usr/bin/env bash
DIRECTORIES=(
	asm/
	emu/
	shared/
	test/
)

for dir in "${DIRECTORIES[@]}"; do
	find "$dir" -type f -regex ".*\.\(cpp\|hpp\)$" \
		| xargs -n1 bash -c 'echo "  formatting $0" ; clang-format -i "$0"'
done
