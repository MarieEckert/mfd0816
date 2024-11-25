#include "args.hpp"

namespace sibu::cli {

/* class ArgumentBase */

ArgumentBase::ArgumentBase(std::string short_name, std::string long_name, bool action)
	: m_isAction(action), m_shortName(short_name), m_longName(long_name) {}

ssize_t ArgumentBase::check(const std::string &name, std::string value) {
	if(name != this->m_shortName && name != this->m_longName) {
		return -1;
	}

	this->m_stringValue = !this->m_isAction ? value : "true";
	return this->m_isAction ? 0 : 1;
}

/* class ArgumentParser */

void ArgumentParser::parse(int argc, char **argv) {
	std::vector<std::string> str_args;
	str_args.reserve(argc);

	for(int ix = 0; ix < argc; ix++) {
		str_args.emplace_back(argv[ix]);
	}

	for(size_t ix = 0; ix < str_args.size(); ix++) {
		for(const auto &arg: this->args) {
			ssize_t inc =
				arg->check(str_args[ix], ix + 1 < str_args.size() ? str_args[ix + 1] : "");
			;
			if(inc == -1) {
				continue;
			}

			ix += inc;
			break;
		}
	}
}

}  // namespace sibu::cli