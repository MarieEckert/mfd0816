#!/usr/bin/env bash

DIRECTORIES=(
	asm/
	emu/
	shared/
	test/
)

format_files=()

if [ -n "$1" ]; then
	mapfile -t format_files < <(
		git diff --name-only origin/main...HEAD -- "${DIRECTORIES[@]}" \
		| grep "\.\(cpp\|hpp\)$" || true
	)
else
	mapfile -t format_files < <(
		find ${DIRECTORIES[@]} -type f -regex '.*\.\(cpp\|hpp\)$'
	)
fi

for file in "${format_files[@]}"; do
	echo "  formatting ${file}"
	clang-format -i "${file}"
done
