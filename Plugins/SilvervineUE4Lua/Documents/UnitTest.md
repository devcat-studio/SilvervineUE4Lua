
Unit Test
=========

SUE4Lua uses the [UE4 Automation System](https://docs.unrealengine.com/en-US/Programming/Automation/index.html) to run unit tests.

When the editor starts, the registered unit tests are executed. If an error occurs, it is printed to the log.

To run unit tests yourself:
1. Go to Window -> Developer Tools -> Session Frontend.
2. On the Automation tab, check 'Silvervine.UE4Lua' and press the Start button.

![](Images/UnitTest_AutomationTab.png) ![](Images/UnitTest_AutomationLog.png)

Adding a unit test
------------------

Add your unit test [here](../Source/SilvervineUE4Lua/Private/Tests).

For example,
```cpp
//=============================================================================================================================
// FSUE4LuaTestCaseVM
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseVM, "Silvervine.UE4Lua.VM", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseVM::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();

	TestTrue(TEXT("VM.Create()"), VM.IsValid());

	return true;
}
```

------------------------------------------------
[Back to Programming Guide](ProgrammingGuide.md)