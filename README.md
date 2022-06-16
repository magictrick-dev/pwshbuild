<img src="md/logo.svg" width="400px">

For the lazy programmer, by the lazy programmer.

# Quickstart Guide

Clone the repository `git clone https://github.com/0xDATAWOLF/pwshbuild.git` and run CMake to configure
the build files. Alternatively, you can also compile `main.cpp` directly using your compiler of choice.
Once you have the executable, move it off to a good location such that you can map that to your environment
variables. After all that is completed, you are free to execute `pwshbuild` at the root of your project
directory to generate the PowerShell scripts.

A configuration file will also be generated. Supply `pwshbuild.conf` with the build directory and the
executable path. The scripts will use this configuration file to execute their respective routines.

# What is pwshbuild?

I wrote pwshbuild to solve a series of very trivial problems that don't normally need solving. Rather
than typing out a series of commands in the terminal, I wrote a series of scripts to do the work for me.
Once I got tired of typing the scripts in the terminal, I created keybinding shortcuts in Visual Studio
Code to run them for me. The final tedium of this whole process involved copying these scripts over
between projects. So I wrote **pwshbuild** to do all this work for me.

* <h3> What <strong>pwshbuild</strong> does by default: </h3>

	**pwhsbuild** generates a series of scripts to interact with CMake. The first script, `config_cmake.ps1`
	configures CMake and uses `pwshbuild.conf` to determine what path it should use for the build directory.
	For most purposes, setting `builddir=./build` in `pwshbuild.conf` should suffice for most use cases.
	The second script, `build.ps1` works in tandem with `config_cmake.ps1`. Since it does not make sense to
	build a project without first running the CMake configuration, `build.ps1` will first run `config_cmake.ps1`
	should the build directory not exist. Once complete, it will then build the project.

	Additionally, you will find `clean.ps1` useful for deleting the build directory. I do not map this script
	to any keys and I only use this script if I change the CMake build configuration for a particular project
	or if I need to reconfigure the project anew.

	The last two scripts are related to debugging. You will find `debug.ps1` to be useful if you use Visual
	Studio for debugging your applications. This will invoke `devenv` on the executable path set in
	`pwshbuild.conf`. Depending on how the CMakeLists is set up, the executable path may be different. For
	example, my typical path looks something like this: `exepathDebug=./build/bin/Debug/pwshbuild.exe`. In
	order for `devenv` to be invoked by the command line, you will need to execute `vsvars64.bat` on terminal
	startup. You can find [more information here](https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_profiles?view=powershell-7.2) if this something you're interested in.
	You can create a profile script that is ran when PowerShell starts so that you have access to `devenv`.
	The other script, `run.ps1`, runs the program using the `exepathDebug` entry in `pwshbuild.conf` as
	described earlier.

* <h3> Can <strong>pwshbuild</strong> do more? </h3>

	You can, of course, modify the scripts as you like. I find that this is something I do with every project
	as it matures. You will find the code for loading `pwshbuild.conf` at the top of every script. All it does
	is parse out the contents of the file and maps it into a variable `$h`. You can add additional entries into
	`pwshbuild.conf` without consequence.

	The idea behind `pwshbuild.conf` is to avoid having to modify each script individually if you make one change
	that affects more than one file.

* <h3> Extensibility of the <strong>pwshbuild</strong> source code: </h3>

	The source code is some of the most trivial code I have ever written, yet it is something that I am quite proud of.
	The nice part is that it is geniuinly quite easy to understand. If you want to modify the scripts that it generates,
	they are all right there, in the source code. Nothing weird or strange going on.

# What are your VSCode keybindings?

Below are the keybindings I use to execute the scripts while using Visual Studio Code. You can change the
keys and behaviors as you see fit to best match what you're comfortable with.

```JSON
{
	"key": "f5",
	"command": "workbench.action.terminal.sendSequence",
	"args": {
		"text": "./run.ps1\u000D"
	}
},
{
	"key": "f9",
	"command": "workbench.action.terminal.sendSequence",
	"args": {
		"text": "./debug.ps1\u000D"
	}
},
{
	"key": "ctrl+b",
	"command": "workbench.action.terminal.sendSequence",
	"args": {
		"text": "./build.ps1\u000D"
	}
}
```
