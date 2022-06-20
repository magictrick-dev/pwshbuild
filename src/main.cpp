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

typedef int b32; // I don't like the bool type.

// ---------------------------------------------------------------------------------------------------
// Platform Abstractions
// ---------------------------------------------------------------------------------------------------

static inline std::string getApplicationPath();

/**
 * Windows specific definitions.
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>

/** Returns the executable path of the application. */
static inline std::string
getApplicationPath()
{

	// Fetch the application path using Windows.
	const DWORD stringBufferSize = MAX_PATH;
	char stringBuffer[stringBufferSize];
	GetModuleFileNameA(NULL, stringBuffer, stringBufferSize);

	// Return it as a string.
	std::string applicationPath = stringBuffer;
	return applicationPath;
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
	"Get-Content \"./pshbuild.conf\" | foreach-object -begin {$h=@{}} -process { $k = [regex]::split($_,'=');\n"
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

static void
initializeApplicationState(std::vector<argument> arguments, astate& applicationState)
{



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
	std::vector<argument> _app_args;

	// Parse out the arguments from the command line.
	parseArguments(_app_args, argc, argv);
	if (!checkArguments(_app_args, argc, argv)) return 1; // If there are errors in the arguments, fast exit.

	// Initialize the application state.
	initializeApplicationState(_app_args, _app_state);
	if (!checkArguments(_app_args, argc, argv)) return 1; // Initialization may invalidate the arguments.
	

/** For now, we don't really want to overwrite the project's own scripts.

	// Create a string stream to store the contents of a file.
	std::stringstream fstring;

	{
		// Generate the configuration file.
		DOutput dout("Generating configuration file");
		fstring << "[General]" << std::endl;
		fstring << "projectname=" << std::endl;
		fstring << std::endl;
		fstring << "[Build]" << std::endl;
		fstring << "builddir=" << std::endl;
		fstring << "exepathDebug=" << std::endl;
		genf("./pshbuild.conf", fstring.str());
		clearss(fstring);
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

*/

	return 0;
}
