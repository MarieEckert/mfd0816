#!/usr/bin/env bash
source $(dirname "$0")/helpers.sh

# ============================================================================ #
# Configuration
# ============================================================================ #
readonly CLANG_TIDY_OPTIONS="-p build/ -warnings-as-errors=*"
readonly MAIN_BRANCH="origin/main"

error_found=0

log_info "$(clang-format --version)"
log_info "clang-tidy version: $(clang-tidy --version)"

# ============================================================================ #
# clang-format check
# ============================================================================ #
log_info "Running clang-format check..."

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

format_changed=0
file_count=0

for dir in "$@"; do
	while IFS= read -r file; do
		((file_count++)) || true
		mkdir -p "$tmpdir/$(dirname "$file")"
		clang-format "$file" > "$tmpdir/$file"
		diff="$(diff --color=always "$file" "$tmpdir/$file")"
		if [[ ! -z "$diff" ]]; then
			print_status "fail" "clang-format: $file (would be reformatted)"
			echo "${diff}" | sed 's/^/    /'
			format_changed=1
		else
			print_status "ok" "clang-format: $file"
		fi
	done < <(find_cpp_files "$dir")
done

if [ "$format_changed" -ne 0 ]; then
	log_error "Formatting issues detected."
	error_found=1
else
	log_success "All $file_count files are properly formatted."
fi

# ============================================================================ #
# clang-tidy check
# ============================================================================ #
log_info "Running clang-tidy..."

current_branch=$(git rev-parse --abbrev-ref HEAD)

if [ "$current_branch" == "$MAIN_BRANCH" ]; then
	log_detail "On main branch — checking all C++ sources."
	files_to_check=()
	for dir in "$@"; do
		mapfile -t tmp < <(find_cpp_files "$dir")
		files_to_check+=("${tmp[@]}")
	done
else
	log_detail "Checking files changed compared to ${MAIN_BRANCH}..."
	mapfile -t files_to_check < <(
		git diff --name-only "$MAIN_BRANCH"...HEAD -- "${@}" \
		| grep "\.\(cpp\|hpp\)$" || true
	)
fi

if [ "${#files_to_check[@]}" -eq 0 ]; then
	log_detail "No C++ files to check with clang-tidy."
else
	log_detail "Found ${#files_to_check[@]} file(s) to analyze."
	tidy_failed=0

	for file in "${files_to_check[@]}"; do
		log_detail "Running clang-tidy: $file ... "
		output=$(clang-tidy ${CLANG_TIDY_OPTIONS} "$file" 2>&1 || true)

		if echo "$output" | grep -E "warning:|error:" | grep -v "/usr/include" >/dev/null; then
			echo -e "${RED}✗${NC}"
			echo "$output" | sed 's/^/    /'
			tidy_failed=1
		else
			echo -e "${GREEN}✓${NC}"
		fi
	done

	if [ "$tidy_failed" -ne 0 ]; then
		log_error "clang-tidy found warnings/errors in user code."
		error_found=1
	else
		log_success "clang-tidy passed for all files."
	fi
fi

# ============================================================================ #
# result
# ============================================================================ #
if [ "$error_found" -ne 0 ]; then
	log_error "Checks failed."
	exit 1
else
	log_success "All checks passed successfully."
	exit 0
fi
