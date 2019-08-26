
How to Setup a File Loader
==========================

SUE4Lua reads all lua files through the [File Loader Class](../Source/SilvervineUE4Lua/Public/LuaFileLoader.h).
This guide explains how to select the appropriate lua file loader for each type of environment and build.

Lua Local FileLoader
--------------------

Developers who edit lua files must use a local file loader.
This is mainly used by engineers and designers.

How to Setup:

1. Open the project settings and go to Plugins -> SilvervineUE4Lua.

2. Uncheck 'Use Bundle File Loader'.

3. Set 'Local File Load Map' to the paths that contains lua files.

![](Images/LocalFileLoader.png)

* Set __virtual path__ as the key for the map. SUE4Lua uses a virtual path instead of an actual path when specifying a lua file path.
* Set the path containing the lua files as the value for the map.

It is recommended to use only the following two virtual paths.

* `SUE4Lua` : Used to read lua files in the SUE4Lua plugin.

    Ex: SUE4Lua/Framework/SUE4Lua.lua

* `Game` : Used to read lua files in game modules.

    Ex: Game/Main.lua

Note:
* [SUE4LuaSample](SUE4LuaSample.md) project is set up to use a local file loader.

Lua Bundle FileLoader
---------------------

A Bundle file is a file that combines several files togheter. Zip format is mainly used.

Usually, those who do not edit lua files do not use source code repositories, the build machine will automatically create and distribute the bundle file.

You must use the bundle file loader when distributing games. 

How to Setup:

1. Open the project settings and go to Plugins -> SilvervineUE4Lua.

2. Check 'Use Bundle File Loader'.

3. Set 'Bundle File Load Map' to the paths that contains bundle files.

![](Images/BundleFileLoader.png)

* Set the same virtual path as the local file loader as the key for the map.
* Set the path containing the bundle file as the value for the map.

Note:
* Currently, only ZIP format is supported.
* It is good to automate bundle creation and deployment by CI tools.

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)