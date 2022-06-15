# pwshbuild | C/C++ | For Visual Studio Code

For the lazy programmer, by the lazy programmer.

## Quickstart Guide

Compile the project using CMake or preferred compiler of choice and place the output executable
somewhere quiet. There are zero dependencies--only one file. Once you have the executable, map it to
your environment variables for ease-of-access so you can execute it from the command line wherever you are.

## What is pwshbuild?

I found that typing the same commands into the command line often becomes tedious,
so I wrote a series of powershell scripts to do the work for me. When typing those scripts into
the command line became equally as tedious, I created keymaps in VSCode to type them in for me.
Whenever I needed to make a new project, copying these powershell scripts over
between each project also became tedious. So I wrote a program that generates these files for me each
time I spin up a new C++ project.

## What are the requirements to use pwshbuild?

You will need to use Powershell to execute these scripts.

Currently, pwshbuild is designed to interact with CMake and Visual Studio (devenv) for debugging.

## How does it work?

Pwshbuild, generates the powershell scripts and necessary configuration file.

You will notice that these scripts are CMake focused--and that's because they are. Pwshbuild makes zero
assumptions about your CMake configuration, therefore it is your responsibility to ensure the `pwshbuild.conf`
file contains all the necessary locations to find what it needs. Don't worry, it's relatively simple to modify.
**You can also check this git repository to see the files it generates!**

## What are your VSCode keybindings?

To make full use of this utility, you will need to set these in your `keybindings.json` configuration
file in order to make full use of these scripts. It's not required that you do this, but I find that it
makes my life a little easier because I can hit a hotkey and run the scripts without switching over to a
terminal to type them out.

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
