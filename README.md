<img src="md/logo.svg" width="400px">

For the lazy programmer, by the lazy programmer.

## Quickstart Guide

Clone the repository `git clone https://github.com/0xDATAWOLF/pwshbuild.git` to your location of choice
and then run CMake to create the build files. Compile the project and move the executable somewhere quiet
such that you can map it to your environment variables to invoke the program from your terminal of choice.
This project doesn't require any dependencies and can be compiled using any compiler of your choice.

Supply `pwshbuild.conf` with the build directory and executable path and the scripts will take care of the
rest. Once done, the scripts are ready to go!

## What is pwshbuild?

I find myself typing the same commands over and over again to be quite tedious, so I wrote a series of
PowerShell scripts to take care of the work for me. When typing those scripts into the command line over
and over again, it too became a tedious chore. So I created keymaps in Visual Studio Code to push these
commands off to the terminal for me. However, whenever I started a new project, I ended needing to copy
over these scripts from one project to another. That too became tedious.

Pwshbuild is my attempt at solving a series of trivial problems: creating scripts, running scripts, and
copying scripts. This project is very simplistic and it is reflected within the source code. It's not
meant to do anything fancy, only enough to make my life simpler. If this is something that resonates with
you, please feel free to use, fork, or contribute to the project. 

## What are the requirements to use pwshbuild?

You will need to use Powershell to execute these scripts.

Currently, pwshbuild is designed to interact with CMake and Visual Studio (devenv) for debugging.

## How does it work?

Pwshbuild generates the powershell scripts and the necessary configuration file in the directory it is
called in. You can run these scripts manually by invoking them or by making some changes to
Visual Studio Code's keybinds to run them for you.

## What are your VSCode keybindings?

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
