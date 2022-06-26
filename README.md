<img src="md/pwshbuild.svg" width="400px">

Tired of typing CMake commands to configure and build your projects? Don't feel like typing out lengthy paths
just to execute your debug build located three folders deep? Need to clean out your build directory files because
CMake isn't happy with some minor change in configuration? Me too. I wrote a series of PowerShell scripts
to solve these minor inconveniences using clever key remaps in Visual Studio Code. Now you can too! With
pwshbuild, you can generate these scripts on the fly and configure them for your particular environment
using a handy config file in a single command. The scripts are designed to be extensible after generation,
meaning that you can tailor them to do anything you want (after all, that's the point of a script).

**Pwshbuild is designed for C/C++ CMake projects developed in VS Code which use Visual Studio for debugging.**

# Quickstart Guide

There are currently no available binaries for pwshbuild, so you will need to compile pwshbuild from source:

1. Clone the repository to any directory of your choice.

	``` git clone https://github.com/0xDATAWOLF/pwshbuild.git ```

2. Configure the project using CMake.

	``` cmake . -B ./build ```

3. Build the project.

	``` cmake --build ./build ```

4. Copy the executable from the build binary directory. The relative directory is shown:

	``` ./build/bin/Debug/pwshbuild.exe ```

5. Paste the executable somewhere that you can add it to your environment variable PATHs. An example is shown:

	``` C:\Program Files (Custom)\binaries\pwshbuild.exe ```

6. Add pwshbuild to your environment variables PATHs.

You are now ready to use pwshbuild! Enter the root of your project directory, execute `pwshbuild`, and it should
drop all the scripts with the configuration file right there for you! All that's left is to edit `pwshbuild.conf`
with the necessary information, and you are ready to go.

# Pwshbuild Documentation

### Commandline Flags

Pwshbuild comes with a variety of CLI parameter flags that you can use:

<table width="100%">
	<tr>
		<th width="20%">Command Parameter</th>
		<th>Description</th>
	</tr>
	<tr>
		<td>--help</td>
		<td>Displays the help dialogue.</td>
	</tr>
	<tr>
		<td>--create-project</td>
		<td>
			Creates a project using the supplied name. Generates a CMAKELISTS and a simple main.cpp file.
			Example: <code>pwshbuild --create-project pwshbuild</code>
		</td>
	</tr>
	<tr>
		<td>--set-buildpath</td>
		<td>
			Sets the builddir in pwshbuild.conf for you using the following argument that proceeds the parameter.</br>
			Example: <code>pwshbuild --set-buildpath ./build</code>
		</td>
	</tr>
	<tr>
		<td>--set-debugpath</td>
		<td>
			Sets the exepathDebug in pwshbuild.conf for you using the following argument that proceeds the parameter.</br>
			Example: <code>pwshbuild --set-debugpath ./build/bin/Debug/pwshbuild.exe</code>
		</td>
	</tr>
	<tr>
		<td>--overwrite-config</td>
		<td>
			By default, if there is an existing <code>pwshbuild.conf</code> within the calling directory, pwshbuild will
			not overwrite it. This flag will force pwshbuild to overwrite the config file when passed in.
		</td>
	</tr>
</table>

### Modifying the Scripts

The scripts that are generated are defined within the source. You can modify the source and recompile the source to change
the default scripts that pwshbuild generates. Additionally, the scripts themselves are quite easy to modify once they are
generated. The config file is modifiable without consequence, simply add entries and headers as you like and each script
will have access to them. At the head of each script is the PowerShell code which parses out the file. The value of each
entry within a properly defined config file is stored with the PowerShell variable `$h`. For the most part, the base
behavior of these scripts simple reuse the common build directory and debug executable path, but you can add whatever you
want to extend the behavior of these scripts.

# VSCode Keyboard Shortcuts

Below are the keybindings I use to execute the scripts while using Visual Studio Code. You can change the
keys and behaviors as you see fit to best match what you're comfortable with. To access the JSON file for
VS Code's keybindings:

``` File > Preferences > Keyboard Shortcuts ```

Switch to the JSON editor using the top right icon within the interface and paste in the following commands:

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
