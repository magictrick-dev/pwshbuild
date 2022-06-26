/**
 * pwshbuild
 * 
 * 		Creates the necessary files needed to build projects using powershell scripts.
 * 
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <utility>
#include <vector>
#include <unordered_map>

#include <assert.h>

#include <stdint.h>
typedef int32_t b32;
typedef int64_t b64;

// ---------------------------------------------------------------------------------------------------
// Platform Abstractions
// ---------------------------------------------------------------------------------------------------

static inline std::string platformGetApplicationPath();
static inline b32 platformFileExists(std::string);
static inline b32 platformCreateDirectory(std::string);

/**
 * Windows specific definitions.
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>

/**
 * Creates a directory using the given path.
 */
static inline b32 platformCreateDirectory(std::string path)
{
	BOOL _create_status = CreateDirectoryA(path.c_str(), NULL);
	return _create_status;
}

/**
 * Returns the executable path of the application.
 */
static inline std::string
platformGetApplicationPath()
{

	// Fetch the application path using Windows.
	const DWORD stringBufferSize = MAX_PATH;
	char stringBuffer[stringBufferSize];
	GetModuleFileNameA(NULL, stringBuffer, stringBufferSize);

	// Return it as a string.
	std::string applicationPath = stringBuffer;
	return applicationPath;
}

/**
 * Determines if a file exists.
 */
static inline b32
platformFileExists(std::string fpath)
{

	// Get the file attributes and use that to determine if the file exists.
	DWORD dwAttrib = GetFileAttributes(fpath.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

}

#else

static inline std::string
getApplicationPath()
{

	// Since we don't support other operating systems, we will simply throw an assertion here.
	std::string _invalid = "";
	assert(_invalid != "");
	return _valid;

}

#endif

// ---------------------------------------------------------------------------------------------------
// Script Definitions
// ---------------------------------------------------------------------------------------------------

/**
 * This little tid-bit will fetch the configuration file, "pshbuild.conf", and load it into the var "$h".
 * Not necessarily ideal, but it does allow for some customization and changes to the project without having
 * to touch every script.
 */
const char* loadconfig =
	"Get-Content \"./pwshbuild.conf\" | foreach-object -begin {$h=@{}} -process { $k = [regex]::split($_,'=');\n"
	"\tif(($k[0].CompareTo(\"\") -ne 0) -and ($k[0].StartsWith(\"[\") -ne $True)) { $h.Add($k[0], $k[1])}}";

/**
 * This is the CMake build script. It performs two operations, the first being that it configures cmake if the
 * build directory doesn't exist, then it runs the build. Two birds, one stone.
 */
const char* buildscript =
	"if (!(Test-Path -Path $h.builddir)) { ./config_cmake.ps1 }\n"
	"cmake --build $h.builddir";

/**
 * The clean script. It deletes the ./build directory. Since the command will spew to standard error,
 * we will catch that and forward it to null since we don't actually care if it succeeded or not.
 */
const char* cleanscript =
	"Remove-Item $h.builddir -Recurse -Force 2> $null";

/**
 * Simply runs the cmake configuration step and sets the build output directory to "./build".
 */
const char* configscript =
	"cmake . -B $h.builddir";

/**
 * The debug script will launch Visual Studio with the executable attached. Not designed for editing,
 * only for debugging.
 */
const char* debugscript =
	"devenv $h.exepathDebug";

/**
 * The legendary run script. As you might imagine, it launches the application.
 */
const char* runscript =
	"Invoke-Expression $h.exepathDebug";

/**
 * The cmakelists.txt file that is generated when invoking --create-project. It will set the
 * executable name within that cmake lists file.
 */
const char* cmakelists =
	"\n"
	"cmake_minimum_required(VERSION 3.10)\n"
	"\n"
	"set(CMAKE_CXX_STANDARD 17)\n"
	"set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)\n";

/**
 * A simple hello world file.
 */
const char* mainsource =
	"/**\n"
	" * A simple hello world file.\n"
	" */\n"
	"#include <iostream>\n\n"
	"int main(int argc, char** argv)\n"
	"{\n"
	"\tstd::cout << \"Hello world\" << std::endl;\n"
	"}\n\n\n";

// ---------------------------------------------------------------------------------------------------
// Utility Functions & Classes
// ---------------------------------------------------------------------------------------------------

/**
 * The primary structure that controls defaults of the application.
 */
typedef struct astate
{

	std::string buildpath; 		// pwshbuild config parameter
	std::string debugpath; 		// pwshbuild config parameter

	std::string projectname;	// Sets the project name and a default executable name.

	b32 overwrite_config; 		// Determines if we overwrite an existing pwshbuild.conf file.
	b32 help_dialogue; 			// Determines if the app should display the help instead of running.

} astate;

/**
 * 	Spins up a ofstream and chucks the string contents into it and then closes it. Always truncates.
 */
static inline void
genf(std::string fpath, std::string contents)
{
	std::ofstream scriptf(fpath, std::ios_base::trunc);
	scriptf << contents;
	scriptf.close();
	return;
}

/**
 * 	Yeets the contents of the stringstream.
 */
static inline void
clearss(std::stringstream& ss)
{
	ss.str("");
}

/**
 * 	Creates a simple display output object that spits a message out, followed by an elipsis, on
 * 	construction. On destruction, it adds "done!" and then ends the line. Quick and dirty verbose
 * 	output for the user.
 */
class DOutput
{
	public:
		DOutput(std::string message) { std::cout << message << "..."; };
		~DOutput() { std::cout << " done!" << std::endl; }
};

static inline void
printHelpEntry(std::string command, std::string desc)
{
	std::cout << std::setw(32) << std::left << command << std::setw(128) << std::left << desc << std::endl;
}

// ---------------------------------------------------------------------------------------------------
// Argument Parsing Framework
// ---------------------------------------------------------------------------------------------------

/**
 * Determine what type of argument flag type it is. Singles imply that there is no next component to
 * the argument. Doubles imply that the following component is part of the flag.
 */
enum argflagtype
{
	invalid,
	single,
	pair,
};

/**
 * Contains the information for each parsed argument.
 */
typedef struct argument
{
	std::string head; 		// The head is the command flag.
	std::string tail;		// The value is the contents of the flag.
	argflagtype type;
	int cdepth;				// Argc index. Used for showing errors to the user.
} argument;

/**
 * Returns a validated argument struct using a given index of the known list within argv.
 */
static inline argument
validateArgument(int& carg, const int argc, char** argv)
{

	argument _a = {};
	_a.cdepth = carg;

	// Keep an unordered map of the various argument tokens.
	static std::unordered_map<std::string, int> valid_tokens =
	{
		{"--help", argflagtype::single},
		{"--overwrite-config", argflagtype::single},
		{"--set-buildpath", argflagtype::pair},
		{"--set-debugpath", argflagtype::pair},
		{"--create-project", argflagtype::pair}
	};

	// If the key doesn't exist, it is an invalid key.
	if (valid_tokens.find(argv[carg]) == valid_tokens.end())
	{
		_a.type = argflagtype::invalid;
		_a.head = std::string(argv[carg]);

		std::string errorMessage = "Unrecognized parameter: \"" + std::string(argv[carg]) + "\"";
		_a.tail = errorMessage;
	}

	// We know it exists, so build up _a.
	else
	{
		_a.type = (argflagtype)valid_tokens[argv[carg]];
		_a.head = std::string(argv[carg]);
		_a.tail = "";
	}

	// If it is a pair, we will need to validate that.
	if (_a.type == argflagtype::pair)
	{

		// If there are no proceeding elements, it is invalid.
		if (carg+1 >= argc) 
		{
			_a.type = argflagtype::invalid;
			_a.tail = "You need to provide an argument for this parameter.";
		}
		
		// If the proceeding argument is another token, it is invalid.
		
		else if (valid_tokens.find(argv[carg+1]) != valid_tokens.end()) 
		{
			_a.type = argflagtype::invalid;
			_a.tail = "You need to provide an argument for this parameter.";
		}

		// Otherwise, we will assume that the proceeding token is a valid token.
		else { _a.tail = std::string(argv[++carg]); }

	}

	return _a;

}

/**
 * Parses out the arguments sent in through the command line and stores them in the supplied vector.
 */
static void
parseArguments(std::vector<argument>& argument_list, int argc, char** argv)
{

	// The first argument is guaranteed to be there on application startup.
	argument_list.push_back({argv[0], "", argflagtype::single, 0});

	// For any additional arguments, we will parse them out as they come along. See validateArgument()
	// for the validation routine.
	for (int carg = 1; carg < argc; ++carg)
		argument_list.push_back(validateArgument(carg, argc, argv));

}

/**
 * Searches for errors in the parsed arguments and shows the user the errors in their ways. Returns true
 * if there were errors to output, false if not.
 */
static bool
checkArguments(std::vector<argument>& argument_list, int argc, char** argv)
{

	bool _contains_errors = false;

	// Create a string that contains the complete command list. This will be used to spit out errors to the user.
	std::string clist = "pwshbuild ";
	const int _command_name_length = (int)clist.length();
	for (int i = 1; i < argc; ++i) clist += (std::string(argv[i]) + " ");

	// Go through all the passed in arguments and find the ones that aren't validated.
	for (argument& carg : argument_list)
	{
		if (carg.type == argflagtype::invalid)
		{

			_contains_errors = true;

			// Calculate the number of spaces we need to reach the argument that contained the error.
			int _space_length = _command_name_length;
			for (int i = 1; i < carg.cdepth; ++i) _space_length += ((int)strlen(argv[i]) + 1);

			// Calculates how many spaces, how many carrots, and prints the error out to the user. + _command_name_length;
			std::cout << "Error: " << carg.tail << " Use --help for additional information. See below for error:" << std::endl;
			std::cout << clist << std::endl;
			for (int s = 0; s < _space_length; ++s) std::cout << " ";
			for (int d = 0; d < strlen(argv[carg.cdepth]); ++d) std::cout << "^";
			std::cout << std::endl << std::endl;
		}
	}

	return _contains_errors;

}

/**
 * Initializes the application state using the arguments provided by the user.
 */
static void
initializeApplicationState(std::vector<argument> arguments, astate& applicationState)
{

	// Process the arguments.
	for (argument& carg : arguments)
	{

		if (carg.head == "--help") 
		{
			applicationState.help_dialogue = true;
		}

		else if (carg.head == "--set-buildpath") 
		{
			applicationState.buildpath = carg.tail;
		}

		else if (carg.head == "--set-debugpath")
		{
			applicationState.debugpath = carg.tail;
		}

		else if (carg.head == "--overwrite-config")
		{
			applicationState.overwrite_config = true;
		}

		else if (carg.head == "--create-project")
		{
			applicationState.projectname = carg.tail;
		}

	}

}

// ---------------------------------------------------------------------------------------------------
// Main Entry Point
// ---------------------------------------------------------------------------------------------------

int
main(int argc, char** argv)
{

	// Program header for the user.
	std::cout << "pwshbuild | Created by Christopher N. DeJong, @Github 0xDATAWOLF, June 2022" << std::endl;
	std::cout << "Current pwshbuild Version: 1.1.0" << std::endl << std::endl;

	// Application runtime variables & structures.
	astate _app_state = {};
	_app_state.buildpath = "";
	_app_state.debugpath = "";

	// Parse out the arguments from the command line.
	std::vector<argument> _app_args;
	parseArguments(_app_args, argc, argv);
	if (checkArguments(_app_args, argc, argv)) return 1; // If there are errors in the arguments, fast exit.

	// Initialize the application state.
	initializeApplicationState(_app_args, _app_state);
	if (checkArguments(_app_args, argc, argv)) return 1; // Initialization may invalidate the arguments.

	// Finally, if the help dialogue was issued in the command arguments, override standard behavior and
	// print everything out.
	if (_app_state.help_dialogue)
	{
		std::cout << "You can use pwshbuild without any arguments. It will generate all the scripts with a\n"
					 "default configuration that you can edit with the desired values as needed. Otherwise,\n"
					 "the list of commands below will allow you to quickly pass in the known configuration\n"
					 "entries using the various parameters below. Certain parameters, when passed, require\n"
					 "that the proceeding argument must be present otherwise it is considered invalid.\n\n";

		printHelpEntry("--help", "You are here! Prints the help dialogue.");
		std::cout << std::endl;

		printHelpEntry("--overwrite-config", "Allows pwshbuild to overwrite pwshbuild.conf if one is present.");
		std::cout << std::endl;

		printHelpEntry("--set-buildpath", "Sets the buildpath entry in pwshbuild.conf for you using the following argument.");
		printHelpEntry("", "Please note that this will not mean the path is valid, only that the path supplied is set in the config.");
		printHelpEntry("", "Example: pwshbuild --set-buildpath ./build");
		std::cout << std::endl;

		printHelpEntry("--set-debugpath", "Sets the debugpath entry in pwshbuild.conf for you using the following argument.");
		printHelpEntry("", "Please note that this will not mean the path is valid, only that the path supplied is set in the config.");
		printHelpEntry("", "Example: pwhsbuild --set-debugpath ./build/bin/Debug/pwshbuild.exe");
		std::cout << std::endl;

		printHelpEntry("--create-project", "Creates a project and a simple executable. Creates CMAKELISTS and main.cpp.");
		printHelpEntry("", "Example: pwhsbuild --create-project pwshbuild");
		std::cout << std::endl;

		return 0; // Exit application.
	}

	// Create a string stream to store the contents of a file.
	std::stringstream fstring;

	{
		if (!platformFileExists("./CMAKELISTS.txt") && _app_state.projectname != "")
		{
			{
				DOutput dout("Generating a CMake project");
				fstring << cmakelists;
				fstring << "project(" << _app_state.projectname << ")\n";
				fstring << "add_executable(" << _app_state.projectname << " ./src/main.cpp)\n";
				fstring << "set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT";
				fstring << " " << _app_state.projectname << ")\n";
				genf("./CMAKELISTS.txt", fstring.str());
				clearss(fstring);
			}
		}
		if (!platformFileExists("./src/main.cpp") && _app_state.projectname != "")
		{
			{
				DOutput dout("Generating a main source file");
				platformCreateDirectory("./src");
				fstring << mainsource;
				genf("./src/main.cpp", fstring.str());
				clearss(fstring);
			}
		}
	}

	{

		if (!platformFileExists("./pwshbuild.conf") || _app_state.overwrite_config)
		{
			// Generate the configuration file.
			DOutput dout("Generating configuration file");
			fstring << "[General]" << std::endl;
			fstring << "projectname=" << std::endl;
			fstring << std::endl;
			fstring << "[Build]" << std::endl;
			fstring << "builddir=" << _app_state.buildpath << std::endl;
			fstring << "exepathDebug=" << _app_state.debugpath << std::endl;
			genf("./pwshbuild.conf", fstring.str());
			clearss(fstring);
		}
		else
		{
			DOutput dout("Skipping configuration file (use --overwrite-config to override this behavior)");
		}
	}

	{
		// Generate the build script.
		DOutput dout("Generating build script");
		fstring << loadconfig << "\n" << buildscript << "\n";
		genf("./build.ps1",  fstring.str());
		clearss(fstring);
	}

	{
		// Generate the clean script.
		DOutput dout("Generating clean script");
		fstring << loadconfig << "\n" << cleanscript << "\n";
		genf("./clean.ps1", fstring.str());
		clearss(fstring);
	}

	{
		// Generate the run script.
		DOutput dout("Generating run script");
		fstring << loadconfig << "\n" << runscript << "\n";
		genf("run.ps1", fstring.str());
		clearss(fstring);
	}

	{
		// Generate config script.
		DOutput dout("Generating config script");
		fstring << loadconfig << "\n" << configscript << "\n";
		genf("./config_cmake.ps1", fstring.str());
		clearss(fstring);
	}

	{
		// Generate the debug script.
		DOutput dout("Generating debug script");
		fstring << loadconfig << "\n" << debugscript << "\n";
		genf("./debug.ps1", fstring.str());
		clearss(fstring);
	}

	return 0;
}
