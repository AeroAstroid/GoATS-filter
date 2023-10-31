# Gadget of Accessible Tourney Seedfinding (GoATS)

**GoATS** is a customizable Minecraft seedfinding program written mostly in C. It features powerful and efficient filtering tools for many different Minecraft versions, primarily suited for speedrunners and speedrun tournament organizers, available in an easily configurable and accessible manner.

This is primarily a wrapper on the Cubiomes C library, and as such, implements many of its basic features, such as simple filtering for structures like villages, fortresses, bastions, strongholds, etc. However, **GoATS** also includes many novel features such as lava pools, magma ravines, end cages, zero cycle towers, and even customizable loot-based filters for many structures (buried treasure, shipwreck, etc). A comprehensive list of its features can be found further below.

### Beta Disclaimer

_**GoATS is currently in BETA!** The code is still incomplete and pending optimizations, features, and full functionality._
As of right now, all included filters *should* work as intended for 1.16+, with at least partial functionality for most filters in 1.15-.

## Installation

*[Note: this installation tutorial is a work in progress, and a more in-depth video tutorial is planned for the official release.]*

*[Note: no installation method that works on Mac has been investigated yet.]*

Download the .zip file of this repository's source code in the Releases tab, or by pressing `Code` > `Download ZIP` in the repository's front page, and extract it wherever you'd like.

**Running GoATS requires a Linux terminal.** If you're on Linux, you're already set and can skip this step. If you're on Windows, you can download one by following the first half of [this tutorial](https://www.youtube.com/watch?v=Gl7zOn2lLo4).

Once you're done, open the Linux terminal in the folder you've extracted **GoATS** into.
- On Linux, you can right click on the folder and select `Terminal`.
- On Windows, hold shift and right click inside the folder and select `Open Linux shell here`.

To install all the dependencies **GoATS** needs to run, you can use this command in the terminal:

```sudo apt install gcc python python-is-python3 openjdk-17-jre-headless make```

Note that if you're running Linux, you might need to use a different package manager depending on your distro (e.g. `pacman`, `flatpak`, etc).

After installing those packages, you can now start using **GoATS**.

## Usage

Before jumping in, make sure again that your terminal has been opened in the same folder you've extracted **GoATS** into. Compile the seedfinding code into an executable by running the command:

```make compile```

You will only need to do this again whenever **GoATS** receives an update - customizing the filter and changing the parameters does not require recompilation.

To customize the filter parameters to your liking, open the `config.py` script in a code editor of your choice, and edit the variables accordingly. Make sure to read the instructions for each one!

Once you're done editing the file, use the command:

```make py```

This will save the filter configuration to `filter.txt`, which gets read by the seedfinding program itself. It will also save the corresponding filter code to the filter history file (`data/filterhistory.txt`), so that you may access it in the future if needed. If you want to skip being asked to name the filter, you can use the command:

```make pyn```

This command does the same thing as the previous one, except it saves the filter automatically under a blank name without prompting the user.

From there, you can automatically filter for the things you asked for by using the following command to run the executable:

```make run```

This will print a list of every matching seed the program encounters, along with some relevant statistics at the end. If you wish to output the results into a file instead of printing them, you can do:

```make run > examplefile.txt```

To filter for something else, change the `config.py` file accordingly, run the Python script again using `make py` to save the new filter, and then just use `make run` again - no recompilation is required for changes to the filter.

### Multi-threading

**GoATS** comes with CPU multi-threading support. You can run the filter with a custom number of threads by running the command:

```make run n```

...where `n` is the number of threads to be used. By default, omitting this parameter makes **GoATS** run on 1 thread. It is not recommended to go above your CPU's own thread count.

## Features

The following is a list of everything you can choose to filter for using **GoATS**. This information is also available in the comments for `config.py`.

- **Shipwreck**: position, type (front/back/full), loot, quadrant
- **Ruined portal**: position, type (with lava/without lava), loot
- **Buried treasure**: position, quantity, loot, quadrant
- **Village**: position, biome, quadrant
- **Desert temple**: position, loot, quadrant
- **Jungle temple**: position, quadrant
- **Potential lava pool**: position, quantity, quadrant
- **Magma ravine**: position, quadrant
- **Bastion**: position, type, quadrant
- **Fortress**: position, double spawner, quadrant
- **Stronghold**: distance from origin, 150 150 blind distance, ocean-exposed
- **End spawn**: exposed end spawn, endstone cage height
- **Zero cycle**: whether front/back dragon, diagonal node tower height
- **Spawn point**: position, accuracy

### Additional Scripts

This program also comes with some extra features in the form of external scripts.

#### `make lavachecker`

- Reads through the `data/seedinfo.txt` file and checks every potential lava pool in each seed, using Minecraft's own world-gen code to determine which ones actually successfuly generate
- Outputs all seeds with successfully generating lava pools in `data/lavachecker_seedinfo.txt`
- 100% accurate for 1.16+, but takes around 2 seconds per seed

## Acknowledgements

This program is meant as a free-to-use tool for *anyone* interested in seedfinding!

If you're a speedrun tournament organizer or similar, and plan to use **GoATS** to aid in seedfinding for a tourney or any other project, you are more than welcome to go ahead, no prior permission or anything needed! (Although it'd be cool if you at least told me you're using it, mainly for the validation that people are actually using the stuff I've made! 😅)

If you have questions, suggestions, complaints, or anything of the sort, feel free to send me a DM over at `aeroastroid` on Discord.

Special thanks go out to:
- **Al3xanDE_17**, for inspiring me to dive into Minecraft tournament seedfinding and to create this project
- **AndyNovo**, for creating the FSG filters, which were an extremely helpful reference for the program's logic as well as many features
- **Cubitect**, for developing and open-sourcing [Cubiomes](https://github.com/cubitect/cubiomes), the primary technical backbone for C seedfinding in general
- **JelleJurre**, for making [seed-checker](https://github.com/jellejurre/seed-checker), which was used for the lava pool checking command, as well as making the FSG (+ WSL) video tutorial linked above
