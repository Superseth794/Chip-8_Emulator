# Chip-8_Emulator
A Chip-8 emulator.

You can change the game run by the emulator in the emulator's configuration settings.


# How to compile and run the emulator ?
First download the project either with `git clone` or by downloading the project folder.

Then, in order to compile the project you will need a buil tool called premake.
Go to https://premake.github.io/download.html and download the correct version according to your environment.
Place the executable file at the root of the project folder.

Finnaly, the project has dependencies which you will allso need to download.
The dependency is the SFML, which you can download here https://www.sfml-dev.org/download.php. You can chose the latest version, but the project was created under version 2.5.1. Again, chose the right files according to your environment.
Place the downloaded folder at the root of the project.

Now the folder should have the following files :
  - Chip-8_Emulator/
  - Readme.md
  - projectConfig.lua
  - SFML/
  - Executable/
  - premake5 (executable)
Files extensions might differ according to your environment.

Now you can start to build the project.
You will need to open a terminal and move to the project directory to build and run.
When you are in the project directory, you first have to run premake with the following command :
  `./premake5 --file=projectConfig.lua gmake2`
This runs premake and tell it to use *projectConfig.lua* as the configuration file and to generate a GNU makefile.
Then you can use this makefile by running :
  `make all`
This runs make on the makefile and builds the project.
Finnaly, move to the directory Executable/Debug.

Here, if everything went well you should have an executable file called Chip-8.
You can run it with the following command :
  `export LD_LIBRARY_PATH=../../SFML/lib/ ; ./Chip-8`
The export part of the command is required to tell the programm where to find the SFML lib, so when you have run it once, you don't have to run it every time then.
If you get an error like
  `dyld: Library not loaded: @rpath/libsfml-graphics.2.5.dylib
   Referenced from: <path>/Executable/Debug/./Chip-8
   Reason: image not found
   [1]    27266 abort      ./Chip-8`
just run again the export part of the command (`export LD_LIBRARY_PATH=../../SFML/lib/`).


# How to change the emulator settings ?
You can change many settings in the config file *Config.txt* provided in `Executable/`.
The file is loaded at runtime so you don't have to rebuild the project when you modify it.

If you want to create another config file you can give its name (with its relative path) as an argument when you run the program.
For example, if you create another config file named *configFileBis.txt* in the *Executable/* directory, the relative path will be "../" so you can run the program with the following command `./Chip-8 ../configFileBis.txt`.
Be carefull, the program will crash if it cannot find and open the configuration file, so make sure the file can be read (in therms of accessibility rights).

The program starts reading a configuration value just after the collons *:* (spaces are ignored) so make sure not to remove them.
Config value keys are case-sensitive so you must not modify them otherwise the program will fail at finding them (and will also fail at finding the corresponding value of course).
If a value cannot be find or read, a default value will be used wich is the value of the default config file provided in the project (values are hard-written in code so you don't have to keep the original version of this file) and an error will be displayed in the console (but the program won't crash).

Finnaly, here are somme infos about the configuration values if you want to modify them :
  - `screen_width` and *screen_height* are expressed in pixels and represent the size of the game window (not the final window but the subwindow where the game is run)
  - `update_frequency` and `framerate` are in Hz and represent respectively the emulator frequency and the game window framerate (so the fps)
  - `file` is the game filename that will be run by the emulator
  - `sound` is the sound filename that will be used by the emulator
  - `font` is the font filename that will be used by the emulator
  - `show_infos`, `show_opcodes` and `show_memory` are used to control which debug parts of the emulator are displayed
  - **All filenames are relative to the executable file**
 
