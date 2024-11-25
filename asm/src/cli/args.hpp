#ifndef SASM_CLI_ARGS_HPP
#define SASM_CLI_ARGS_HPP

#include <ios>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace sibu::cli {

template <typename T>
T _getAs(std::string str) {
	std::stringstream convert;
	convert << std::boolalpha << str;

	T value;
	convert >> value;
	return value;
}

class ArgumentBase {
   public:
	ArgumentBase(std::string short_name, std::string long_name = "", bool action = false);

	ssize_t check(const std::string &name, std::string value);

   protected:
	bool m_isAction;

	std::string m_shortName;

	std::string m_longName;

	std::optional<std::string> m_stringValue;
};

template <typename T>
class Argument : public ArgumentBase {
   public:
	Argument(std::string short_name, std::string long_name = "", bool action = false)
		: ArgumentBase(short_name, long_name, action) {}

	std::optional<T> get() {
		if(this->m_value.has_value()) {
			return this->m_value;
		}

		if(!this->m_stringValue.has_value()) {
			return std::nullopt;
		}

		this->m_value = _getAs<T>(this->m_stringValue.value());

		return this->m_value;
	};

   private:
	std::optional<T> m_value;
};

class ArgumentParser {
   public:
	void parse(int argc, char **argv);

	template <typename T>
	void addArgument(Argument<T> *arg) {
		this->args.push_back(arg);
	};

   private:
	std::vector<ArgumentBase *> args;
};

}  // namespace sibu::cli

#endif