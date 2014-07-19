MineVoxel
=========

Convert KV6 3d files to Lua programs for [ComputerCraft](http://www.computercraft.info/) turtles.

Set up
==========
* The turtle has fuel when the program starts
* The turtle is full of building material (1024 blocks)
* At its left the turtle has two boxes where it can pick up:
  * Fuel in the first one
  * Building materials in the leftmost one

The boxes are not necessary if the construction requires less than 1024 blocks.

The program will never check if enough material is available in the material box. It is up to you to put enough. Same thing goes for the fuel. The turtle will refuel before picking up material if its fuel level is under 6000.

The total number of blocks required for the construction can be found at the first line of the generated program.

Usage
=====
    Usage:
    MineVoxel.exe [-d|-dc] kv6_file
      kv6_file: path to a .kv6 file
      -d: debug output
      -dc: comprehensive debug output

The Lua program will be output on the standard output, therefore to output to a file you need to use the `>` operator.
For example, this command:

    MineVoxel.exe C:\path\to\my\file.kv6 > lua_program.txt
will create a `lua_program.txt` file in the current directory.

This program can be served to a turtle in [ComputerCraft](http://www.computercraft.info/).


