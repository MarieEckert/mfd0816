#!/usr/bin/env bash

# ============================================================================ #
# Configuration
# ============================================================================ #
readonly CPP_EXTENSIONS="cpp|cc|cxx|c|h|hpp|hxx"
readonly CLANG_TIDY_OPTIONS="-p build/ -warnings-as-errors=*"
readonly MAIN_BRANCH="main"

readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly NC='\033[0m'

error_found=0

# ============================================================================ #
# Helper functions
# ============================================================================ #
find_cpp_files() {
	local dir="$1"
	find "$dir" -type f -regex ".*\.\(cpp\|hpp\)$"
}

print_status() {
	local status="$1"
	local msg="$2"
	if [[ "$status" == "ok" ]]; then
		echo -e "  ${GREEN}✓${NC} $msg"
	else
		echo -e "  ${RED}✗${NC} $msg"
	fi
}

# ============================================================================ #
# clang-format check
# ============================================================================ #
echo -e "${YELLOW}==> Running clang-format check...${NC}"

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
	echo -e "${RED}Formatting issues detected.${NC}"
	error_found=1
else
	echo -e "${GREEN}All $file_count files are properly formatted.${NC}"
fi

# ============================================================================ #
# clang-tidy check
# ============================================================================ #
echo -e "${YELLOW}\n==> Running clang-tidy...${NC}"

current_branch=$(git rev-parse --abbrev-ref HEAD)

if [ "$current_branch" == "$MAIN_BRANCH" ]; then
	echo "On main branch — checking all C++ sources."
	files_to_check=()
	for dir in "$@"; do
		mapfile -t tmp < <(find_cpp_files "$dir")
		files_to_check+=("${tmp[@]}")
	done
else
	echo "Checking files changed compared to ${MAIN_BRANCH}..."
	mapfile -t files_to_check < <(
		git diff --name-only "$MAIN_BRANCH"...HEAD -- "${@}" \
		| grep -E "\.(${CPP_EXTENSIONS})$" || true
	)
fi

if [ "${#files_to_check[@]}" -eq 0 ]; then
	echo "No C++ files to check with clang-tidy."
else
	echo "Found ${#files_to_check[@]} file(s) to analyze."
	tidy_failed=0

	for file in "${files_to_check[@]}"; do
		echo -n "Running clang-tidy: $file ... "
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
		echo -e "${RED}clang-tidy found warnings/errors in user code.${NC}"
		error_found=1
	else
		echo -e "${GREEN}clang-tidy passed for all files.${NC}"
	fi
fi

# ============================================================================ #
# result
# ============================================================================ #
if [ "$error_found" -ne 0 ]; then
	echo -e "\n${RED}Checks failed.${NC}"
	exit 1
else
	echo -e "\n${GREEN}All checks passed successfully.${NC}"
	exit 0
fi
