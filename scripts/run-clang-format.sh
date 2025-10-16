#!/usr/bin/env bash
source $(dirname "$0")/helpers.sh

DIRECTORIES=(
	asm/
	emu/
	shared/
	test/
)

format_files=()

log_info "$(clang-format --version)"

if [ -n "$1" ]; then
	log_info "Formatting files changed compared to origin/main"
	mapfile -t format_files < <(
		git diff --name-only origin/main...HEAD -- "${DIRECTORIES[@]}" \
		| grep "\.\(cpp\|hpp\)$" || true
	)
else
	log_info "Formatting all files"
	mapfile -t format_files < <(
		find_cpp_files ${DIRECTORIES[@]}
	)
fi

for file in "${format_files[@]}"; do
	clang-format -i "${file}" \
		&& print_status "ok" "formatted ${file}" \
		|| print_status "fail" "failed to format ${file}"
done
