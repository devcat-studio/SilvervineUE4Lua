// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "Internationalization/PolyglotTextData.h"
#include "Misc/QualifiedFrameTime.h"

#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCasePrimitiveTypes
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCasePrimitiveTypes, "Silvervine.UE4Lua.PrimitiveTypes", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCasePrimitiveTypes::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	VM->ExecuteString(
		TEXT("\n	function Test(arg)")
		TEXT("\n		return arg")
		TEXT("\n	end"));

	auto TestFunc = FSUE4LuaFunction::CreateFromGlobal(VM.ToSharedRef(), TEXT("Test"));
	if (!TestFunc.IsValid())
	{
		return false;
	}

	{
		TestTrue(TEXT("float to int cast"), TestFunc->Call<int64>(1.23f) == 1);
	}

	{
		TestTrue(TEXT("Parameter Passing : nil"), TestFunc->Call<FSUE4LuaStack::FNil>(FSUE4LuaStack::Nil) == FSUE4LuaStack::Nil);
	}
	{
		#define IMPLEMENT_TEST(TypeName, ...)														\
		{																							\
			const TypeName Arg(__VA_ARGS__);														\
			TestTrue(TEXT("Parameter Passing : ") #TypeName, TestFunc->Call<TypeName>(Arg) == Arg);	\
		}
		IMPLEMENT_TEST(int8, -123);
		IMPLEMENT_TEST(uint8, 123);
		IMPLEMENT_TEST(int16, -123);
		IMPLEMENT_TEST(uint16, 123);
		IMPLEMENT_TEST(int32, -123);
		IMPLEMENT_TEST(uint32, 123);
		IMPLEMENT_TEST(int64, -123);
		IMPLEMENT_TEST(uint64, 123);
		IMPLEMENT_TEST(bool, false);
		IMPLEMENT_TEST(bool, true);
		IMPLEMENT_TEST(float, 123.4f);
		IMPLEMENT_TEST(double, 123.4);
		IMPLEMENT_TEST(FVector, 1.0f, 2.0f, 3.0f);
		IMPLEMENT_TEST(FVector2D, 1.0f, 2.0f);
		IMPLEMENT_TEST(FVector4, 1.0f, 2.0f, 3.0f, 4.0f);
		IMPLEMENT_TEST(FIntPoint, 1, 2);
		IMPLEMENT_TEST(FIntVector, 1, 2, 3);
		IMPLEMENT_TEST(FPlane, 1.0f, 2.0f, 3.0f, 4.0f);
		IMPLEMENT_TEST(FRotator, 1.0f, 2.0f, 3.0f);
		IMPLEMENT_TEST(FQuat, 1.0f, 2.0f, 3.0f, 4.0f);
		IMPLEMENT_TEST(FLinearColor, 0.1f, 0.2f, 0.3f, 0.4f);
		IMPLEMENT_TEST(FColor, 1, 2, 3, 4);
		IMPLEMENT_TEST(FGuid, 1, 2, 3, 4);
		IMPLEMENT_TEST(FDateTime, 123);
		IMPLEMENT_TEST(FTimespan, 123);
		IMPLEMENT_TEST(FFrameRate, 123, 4);
		IMPLEMENT_TEST(FFrameNumber, 123);
		IMPLEMENT_TEST(FFrameTime, 123, 0.4f);
		IMPLEMENT_TEST(FPrimaryAssetType, TEXT("123"));
		IMPLEMENT_TEST(FPrimaryAssetId, TEXT("123"), TEXT("456"));
		IMPLEMENT_TEST(FSoftObjectPath, TEXT("123"), TEXT("456"));
		#undef IMPLEMENT_TEST			
	}
	{
		{
			const char* Arg = "123";
			TestTrue(TEXT("Parameter Passing : const char*"), TestFunc->Call<FString>(Arg) == FString(Arg));
		}
		{
			const TCHAR* Arg = TEXT("한글");
			TestTrue(TEXT("Parameter Passing : const TCHAR*"), TestFunc->Call<FString>(Arg) == FString(Arg));
		}
		{
			FString Arg = TEXT("str");
			TestTrue(TEXT("Parameter Passing : FString"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			FName Arg(TEXT("str"));
			TestTrue(TEXT("Parameter Passing : FName"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			FText Arg = FText::FromString(TEXT("str"));
			TestTrue(TEXT("Parameter Passing : FText"), TestFunc->Call<decltype(Arg)>(Arg).ToString() == Arg.ToString());
		}
	}
	{
		FName Arg = NAME_None;
		TestTrue(TEXT("Parameter Passing : NAME_None"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
	}
	{
		{
			FTransform Arg(FQuat(1.0f, 2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));
			TestTrue(TEXT("Parameter Passing : FTransform"), TestFunc->Call<decltype(Arg)>(Arg).Equals(Arg));
		}
		{
			FBox Arg(FVector(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f));
			TestTrue(TEXT("Parameter Passing : FBox"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			FBox2D Arg(FVector2D(1.0f, 2.0f), FVector2D(4.0f, 5.0f));
			TestTrue(TEXT("Parameter Passing : FBox2D"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			FBoxSphereBounds Arg(FVector(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), 7.0f);
			TestTrue(TEXT("Parameter Passing : FBoxSphereBounds"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			FRandomStream Arg(123);
			TestTrue(TEXT("Parameter Passing : FRandomSeed"), TestFunc->Call<decltype(Arg)>(Arg).GetCurrentSeed() == Arg.GetInitialSeed());
		}
		{
			FMatrix Arg(FPlane(1.0f, 2.0f, 3.0f, 4.0f), FPlane(5.0f, 6.0f, 7.0f, 8.0f), FPlane(9.0f, 10.0f, 11.0f, 12.0f), FPlane(13.0f, 14.0f, 15.0f, 16.0f));
			TestTrue(TEXT("Parameter Passing : FMatrix"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			struct FComparer
			{
				static bool Equals(const FQualifiedFrameTime& L, const FQualifiedFrameTime& R)
				{
					return L.Time == R.Time && L.Rate == R.Rate;
				}
			};

			FQualifiedFrameTime Arg(FFrameTime(123, 0.4f), FFrameRate(123, 4));
			TestTrue(TEXT("Parameter Passing : FQualifiedFrameTime"), FComparer::Equals(TestFunc->Call<decltype(Arg)>(Arg), Arg));
		}
		{
			struct FComparer
			{
				static bool Equals(const FTimecode& L, const FTimecode& R)
				{
					return L.Hours == R.Hours && L.Minutes == R.Minutes && L.Seconds == R.Seconds && L.Frames == R.Frames && L.bDropFrameFormat == R.bDropFrameFormat;
				}
			};

			FTimecode Arg(1, 2, 3, 4, true);
			TestTrue(TEXT("Parameter Passing : FTimecode"), FComparer::Equals(TestFunc->Call<decltype(Arg)>(Arg), Arg));
		}
		{
			struct FComparer
			{
				static bool Equals(const FPolyglotTextData& L, const FPolyglotTextData& R)
				{
					return L.GetCategory() == R.GetCategory();
				}
			};

			FPolyglotTextData Arg(ELocalizedTextSourceCategory::Editor, TEXT("123"), TEXT("456"), TEXT("789"), TEXT("012"));
			TestTrue(TEXT("Parameter Passing : FPolyglotTextData"), FComparer::Equals(TestFunc->Call<decltype(Arg)>(Arg), Arg));
		}
		{
			FFloatRangeBound Arg1 = FFloatRangeBound::Exclusive(123.0f);
			FFloatRangeBound Arg2 = FFloatRangeBound::Inclusive(123.0f);
			FFloatRangeBound Arg3 = FFloatRangeBound::Open();
			TestTrue(TEXT("Parameter Passing : FFloatRangeBound(Exclusive)"), TestFunc->Call<decltype(Arg1)>(Arg1) == Arg1);
			TestTrue(TEXT("Parameter Passing : FFloatRangeBound(Inclusive)"), TestFunc->Call<decltype(Arg2)>(Arg2) == Arg2);
			TestTrue(TEXT("Parameter Passing : FFloatRangeBound(Open)"), TestFunc->Call<decltype(Arg3)>(Arg3) == Arg3);
		}
		{
			FFloatRange Arg(FFloatRangeBound::Exclusive(123.0f), FFloatRangeBound::Inclusive(456.0f));
			TestTrue(TEXT("Parameter Passing : FFloatRange"), TestFunc->Call<decltype(Arg)>(Arg) == Arg);
		}
		{
			struct FComparer
			{
				static bool Equals(const FInt32Interval& L, const FInt32Interval& R)
				{
					return L.Min == R.Min && L.Max == R.Max;
				}
			};

			FInt32Interval Arg(123, 456);
			TestTrue(TEXT("Parameter Passing : FInt32Interval"), FComparer::Equals(TestFunc->Call<decltype(Arg)>(Arg), Arg));
		}
	}

	return true;
}