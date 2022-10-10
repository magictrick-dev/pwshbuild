/**
 * ---------------------------------------------------------------------------------------------------------------------
 * cliparser.h
 * 		Handles command line parsing.
 * ---------------------------------------------------------------------------------------------------------------------
 * pwshbuild - Developed by Christopher N. DeJong, @Github 0xDATAWOLF, June 2022
 * A project & script generator for CMake using PowerShell.
 * 
 * Version 1.2.0 - The not "janky AF" version.
 * ---------------------------------------------------------------------------------------------------------------------
 */

#ifndef PWSHBUILD_CLIPARSER_H
#define PWSHBUILD_CLIPARSER_H
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief The wrapper structure which defines a command line arguments. Allows
 * for easy access to adjacent command line arguments.
 */
class CLIArgument
{
	public:
		CLIArgument();
		CLIArgument(std::string);

		void 			set_value(std::string);
		std::string 	get_value() const;

		CLIArgument* 	next() const;
		CLIArgument* 	prev() const;

	protected:
		std::string _value;

		CLIArgument* _next_arg;
		CLIArgument* _prev_arg;

		friend class CLIParser;
		friend std::ostream& operator<<(std::ostream&, const CLIArgument&);
		friend std::ostream& operator<<(std::ostream&, const CLIArgument*);
};

/**
 * @brief The governing controller class for command line parsing. Simply takes
 * the argument count and argument vector passed into main and organizes the data
 * into an easily retrievable format.
 */
class CLIParser
{

	public:
		CLIParser();
		CLIParser(int argc, char** argv);

		void parse(int argc, char** argv);

		CLIArgument* get(std::string key);
		bool is_set(std::string key);

		size_t size() const;

		CLIArgument& 	operator[](int index);
		CLIArgument 	operator[](int index) const;

	protected:
		std::string _invocation_path;
		std::vector<CLIArgument> _arguments;

};

#endif