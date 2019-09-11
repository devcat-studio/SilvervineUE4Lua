
SilvervineUE4Lua
================

SilvervineUE4Lua (SUE4Lua) is a plug-in that allows you to make games using Lua in UE4.

We're using this plug-in to make a game called [DragonHound](https://www.youtube.com/watch?v=m-AS21f7Rao)

You can find the programming guide [here](ProgrammingGuide.md)

Why We Make SUE4Lua
-------------------

The UE4 blueprint is a great tool, but there are some problems.

* It is very hard to manage complexity in Blueprints. 
* Collaboration is difficult. Binary format makes it difficult to branch, merge and diff.
* Debugging is difficult. Existing blueprint debugging tools are not sufficent to find a bug.

To solve these problems, we decided to use a lua script language and developed a plug-in like this.

Features
--------

SUE4Lua has several features that are different from other UE4 Lua script plug-ins.

* Lua script is treated as a source code, not as a binary asset.

    You can uses a text editor, such as Visual Studio or Visual Studio Code.

* Tools such as git can be used as Lua file repository.

    You can branch, merge and diff your scripts easily.

* Derived classes are not created by SUE4Lua.

    We think a derived lua class of the blueprint or C++ class has more cons over pros.

* Lua script is used for reducing complexity of the blueprint codes and communicating between C++ and the blueprint.

Supported Environment
---------------------

Because the game we're making is targeted for the Windows, we couldn't test it in other environments.

SUE4Lua doesn't have any platform-dependent codes, we think it runs without any problems on non-Windows platforms. If does not, please let us know and we'll try to fix it.

The following versions of UE4 are supported:

* 4.21
* 4.22
* 4.23

We have been developing SUE4Lua since version 4.19, so we expect it to work well with older engine versions.

Note:
* 4.20 has a bug in the Unreal Header Tool (UHT) and the codegen module in SUE4Lua is not compiled with it. This issue is difficult to workaround without engine modification, so SUE4Lua is not recommended for 4.20.
* In 4.23 installed engine build, there is an issue where the SUE4Lua plugin causes a build error. You can workaround it as follows:
1. Open Engine/Config/BaseEngine.ini
2. Modify `HasInstalledPlatformInfo` to _false_ in the `InstalledPlatforms` section.

The supported Lua versions are:

* 5.3.4

Version 5.1.5 may work without problems, but there are several issues.
* Because 5.1.5 doesn't support _integer_ type, `int64` value passed to Lua may be truncated.

* You can not use _pairs()_ when iterating UObject, UStruct, TArray and so on. The `__pairs` meta method is not supported in 5.1.5.

Note:
* SUE4Lua already contains the Lua 5.3.4 source codes.
* We have no plans to support LuaJIT.

Installation
------------

1. Clone this repository and copy the Plugins/SilvervineUE4Lua folder under the Plugins folder of your uproject.

2. Open the project in the UE4 editor and enable SilvervineUE4Lua in the Plugins section.

![](Images/EnableSUE4Lua.png)

3. Restart the editor. When build completes you are ready to use SUE4Lua. 

Note:
* There are no plans to support binary installation.

Quick Example
-------------

To help you understand how to use SUE4Lua, we created an example project called SUE4LuaSample.

This uproject is a ported version of the Vehicle_BP template.
For more information, see the [SUE4LuaSample](SUE4LuaSample.md).

[Unit Test](../Source/SilvervineUE4Lua/Private/Tests) are also good examples.

Future Works
------------

We have the following plans to do. Along with game development, we improves SUE4Lua continuously.

* More documentation and English translation
* Better understandable warnings and error message
* Interactive console(REPL) supports

Contributors
------------

* Jeon Hyoung Kyou(henjeon@nexon.co.kr)
* Yoo Joo Won(sauna59479@nexon.co.kr)

Contact
-------

If you have any problems, questions, or features you need, please feel free to email to henjeon@nexon.co.kr.