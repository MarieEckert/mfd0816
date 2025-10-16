readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly NC='\033[0m'

log_info() {
	echo -e "${YELLOW}>>> ${@}${NC}"
}

log_detail() {
	echo -e "  > ${@}"
}

log_success() {
	echo -e "${GREEN}>>> ${@}${NC}"
}

log_error() {
	echo -e "${RED}>>> ${@}${NC}"
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

find_cpp_files() {
	find "$@" -type f -regex ".*\.\(cpp\|hpp\)$"
}
