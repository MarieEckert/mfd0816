/*
 * Copyright (C) 2024  Marie Eckert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MFDASM_CLI_ARGS_HPP
#define MFDASM_CLI_ARGS_HPP

#include <ios>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace mfdasm::cli {

/**
 * @brief Helper tempalte for converting a string to the generic type T
 */
template <typename T>
T _getAs(std::string str) {
	std::stringstream convert;
	convert << std::boolalpha << str;

	T value;
	convert >> value;
	return value;
}

/**
 * @brief Base class for arguments.
 */
class ArgumentBase {
   public:
	/**
	 * @brief Construct an Argument Base.
	 * @param short_name The shorthand name for the argument.
	 * @param long_name (Optional) The long name for the argument.
	 * @param action (Optional, default false) Is it an action argument (takes no parameters).
	 */
	ArgumentBase(std::string short_name, std::string long_name = "", bool action = false);

	/**
	 * @brief Checks if the given argument and option combination works for this argument.
	 * @param name The argument name to check.
	 * @param value The value to check.
	 * @return The amount of elements consumed.
	 */
	ssize_t check(const std::string &name, std::string value);

   protected:
	bool m_isAction;

	std::string m_shortName;

	std::string m_longName;

	std::optional<std::string> m_stringValue;
};

/**
 * @brief Generic child-class of ArgumentBase, this is the class which should be used
 * in combination with ArgumentParser.
 */
template <typename T>
class Argument : public ArgumentBase {
   public:
	/** @see ArgumentBase::ArgumentBase */
	Argument(std::string short_name, std::string long_name = "", bool action = false)
		: ArgumentBase(short_name, long_name, action) {}

	/**
	 * @brief Gets the value which was set for the argument. If it has a value and it
	 * is the first call, it will first need to convert this argument from its string
	 * form to the generic type.
	 * @return std::nullopt if the argument wasnt set.
	 */
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

}  // namespace mfdasm::cli

#endif