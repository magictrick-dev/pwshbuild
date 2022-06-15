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

#include <stdint.h>

/**
 * Future TODOs:
 * 
 * 1. Command line arguments.
 * 		Implement command line arguments such that the generated config file is set at execution time.
 * 		This will allow the user to skip the step having to do edit it.
 * 
 */

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
 * genf(filepath, contents)
 * 
 * 		Spins up a ofstream and chucks the string contents into it and then closes it. Always truncates.
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
 * clearss(stringstream)
 * 
 * 		Yeets the contents of the stringstream.
 */
static inline void
clearss(std::stringstream& ss)
{
	ss.str("");
}

/**
 * DOutput(std::string)
 * 
 * 		Creates a simple display output object that spits a message out, followed by an elipsis, on
 * 		construction. On destruction, it adds "done!" and then ends the line. Quick and dirty verbose
 * 		output for the user.
 */
class DOutput
{
	public:
		DOutput(std::string message) { std::cout << message << "..."; };
		~DOutput() { std::cout << " done!" << std::endl; }
};

// ---------------------------------------------------------------------------------------------------
// Main Entry Point
// ---------------------------------------------------------------------------------------------------

int
main(int argc, char**argv)
{

	// Program header for the user.
	std::cout << "Pwshbuild | Created by Christopher N. DeJong, @Github 0xDATAWOLF, June 2022" << std::endl;
	std::cout << "Current pwshbuild Version: 1.0.0" << std::endl;

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



	return 0;
}
