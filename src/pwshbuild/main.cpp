/**
 * ---------------------------------------------------------------------------------------------------------------------
 * main.cpp
 * 		The main entry point for the application.
 * ---------------------------------------------------------------------------------------------------------------------
 * 
 * pwshbuild - Developed by Christopher N. DeJong, @Github 0xDATAWOLF, June 2022
 * A project & script generator for CMake using PowerShell.
 * 
 * Version 1.2.0 - The not "janky AF" version.
 * 
 * ---------------------------------------------------------------------------------------------------------------------
 * 
 * -----------------------------------------------------------------------------
 * Command-line Documentation
 * -----------------------------------------------------------------------------
 * 
 * Here is the basic "how-to" for pwshbuild. Naturally, as a CLI application,
 * pwshbuild uses a series of command line switches and arguments to function.
 * Below is a complete list of pwshbuild's CLI:
 * 
 * pwshbuild -h,--help
 * 		Displays the help dialogue.
 * 
 * pwshbuild --create-project (project_name:required) (platform:default=CLI)[cli,win32]
 * 		The initial project generation command. Used for generating projects.
 * 
 * 		project_name
 * 			The name of the CMake project.
 * 
 * 		platform
 * 			The platform to which the CMake project is defined for. If one is not
 * 			provided at the time of generation, the default is a command line application.
 * 
 * pwshbuild -[cbrde] (directory:default=cwd)
 * 		The basic command structure for operating pwshbuild. Operations can be supplied
 * 		in any order, but will always run in a determined order:
 * 		
 * 		0  : -h, Displays the help dialogue. All other options are ignored.
 * 		1  : -r, Cleans the project before configuring.
 * 		10 : -c, Configures the project.
 * 		20 : -b, Builds the project.
 * 		25 : -e, Runs the executable.
 * 		30 : -d, Debugs the project.
 * 
 * 		pwshbuild -c (directory:default=cwd)
 * 			Configures the CMake project that resides within the current working directory or
 * 			using the provided directory path.
 * 
 * 		pwshbuild -b (directory:default=cwd)
 * 			Builds the CMake project that resides within the current working directory or using
 * 			the provided directory path.
 * 
 * 		pwshbuild -r (directory:default=cwd)
 * 			Cleans the project by removing the build directory.
 * 
 * 		pwshbuild -d (directory:default=cwd)
 * 			Launches the debugger.
 * 
 * -----------------------------------------------------------------------------
 * Generator Documentation
 * -----------------------------------------------------------------------------
 * 
 * In order for pwshbuild to generate projects, it needs a project template in order
 * to make files. The generator file should be specified in the executable's directory
 * such that pwshbuild can find it.
 * 
 * Each file that pwshbuild generates is designated with:
 * 
 * #!filename.ext@./some/rel/dir
 * 
 * The first portion, "#!" tells pwshbuild that the line is a preprocessor section and
 * that it should generate something. The "filename.ext" tells pwshbuild that it should create a file.
 * Any text data that proceeds this will inserted into that file until pwshbuild encounters another
 * "#!" directive.
 * 
 * A file name is not required and empty directories can be created simply by ignoring
 * this section: "#!@./some/rel/dir". Any text data between this and the next directive
 * is ignored.
 * 
 * Additionally, a path can also be omitted, indicating that the file should be created
 * at the root level directory: "#!filename.ext".
 * 
 * Finally, an empty directive does a full stop on text processing: "#!". While not required
 * as the generator will perform a stop on the next directive, it is much more explicit.
 * 
 * Tokens can be passed in the command line which can then be inserted into the scripts.
 * Within the generator script, a token can be in-lined with the "#$0$", "#$1$", ... "#$N$"
 * format. A numeric index refers to the CLI value passed in.
 * 
 * The command line format for this is:
 * pwshbuild --create-project testproj token1 token2 token3 ... tokenN
 * 
 * Tokens can also be given a key:
 * pwshbuild --create-project testproj key1=value1 token2 key2=value2 token3 ... tokenN
 * 
 * For tokens which require extended string data,
 * the token should be wrapped quotes:
 * pwshbuild --create-project testproj "extkey1=extvalue1" token2 key2=value2 ... tokenN
 * 
 * Tokens are not validated and the numeric indexes will not error when out of bounds.
 * Instead, pwshbuild will silently output a warning that a certain referenced token
 * has been used and will output useful information to the user.
 * 
 * ---------------------------------------------------------------------------------------------------------------------
 */

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#include <pwshbuild/utils/primitives.h>
#include <pwshbuild/cli/cliparser.h>
#include <pwshbuild/cli/generator.h>

void show_program_header()
{
	std::string header = "Pwshbuild - Project Management Tool version 1.1.0A";
	std::string author = "Created by Christopher N. DeJong @Github 0xDATAWOLF";
	std::cout << header << "\n" << author << "\n\n";
}

/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Main Entry Point
 * ---------------------------------------------------------------------------------------------------------------------
 */

int
main(int argc, char** argv)
{

	CLIParser cparser(argc, argv);

	// Dump the arguments to the command line.
	//std::cout << "Arguments:\n";
	//for (int i = 0; i < cparser.size(); ++i)
	//	std::cout << cparser[i] << std::endl;
	//std::cout << "\n\n";

	// Okay, now to generate a project.
	if (cparser.is_set("create-project"))
	{

		show_program_header();

		if (cparser.get("create-project")->next() == nullptr)
		{
			std::cerr << "    A project name must be supplied in order to generate a project.\n";
			std::cerr << "    See pwshbuild --help for more information.\n";
			exit(1);
		}

		std::cout << "    Creating a new project: " << cparser.get("create-project")->next() << std::endl;

		// Check if the generator file exists.
		std::filesystem::path generator_path = "./generator.pwsh";
		if (!std::filesystem::exists(generator_path))
		{
			std::cerr << "    A generator file must be defined in order to create a new project.\n";
			std::cerr << "    Create a generator file in the root directory of your project or\n";
			std::cerr << "    supply one in the same directory as pwshbuild.exe\n";
			std::cerr << "    See pwshbuild --help for more information.\n";
			exit(1);
		}

		GeneratorProject pgenerator = generator_path;
		pgenerator.parse();
	}

	return 0;

}








