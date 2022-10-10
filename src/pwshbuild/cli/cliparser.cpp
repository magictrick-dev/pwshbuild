/**
 * ---------------------------------------------------------------------------------------------------------------------
 * cliparser.cpp
 * 		Implementation for cliparser.h, handles command line parsing.
 * ---------------------------------------------------------------------------------------------------------------------
 * pwshbuild - Developed by Christopher N. DeJong, @Github 0xDATAWOLF, June 2022
 * A project & script generator for CMake using PowerShell.
 * 
 * Version 1.2.0 - The not "janky AF" version.
 * ---------------------------------------------------------------------------------------------------------------------
 */
#include <pwshbuild/cli/cliparser.h>

/**
 * @brief Default constructor. Sets to known values.
 */
CLIArgument::
CLIArgument() :
_value(""), _next_arg(nullptr), _prev_arg(nullptr)
{

}

/**
 * @brief CLIArgument constructor. Sets to known values.
 */
CLIArgument::
CLIArgument(std::string value) :
_value(value), _next_arg(nullptr), _prev_arg(nullptr)
{

}

/**
 * @brief Sets the argument value to the provided string.
 * @param value The value to set the argument to.
 */
void CLIArgument::
set_value(std::string value)
{
	this->_value = value;
}

/**
 * @brief Returns the value of the argument.
 * @returns A string which defines the value of this argument.
 */
std::string CLIArgument::
get_value() const
{
	return this->_value;
}

/**
 * @brief Returns the right-adjacent CLI argument.
 */
CLIArgument* CLIArgument::
next() const
{
	return this->_next_arg;
}

/**
 * @brief Returns the left-adjacent CLI argument.
 */
CLIArgument* CLIArgument::
prev() const
{
	return this->_prev_arg;
}

/**
 * @brief An operator overload for printing a CLIArgument to an output stream.
 */
std::ostream&
operator<<(std::ostream& lhs, const CLIArgument& rhs)
{
	lhs << rhs._value;
	return lhs;
}

/**
 * @brief An operator overload for printing a CLIArgument to an output stream.
 */
std::ostream&
operator<<(std::ostream& lhs, const CLIArgument* rhs)
{
	lhs << rhs->_value;
	return lhs;
}

/**
 * -----------------------------------------------------------------------------
 * CLIParser Implementation
 * -----------------------------------------------------------------------------
 */

/**
 * @brief Default constructor.
 */
CLIParser::
CLIParser()
{
	return; // Do nothing.
}

/**
 * @brief Constructor which forwards the argc/argv to CLIParser::parse.
 * @param argc The argument count set by main.
 * @param argv The argument vector set by main.
 */
CLIParser::
CLIParser(int argc, char** argv)
{
	// Forward to the parser.
	this->parse(argc, argv);
	return;
}

/**
 * @brief Parses argc/argv and formats
 * @param argc The argument count set by main.
 * @param argv The argument vector set by main.
 */
void CLIParser::
parse(int argc, char** argv)
{

	// The invocation path is always the first element in the arg vector.
	this->_invocation_path = argv[0];

	// Set the argument values.
	this->_arguments.resize(argc-1);
	for (int index = 1; index < argc; ++index)
	{

		// Strip the heading dashes.
		int c_index = 0;
		while (argv[index][c_index] != '\0')
		{
			if (argv[index][c_index] != '-')
				break;
			c_index++;
		}

		// Set the string without the heading dashes.
		std::string base_string = argv[index];
		this->_arguments[index-1]._value = base_string.substr(c_index, base_string.length());

		// Set prev/next.
		if (index != 1)
			this->_arguments[index-1]._prev_arg = &this->_arguments[index-2];
		if (index < argc-1)
			this->_arguments[index-1]._next_arg = &this->_arguments[index];

	}

	return;
}

/**
 * @brief Retrieves a CLIArgument through a given key.
 */
CLIArgument* CLIParser::
get(std::string key)
{
	for (int i = 0; i < this->_arguments.size(); ++i)
		if (this->_arguments[i]._value == key)
			return &this->_arguments[i];
	return nullptr;
}

bool CLIParser::
is_set(std::string key)
{
	CLIArgument* search_results = this->get(key);
	if (search_results == nullptr) return false;
	else return true;
}

/**
 * @brief Returns the size of the arguments list.
 */
size_t CLIParser::
size() const
{
	return this->_arguments.size();
}

/**
 * @brief Returns a reference to the argument given by the index.
 * @param index The index to grab the argument from.
 */
CLIArgument& CLIParser::
operator[](int index)
{
	return this->_arguments[index];
}

/**
 * @brief Returns a reference to the argument given by the index.
 * @param index The index to grab the argument from.
 */
CLIArgument CLIParser::
operator[](int index) const
{
	return this->_arguments[index];
}
