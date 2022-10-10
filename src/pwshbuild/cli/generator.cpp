#include <pwshbuild/cli/generator.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

/**
 * -----------------------------------------------------------------------------
 * GeneratorObject Implementation
 * -----------------------------------------------------------------------------
 */



/**
 * -----------------------------------------------------------------------------
 * GeneratorProject Implementation
 * -----------------------------------------------------------------------------
 */

GeneratorProject::
GeneratorProject(std::string generator_filepath) :
_generator_file(std::filesystem::path(generator_filepath))
{
	return; // Do nothing.
}

GeneratorProject::
GeneratorProject(std::filesystem::path generator_filepath) :
_generator_file(generator_filepath)
{
	return; // Do nothing.
}

void GeneratorProject::
parse()
{

	// Ensure the generator file exists before proceeding with parsing.
	if (!std::filesystem::exists(this->_generator_file))
	{
		std::cerr << "    Unable to parse the generator file. File does not exist.\n";
		exit(1); // Kill the application.
	}

	// Create the file stream and make sure it is open.
	std::ifstream file_stream;
	file_stream.open(this->_generator_file);
	if (!file_stream.is_open())
	{
		std::cerr << "    Unable to open the generator file.\n";
		exit(1);
	}

	// Parse each line and determine if it is a directive.
	std::string current_line;
	int line_count = 0;
	while(std::getline(file_stream, current_line))
	{
		// Determine if the line is a directive--the directive must be at index
		// zero otherwise it is an invalid directive.
		size_t directive = current_line.find("#!");
		if (directive != std::string::npos && directive == 0)
		{
			std::cout << "Directive found at line: " << line_count << std::endl;

			// Get the root directive from the identifier "#!".
			std::string root_directive = current_line.substr(2);
			std::string directive_file_name;
			std::string directive_file_path;

			size_t split_location = root_directive.find("@");
			if (split_location == std::string::npos)
				directive_file_path = "";
			else
				directive_file_path = root_directive.substr(split_location+1);
			if (split_location == std::string::npos)
				directive_file_name = root_directive;
			else
				directive_file_name = root_directive.substr(0, split_location);

			std::cout << "Showing directive parse from line " << line_count << ":\n";
			std::cout << "Root: " << root_directive << std::endl;
			std::cout << "File name: " << directive_file_name << std::endl;
			std::cout << "File path: " << directive_file_path << std::endl;
			std::cout << "Is Directory: " <<
				((directive_file_name == "" && directive_file_path != "") ? "Yes" : "No") << std::endl;
			std::cout << "Is File: " <<
				((directive_file_name != "") ? "Yes" : "No") << std::endl;
			std::cout << "Is Empty Directive: " <<
				((root_directive == "") ? "Yes" : "No") << std::endl;
			std::cout << std::endl;
		}

		line_count++;

	}

	// Close the file stream.
	file_stream.close();

}
