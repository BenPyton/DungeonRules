# Dungeon Rules Plugin

![Plugin version number](https://img.shields.io/badge/Version-v0.1.0-orange)
[![ProceduralDungeon version number](https://img.shields.io/badge/Procedural_Dungeon-v3.3+-blue)][DEPENDANCY]
[![Unreal Engine Supported Versions](https://img.shields.io/badge/Unreal_Engine-5.3+%3F-9455CE?logo=unrealengine)](https://github.com/BenPyton/DungeonRules/releases)
[![License](https://img.shields.io/github/license/BenPyton/DungeonRules?label=License&color=blue)](LICENSE)
[![Active Development](https://img.shields.io/badge/Maintenance%20Level-Actively%20Developed-brightgreen.svg)](https://gist.github.com/cheerfulstoic/d107229326a01ff0f333a1d3476e068d)
[![Discord](https://img.shields.io/discord/1182461404092055574?logo=discord&logoColor=white&label=Discord&color=%235865F2)][DISCORD]

${\color{orange}\boxed{DISCLAIMER}}$ This plugin is currently experimental and not production ready yet. Use it only for testing purpose, if you want to help me improve it (either with bug reports/feature suggestions, or by contributing) or if you are ready to modify the C++ code to your needs.

## Overview

This Unreal Engine 5 plugin extends my other plugin [Procedural Dungeon][DEPENDANCY] to add a state machine-like asset and graph editor, easing the creation of dungeon rules.

Each rule state have a `Room Chooser` that will return which room data to add to the dungeon.\
After a room data has been added, the dungeon generator will try to go to other linked states, depending on their `Transition Conditions`.
If no condition is fulfilled, it will stay in the same state.\
The generation will stop if no door remains unconnected, or if the state `Stop` is reached.

If you have any bug or crash, please open an issue in the Github repo.\
If you have suggestions, questions or need help to use the plugin you can join the [Discord server][DISCORD].\
If you want to contribute, feel free to create a pull request.

## Features

- New `Dungeon Rules` data asset with its own graph editor.
- New `Dungeon Generator` subclass using the added data asset.
- Create your own custom `Room Chooser` and `Transition Condition` classes.
- Generate various dungeons by just changing the `Dungeon Rules` asset in your generator.

## How to use it

### The Dungeon Rules
Create a new `Dungeon Rules` data asset with `right-click` &rarr; `Procedural Dungeon` &rarr; `Dungeon Rules`.\
Double-click on your new asset to open the graph editor.\
Place new states with right-click and drag link between states to create transitions.\
Create new child blueprints from classes `Room Chooser` and `Transition Condition` and implement their overridable functions to fit your project needs.\
In your `Dungeon Rules` asset, fill your states and transitions with your new classes.

### The Dungeon Generator
Create a new actor blueprint deriving from `Dungeon Generator With Rules`.\
Fill the `Dungeon Rules` variable with your new asset.\
Override the [Is Valid Dungeon](https://github.com/BenPyton/ProceduralDungeon/wiki/Is-Valid-Dungeon) and [Choose Door](https://github.com/BenPyton/ProceduralDungeon/wiki/Choose-Door) like you would do with the Procedural Dungeon plugin.\
Call its `Generate` somewhere (e.g. in its `Begin Play`).

## Installation

Install the [Procedural Dungeon plugin][DEPENDANCY] v3.3 or newer.

### - Blueprint-only Porject (*only for UE 5.3 at the moment*)
Download and unzip the [latest pre-compiled build](https://github.com/BenPyton/DungeonRules/releases/latest):\
in your engine at `<engine_path>/Engine/Plugins/Marketplace`\
**OR**\
in your project at `<project_path>/Plugins`.

### - C++ Project (*May require code modification for UE != 5.3*)
Download the sources and place it in your C++ project at `<project_path>/Plugins`.\
Regenerate your VS files and compile your project.

## License

This plugin is under [BSL-1.0 license](LICENSE).\
This means you are free to use this plugin for personal/free/commercial projects, you are also allowed to modify the source code and/or redistribute it.\
The only requirement is to add the copyright notice and a copy of the license when redistributing the source code or pre-compiled binaries, modified or not.

## *Support Me*

 If you like my plugins, please consider tipping:

[![Ko-fi](https://img.shields.io/badge/Ko--fi-ff5f5f?style=for-the-badge)](https://ko-fi.com/M4M3NW2JV)
[![liberapay](https://img.shields.io/badge/liberapay-f6c915?style=for-the-badge)](https://liberapay.com/BenPyton/donate)
[![PayPal](https://img.shields.io/badge/PayPal-142c8e?style=for-the-badge)](https://www.paypal.com/donate/?hosted_button_id=9VWP66JU5DZXN)

[DISCORD]: https://discord.gg/YE2dPda2CC
[DEPENDANCY]: https://github.com/BenPyton/ProceduralDungeon
