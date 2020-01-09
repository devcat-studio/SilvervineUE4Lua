// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaUnitTests.h"

#include "Kismet/KismetMathLibrary.h"

#include "LuaFunction.h"
#include "LuaVirtualMachine.h"


//=============================================================================================================================
// FSUE4LuaTestCaseMath
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMath, "Silvervine.UE4Lua.Math", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMath::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;

		float Width = FMath::Abs(Arg1);
		float Height = FMath::Abs(Arg2);
		float Min = FGenericPlatformMath::Min(Width, Height);
		float Max = FGenericPlatformMath::Max(Width, Height);
		float Ratio = Min / Max;

		float Result = Max * FMath::Sqrt(1.f + Ratio * Ratio);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Math.Hypotenuse(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Hypotenuse(Width, Height)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Result);
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Math.GridSnap(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.GridSnap(Location, GridSize)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FMath::GridSnap(Arg1, Arg2));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.ClampAngle(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.ClampAngle(AngleDegrees, MinAngleDegrees, MaxAngleDegrees)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::ClampAngle(Arg1, Arg2, Arg3));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.Clamp(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Clamp(Value, Min, Max)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::Clamp(Arg1, Arg2, Arg3));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.Lerp(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Lerp(F1, F2, Alpha)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == (Arg1 + Arg3 * (Arg2 - Arg1)));
	}
	{
		float Arg = 1.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Round(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Round(F)"),
			FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg) == FMath::RoundToInt(Arg));
	}
	{
		float Arg = 1.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Truncate(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Truncate(Float)"),
			FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg) == FMath::TruncToInt(Arg));
	}
	{
		FVector Arg(1.5f, 2.5f, 3.5f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.TruncateVector(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.TruncateVector(Vector)"),
			FSUE4LuaFunction::CallGlobal<FIntVector>(VM.ToSharedRef(), TEXT("Test"), Arg) == FIntVector(FMath::TruncToInt(Arg.X), FMath::TruncToInt(Arg.Y), FMath::TruncToInt(Arg.Z)));
	}
	{
		float Arg = 1.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.SignOfFloat(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.SignOfFloat(Float)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == FMath::Sign(Arg));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;

		float Result;
		if (Arg2 == Arg3)
		{
			if (Arg1 < Arg2)
			{
				Result = 0.f;
			}
			else
			{
				Result = 1.f;
			}
		}
		else
		{
			if (Arg2 > Arg3)
			{
				Swap(Arg2, Arg3);
			}
			Result = (Arg1 - Arg2) / (Arg3 - Arg2);
		}

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.NormalizeToRange(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.NormalizeToRange(Value, RangeMin, RangeMax)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Result);
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;
		float Arg4 = 4.0f;
		float Arg5 = 5.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3, arg4, arg5)")
			TEXT("\n		return UE4.Math.MapRangeUnclamped(arg1, arg2, arg3, arg4, arg5)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.MapRangeUnclamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3, Arg4, Arg5) == FMath::GetMappedRangeValueUnclamped(FVector2D(Arg2, Arg3), FVector2D(Arg4, Arg5), Arg1));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;
		float Arg4 = 4.0f;
		float Arg5 = 5.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3, arg4, arg5)")
			TEXT("\n		return UE4.Math.MapRangeClamped(arg1, arg2, arg3, arg4, arg5)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.MapRangeClamped(Value, InRangeA, InRangeB, OutRangeA, OutRangeB)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3, Arg4, Arg5) == FMath::GetMappedRangeValueClamped(FVector2D(Arg2, Arg3), FVector2D(Arg4, Arg5), Arg1));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 0.5f;
		float Arg4 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3, arg4)")
			TEXT("\n		return UE4.Math.InterpEaseInOut(arg1, arg2, arg3, arg4)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.InterpEaseInOut(Float1, Float2, Alpha, Exponent)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3, Arg4) == FMath::InterpEaseInOut(Arg1, Arg2, Arg3, Arg4));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Math.MakePulsatingValue(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.MakePulsatingValue(InCurrentTime, InPulsesPerSecond)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FMath::MakePulsatingValue(Arg1, Arg2));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.MakePulsatingValue(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.MakePulsatingValue(InCurrentTime, InPulsesPerSecond, InPhase)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::MakePulsatingValue(Arg1, Arg2, Arg3));
	}
	{
		float Arg1 = 1.0f;
		float Arg2 = 2.0f;
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.FixedTurn(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.FixedTurn(InCurrent, InDesired, InDeltaRate)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::FixedTurn(Arg1, Arg2, Arg3));
	}
	{
		float Arg1 = 45.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Sin(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Sin(Rad)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::Sin(Arg1));
	}
	{
		float Arg1 = 0.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Asin(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Asin(Value)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::Asin(Arg1));
	}
	{
		float Arg1 = 45.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Cos(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Cos(Rad)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::Cos(Arg1));
	}
	{
		float Arg1 = 0.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Acos(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Acos(Value)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::Acos(Arg1));
	}
	{
		float Arg1 = 45.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Tan(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Tan(Rad)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::Tan(Arg1));
	}
	{
		float Arg1 = 0.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.Atan(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Atan(Value)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::Atan(Arg1));
	}
	{
		float Arg1 = 50.0f;
		float Arg2 = 100.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Math.Atan2(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.Atan2(Value)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FMath::Atan2(Arg1, Arg2));
	}
	{
		float Arg1 = 45.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.DegreesToRadians(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.DegreesToRadians(Degree)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::DegreesToRadians(Arg1));
	}
	{
		float Arg1 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.RadiansToDegrees(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.RadiansToDegrees(Degree)"),
			FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::RadiansToDegrees(Arg1));
	}
	{
		float Arg1 = 0.000000001f; // smaller then SMALL_NUMBER(0.00000001);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.IsNearlyZero(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.IsNearlyZero(Value) #1"),
			FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1));
	}
	{
		float Arg1 = 45.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Math.IsNearlyZero(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.IsNearlyZero(Value) #2"),
			FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FMath::IsNearlyZero(Arg1));
	}
	{
		float Arg1 = 0.1f;
		float Arg2 = 0.2f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Math.IsNearlyZero(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.IsNearlyZero(Value, Tolerance) #2"),
			FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FMath::IsNearlyZero(Arg1, Arg2));
	}
	{
		float Arg1 = 45.0f;
		float Arg2 = 45.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Math.IsNearlyEqual(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.IsNearlyEqual(Value1, Value2)"),
			FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FMath::IsNearlyEqual(Arg1, Arg2));
	}
	{
		float Arg1 = 45.0f;
		float Arg2 = 45.5f;
		float Arg3 = 1.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return UE4.Math.IsNearlyEqual(arg1, arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Math.IsNearlyEqual(Value1, Value2, Tolerance)"),
			FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::IsNearlyEqual(Arg1, Arg2, Arg3));
	}
	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathVector
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathVector, "Silvervine.UE4Lua.MathVector", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathVector::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg)")
			TEXT("\n		return -arg")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector unary minus"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(-Arg));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector + Vector"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector + float"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector - Vector"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector - float"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector * Vector"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector * float"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector / Vector"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector / float"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector == Vector"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Equals(Vector) #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Equals(Vector) #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.Equals(Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);
		float Arg3 = 10.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Equals(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Equals(Vector, Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Arg1.Equals(Arg2, Arg3));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:AllComponentsEqual()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.AllComponentsEqual()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.AllComponentsEqual());
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 10.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:AllComponentsEqual(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.AllComponentsEqual(Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.AllComponentsEqual(Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:DotProduct(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.DotProduct(Vector)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector::DotProduct(Arg1, Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:CrossProduct(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.CrossProduct(Vector)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(FVector::CrossProduct(Arg1, Arg2)));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:Size()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Size()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.Size());
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:SizeSquared()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.SizeSquared()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.SizeSquared());
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:Size2D()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Size2D()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.Size2D());
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:SizeSquared2D()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.SizeSquared2D()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.SizeSquared2D());
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 1.0f;
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:ClampToSize(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.ClampToSize(Min, Max)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(Arg1.GetClampedToSize(Arg2, Arg3)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Distance(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Distance(Vector)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector::Dist(Arg1, Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:DistanceSquared(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.DistanceSquared(Vector)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector::DistSquared(Arg1, Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Distance2D(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Distance2D(Vector)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector::Dist2D(Arg1, Arg2));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:DistanceSquared2D(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.DistanceSquared2D(Vector)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector::DistSquared2D(Arg1, Arg2));
	}
	{
		FVector Arg1(0.0f, 0.0f, 0.0f);
		FVector Arg2(1.0f, 2.0f, 3.0f);
		FVector Arg3(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:DistancePointToSegment(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.DistancePointToSegment(SegmentStart, SegmentEnd)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::PointDistToSegment(Arg1, Arg2, Arg3));
	}
	{
		FVector Arg1(0.0f, 0.0f, 0.0f);
		FVector Arg2(1.0f, 2.0f, 3.0f);
		FVector Arg3(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:DistancePointToLine(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.DistancePointToLine(LineOrigin, LineDirection)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == FMath::PointDistToLine(Arg1, Arg3, Arg2));
	}
	{
		FVector Arg(0.000001f, 0.000001f, 0.000001f); // smaller then KINDA_SMALL_NUMBER(0.00001)

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNearlyZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.IsNearlyZero() #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg));
	}
	{
		FVector Arg(0.1f, 0.2f, 0.3f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNearlyZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.IsNearlyZero() #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsNearlyZero());
	}
	{
		FVector Arg1(0.1f, 0.2f, 0.3f);
		float Arg2 = 0.4f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:IsNearlyZero(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.IsNearlyZero(Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.IsNearlyZero(Arg2));
	}
	{
		FVector Arg(0.0f, 0.0f, 0.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.IsZero() #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.IsZero() #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsZero());
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);
		Arg.Normalize();

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNormalized()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.IsNormalized()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsNormalized());
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(-1.0f, 2.0f, -3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:UnrotateByRotator(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.UnrotateByRotator(Rotator)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg2.UnrotateVector(Arg1)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(-1.0f, 2.0f, -3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:RotateByRotator(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.RotateByRotator(Rotator)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg2.RotateVector(Arg1)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 60.0f;
		FVector Arg3(-1.0f, 1.0f, -1.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:RotateByAngleAxis(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.RotateByAngleAxis(AngleDeg, Axis)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(Arg1.RotateAngleAxis(Arg2, Arg3)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);
		float Arg3 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Lerp(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Lerp(Vector, Alpha)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(Arg1 + Arg3 * (Arg2 - Arg1)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(1.0f, 0.0f, 0.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Reflection(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.Reflection(Surface)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(FMath::GetReflectionVector(Arg1, Arg2)));
	}
	{
		FVector Arg1(0.0f, 0.0f, 0.0f);
		FVector Arg2(1.0f, 2.0f, 3.0f);
		FVector Arg3(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:FindClosestPointOnSegment(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.FindClosestPointOnSegment(SegmentStart, SegmentEnd)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(FMath::ClosestPointOnSegment(Arg1, Arg2, Arg3)));
	}
	{
		FVector Arg1(0.0f, 0.0f, 0.0f);
		FVector Arg2(1.0f, 2.0f, 3.0f);
		FVector Arg3(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:FindClosestPointOnLine(arg2, arg3)")
			TEXT("\n	end"));

		const FVector SafeDir = Arg3.GetSafeNormal();
		const FVector ClosestPoint = Arg2 + (SafeDir * ((Arg1 - Arg2) | SafeDir));

		TestTrue(TEXT("Vector.FindClosestPointOnLine(LineOrigin, LineDirection)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(ClosestPoint));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:ProjectVectorOnToVector(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.ProjectVectorOnToVector(Vector)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.ProjectOnTo(Arg2)));
	}
	{
		FVector Arg1(0.0f, 0.0f, 0.0f);
		FVector Arg2(1.0f, 2.0f, 3.0f);
		FVector Arg3(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:ProjectPointOnToPlane(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.ProjectPointOnToPlane(PlaneBase, PlaneNormal)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(FVector::PointPlaneProject(Arg1, Arg2, Arg3)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:ProjectVectorOnToPlane(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.ProjectVectorOnToPlane(PlaneNormal)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(FVector::VectorPlaneProject(Arg1, Arg2)));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMin()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.GetMin()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetMin());
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMax()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.GetMax()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetMax());
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:GetNormal(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.GetNormal()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetSafeNormal()));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 1.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:GetNormal(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.GetNormal(Tolerence)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1).Equals(Arg1.GetSafeNormal(Arg2)));
	}
	{
		FVector Arg1(1.0f, 2.0f, 3.0f);
		FVector Arg2(6.0f, 5.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:GetUnitDirection(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.GetUnitDirection(Vector)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals((Arg2 - Arg1).GetSafeNormal()));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:ToTransform()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector.ToTransform()"), FSUE4LuaFunction::CallGlobal<FTransform>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(FTransform(Arg)));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathVector2D
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathVector2D, "Silvervine.UE4Lua.MathVector2D", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathVector2D::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg)")
			TEXT("\n		return -arg")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D unary minus"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(-Arg));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D + Vector2D"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		float Arg2 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D + float"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D - Vector2D"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		float Arg2 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D - float"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D * Vector2D"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		float Arg2 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D * float"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D / Vector2D"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		float Arg2 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D / float"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D == Vector2D"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Equals(Vector2D) #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Equals(Vector2D) #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.Equals(Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);
		float Arg3 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Equals(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Equals(Vector2D, Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Arg1.Equals(Arg2, Arg3));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:DotProduct(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.DotProduct(Vector2D)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector2D::DotProduct(Arg1, Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:CrossProduct(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.CrossProduct(Vector2D)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector2D::CrossProduct(Arg1, Arg2));
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:Size()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Size()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.Size());
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:SizeSquared()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.SizeSquared()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.SizeSquared());
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Distance(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Distance(Vector2D)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector2D::Distance(Arg1, Arg2));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:DistanceSquared(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.DistanceSquared(Vector2D)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FVector2D::DistSquared(Arg1, Arg2));
	}
	{
		FVector2D Arg(0.000001f, 0.000001f); // smaller then KINDA_SMALL_NUMBER(0.00001)

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNearlyZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.IsNearlyZero() #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg));
	}
	{
		FVector2D Arg(0.1f, 0.2f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNearlyZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.IsNearlyZero() #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsNearlyZero());
	}
	{
		FVector2D Arg1(0.1f, 0.2f);
		float Arg2 = 0.3f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:IsNearlyZero(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.IsNearlyZero(Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.IsNearlyZero(Arg2));
	}
	{
		FVector2D Arg(0.0f, 0.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.IsZero() #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg));
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.IsZero() #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsZero());
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		float Arg2 = 3.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Rotate(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Rotate(AngleDeg)"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.GetRotated(Arg2)));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);
		float Arg3 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Lerp(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.Lerp(Vector2D, Alpha)"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(Arg1 + Arg3 * (Arg2 - Arg1)));
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMin()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.GetMin()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetMin());
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMax()")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.GetMax()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetMax());
	}
	{
		FVector2D Arg(1.0f, 2.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:GetNormal(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.GetNormal()"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetSafeNormal()));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		float Arg2 = 1.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:GetNormal(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.GetNormal(Tolerence)"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1).Equals(Arg1.GetSafeNormal(Arg2)));
	}
	{
		FVector2D Arg1(1.0f, 2.0f);
		FVector2D Arg2(3.0f, 4.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:GetUnitDirection(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Vector2D.GetUnitDirection(Vector)"), FSUE4LuaFunction::CallGlobal<FVector2D>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals((Arg2 - Arg1).GetSafeNormal()));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathTransform
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathTransform, "Silvervine.UE4Lua.MathTransform", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathTransform::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FTransform Arg2(FRotator(2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform + Transform"), FSUE4LuaFunction::CallGlobal<FTransform>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FTransform Arg2(FRotator(2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform * Transform"), FSUE4LuaFunction::CallGlobal<FTransform>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FTransform Arg(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform == Transform"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FTransform Arg(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.Equals(Transform) #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FTransform Arg2(FRotator(2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.Equals(Transform) #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.Equals(Arg2));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FTransform Arg2(FRotator(2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));
		float Arg3 = 10.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Equals(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.Equals(Transform, Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Arg1.Equals(Arg2, Arg3));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FVector Arg2(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:TransformLocation(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.TransformLocation(Location)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.TransformPosition(Arg2)));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FVector Arg2(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:TransformDirection(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.TransformDirection(Direction)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.TransformPositionNoScale(Arg2)));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FRotator Arg2(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:TransformRotation(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.TransformRotation(Rotation)"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.TransformRotation(Arg2.Quaternion()).Rotator()));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FVector Arg2(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:InverseTransformLocation(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.InverseTransformLocation(Location)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.InverseTransformPosition(Arg2)));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FVector Arg2(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:InverseTransformDirection(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.InverseTransformDirection(Direction)"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.InverseTransformPositionNoScale(Arg2)));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FRotator Arg2(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:InverseTransformRotation(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.InverseTransformRotation(Rotation)"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.InverseTransformRotation(Arg2.Quaternion()).Rotator()));
	}
	{
		FTransform Arg(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Inverse()")
			TEXT("\n	end"));

		TestTrue(TEXT("Transform.Inverse()"), FSUE4LuaFunction::CallGlobal<FTransform>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.Inverse()));
	}
	{
		FTransform Arg1(FRotator(1.0f, 2.0f, 3.0f), FVector(4.0f, 5.0f, 6.0f), FVector(7.0f, 8.0f, 9.0f));
		FTransform Arg2(FRotator(2.0f, 3.0f, 4.0f), FVector(5.0f, 6.0f, 7.0f), FVector(8.0f, 9.0f, 10.0f));
		float Arg3 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Lerp(arg2, arg3)")
			TEXT("\n	end"));

		FTransform Result1 = FSUE4LuaFunction::CallGlobal<FTransform>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3);

		FTransform Result2;
		{
			FTransform NArg1 = Arg1;
			FTransform NArg2 = Arg2;
			NArg1.NormalizeRotation();
			NArg2.NormalizeRotation();
			Result2.Blend(NArg1, NArg2, Arg3);
		}

		TestTrue(TEXT("Transform.Lerp(Transform, Alpha)"), Result1.Equals(Result2));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathRotator
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathRotator, "Silvervine.UE4Lua.MathRotator", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathRotator::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator + Rotator"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator - Rotator"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator * float"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator == Rotator"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		auto a = FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg);

		TestTrue(TEXT("Rotator.Equals(Rotator) #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.Equals(Rotator) #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.Equals(Arg2));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);
		float Arg3 = 10.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Equals(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.Equals(Rotator, Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Arg1.Equals(Arg2, Arg3));
	}
	{
		FRotator Arg(0.000001f, 0.000001f, 0.000001f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNearlyZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.IsNearlyZero() #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg));
	}
	{
		FRotator Arg(0.1f, 0.2f, 0.3f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNearlyZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.IsNearlyZero() #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsNearlyZero());
	}
	{
		FRotator Arg1(0.1f, 0.2f, 0.3f);
		float Arg2 = 0.4f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:IsNearlyZero(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.IsNearlyZero(Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.IsNearlyZero(Arg2));
	}
	{
		FRotator Arg(0.0f, 0.0f, 0.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.IsZero() #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsZero()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.IsZero() #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsZero());
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:Inverse()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.Inverse()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetInverse()));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);
		float Arg3 = 0.5f;
		bool Arg4 = true;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3, arg4)")
			TEXT("\n		return arg1:Lerp(arg2, arg3, arg4)")
			TEXT("\n	end"));

		FRotator Result;
		{
			FQuat AQuat(Arg1);
			FQuat BQuat(Arg2);
			FQuat ResultQuat = FQuat::Slerp(AQuat, BQuat, Arg3);
			ResultQuat.Normalize();

			Result = ResultQuat.Rotator();
		}

		TestTrue(TEXT("Rotator.Lerp(Rotator, Alpha, bShortestPath) #1"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3, Arg4).Equals(Result));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);
		float Arg3 = 0.5f;
		bool Arg4 = false;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3, arg4)")
			TEXT("\n		return arg1:Lerp(arg2, arg3, arg4)")
			TEXT("\n	end"));

		FRotator Result;
		{
			FRotator DeltaAngle = Arg2 - Arg1;
			Result = Arg1 + Arg3 * DeltaAngle;
		}

		TestTrue(TEXT("Rotator.Lerp(Rotator, Alpha, bShortestPath) #2"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3, Arg4).Equals(Result));
	}
	{
		FRotator Arg1(1.0f, 2.0f, 3.0f);
		FRotator Arg2(4.0f, 5.0f, 6.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Delta(arg2)")
			TEXT("\n	end"));

		FRotator Delta = Arg1 - Arg2;
		Delta.Normalize();

		TestTrue(TEXT("Rotator.Delta()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Delta));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAxisX()")
			TEXT("\n	end"));

		FRotationMatrix RMatrix(Arg);
		FVector XAxis = RMatrix.GetScaledAxis(EAxis::X);

		TestTrue(TEXT("Rotator.GetAxisX()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(XAxis));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAxisY()")
			TEXT("\n	end"));

		FRotationMatrix RMatrix(Arg);
		FVector YAxis = RMatrix.GetScaledAxis(EAxis::Y);

		TestTrue(TEXT("Rotator.GetAxisY()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(YAxis));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAxisZ()")
			TEXT("\n	end"));

		FRotationMatrix RMatrix(Arg);
		FVector ZAxis = RMatrix.GetScaledAxis(EAxis::Z);

		TestTrue(TEXT("Rotator.GetAxisZ()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(ZAxis));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetNormal()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.GetNormal()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetNormalized()));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Rotator.MakeRotFromX(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.MakeRotFromX()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(FRotationMatrix::MakeFromX(Arg).Rotator()));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Rotator.MakeRotFromY(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.MakeRotFromY()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(FRotationMatrix::MakeFromY(Arg).Rotator()));
	}
	{
		FVector Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Rotator.MakeRotFromZ(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.MakeRotFromZ()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(FRotationMatrix::MakeFromZ(Arg).Rotator()));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:ToQuat()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.ToQuat()"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.Quaternion()));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetForwardVector()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.GetAxisX()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.Vector()));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetForwardVector()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.GetForwardVector()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.Vector()));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetRightVector()")
			TEXT("\n	end"));

		FRotationMatrix RMatrix(Arg);
		FVector YAxis = RMatrix.GetScaledAxis(EAxis::Y);

		TestTrue(TEXT("Rotator.GetRightVector()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(YAxis));
	}
	{
		FRotator Arg(1.0f, 2.0f, 3.0f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetUpVector()")
			TEXT("\n	end"));

		FRotationMatrix RMatrix(Arg);
		FVector ZAxis = RMatrix.GetScaledAxis(EAxis::Z);

		TestTrue(TEXT("Rotator.GetUpVector()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(ZAxis));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathQuat
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathQuat, "Silvervine.UE4Lua.MathQuat", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathQuat::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat + Quat"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat - Quat"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat * float"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat * Quat"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		float Arg2 = 4.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat / float"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat == Quat"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.Equals(Quat) #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.Equals(Quat) #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.Equals(Arg2));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());
		float Arg3 = 10.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Equals(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.Equals(Quat, Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Arg1.Equals(Arg2, Arg3));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:Size()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.Size()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.Size());
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:SizeSquared()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.SizeSquared()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.SizeSquared());
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		Arg.Normalize();

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsNormalized()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.IsNormalized()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsNormalized());
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:Inverse()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.Inverse()"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.Inverse()));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());
		float Arg3 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3, arg4)")
			TEXT("\n		return arg1:Lerp(arg2, arg3, arg4)")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.Lerp(Rotator, Alpha)"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(FMath::Lerp(Arg1, Arg2, Arg3)));
	}
	{
		FQuat Arg1(FRotator(1.0f, 2.0f, 3.0f).Quaternion());
		FQuat Arg2(FRotator(4.0f, 5.0f, 6.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:AngularDistance(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.AngularDistance(Quat)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1.AngularDistance(Arg2)));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAngle()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.GetAngle()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetAngle());
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetRotationAxis()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.GetRotationAxis()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetRotationAxis()));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAxisX()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.GetAxisX()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetAxisX()));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAxisY()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.GetAxisY()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetAxisY()));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetAxisZ()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.GetAxisZ()"), FSUE4LuaFunction::CallGlobal<FVector>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetAxisZ()));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetNormal()")
			TEXT("\n	end"));

		TestTrue(TEXT("Quat.GetNormal()"), FSUE4LuaFunction::CallGlobal<FQuat>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.GetNormalized()));
	}
	{
		FQuat Arg(FRotator(1.0f, 2.0f, 3.0f).Quaternion());

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:ToRotator()")
			TEXT("\n	end"));

		TestTrue(TEXT("Rotator.ToRotator()"), FSUE4LuaFunction::CallGlobal<FRotator>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.Rotator()));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathColor
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathColor, "Silvervine.UE4Lua.MathColor", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathColor::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FColor Arg1(1, 2, 3, 4);
		FColor Arg2(5, 6, 7, 8);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		FColor Result = FSUE4LuaFunction::CallGlobal<FColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2);
		Arg1 += Arg2;

		TestTrue(TEXT("Color + Color"), Result == Arg1);
	}
	{
		FColor Arg(1, 2, 3, 4);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Color == Color"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FColor Arg(1, 2, 3, 4);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:ToLinearColor()")
			TEXT("\n	end"));

		TestTrue(TEXT("Color.ToLinearColor()"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg).Equals(Arg.ReinterpretAsLinear()));
	}

	return true;
}

//=============================================================================================================================
// FSUE4LuaTestCaseMathLinearColor
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathLinearColor, "Silvervine.UE4Lua.MathLinearColor", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathLinearColor::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor + LinearColor"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 + Arg2));
	}
	{
		FLinearColor Arg1(0.5f, 0.6f, 0.7f, 0.8f);
		FLinearColor Arg2(0.1f, 0.2f, 0.3f, 0.4f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor - LinearColor"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 - Arg2));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor * LinearColor"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		float Arg2 = 2.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor * float"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 * Arg2));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor / LinearColor"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		float Arg2 = 2.0f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor / float"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1 / Arg2));
	}
	{
		FLinearColor Arg(0.1f, 0.2f, 0.3f, 0.4f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor == LinearColor"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FLinearColor Arg(0.1f, 0.2f, 0.3f, 0.4f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Equals(LinearColor) #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg, Arg));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Equals(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Equals(LinearColor) #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.Equals(Arg2));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);
		float Arg3 = 0.2f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Equals(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Equals(LinearColor, Tolerance)"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3) == Arg1.Equals(Arg2, Arg3));
	}
	{
		FLinearColor Arg(0.000001f, 0.000001f, 0.000001f, 0.000001f); // smaller then DELTA(0.00001)

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsAlmostBlack()")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.IsAlmostBlack()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.IsAlmostBlack());
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		float Arg2 = 0.1f;
		float Arg3 = 0.2f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Clamp(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Clamp(Min, Max)"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(Arg1.GetClamped(Arg2, Arg3)));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);
		float Arg3 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:Lerp(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Lerp(LinearColor, Alpha)"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(Arg1 + Arg3 * (Arg2 - Arg1)));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);
		float Arg3 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2, arg3)")
			TEXT("\n		return arg1:LerpUsingHSV(arg2, arg3)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.LerpUsingHSV(LinearColor, Alpha)"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2, Arg3).Equals(FLinearColor::LerpUsingHSV(Arg1, Arg2, Arg3)));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		float Arg2 = 0.5f;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Desaturate(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Desaturate(Desaturation)"), FSUE4LuaFunction::CallGlobal<FLinearColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2).Equals(Arg1.Desaturate(Arg2)));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		FLinearColor Arg2(0.5f, 0.6f, 0.7f, 0.8f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:Distance(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.Distance(LinearColor)"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (FLinearColor::Dist(Arg1, Arg2)));
	}
	{
		FLinearColor Arg(0.1f, 0.2f, 0.3f, 0.4f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMax()")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.GetMax()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetMax());
	}
	{
		FLinearColor Arg(0.1f, 0.2f, 0.3f, 0.4f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMin()")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.GetMin()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetMin());
	}
	{
		FLinearColor Arg(0.1f, 0.2f, 0.3f, 0.4f);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetLuminance()")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.GetLuminance()"), FSUE4LuaFunction::CallGlobal<float>(VM.ToSharedRef(), TEXT("Test"), Arg) == Arg.GetLuminance());
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		bool Arg2 = false;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:ToColor(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.GetLuminance(bSRGB) #1"), FSUE4LuaFunction::CallGlobal<FColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.ToFColor(Arg2));
	}
	{
		FLinearColor Arg1(0.1f, 0.2f, 0.3f, 0.4f);
		bool Arg2 = true;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:ToColor(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("LinearColor.GetLuminance(bSRGB) #2"), FSUE4LuaFunction::CallGlobal<FColor>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.ToFColor(Arg2));
	}

	return true;
}


//=============================================================================================================================
// FSUE4LuaTestCaseMathDateTime
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathDateTime, "Silvervine.UE4Lua.MathDateTime", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathDateTime::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FDateTime Arg1(2000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime + FTimespan"), FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 + Arg2));
	}
	{
		FDateTime Arg1(2000);
		FDateTime Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime - DateTime"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 - Arg2));
	}
	{
		FDateTime Arg1(2000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime - Timespan"), FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 - Arg2));
	}
	{
		FDateTime Arg1(1000);
		FDateTime Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime == DateTime #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2));
	}
	{
		FDateTime Arg1(2000);
		FDateTime Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime == DateTime #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 == Arg2));
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetDate()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetDate()"), FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetDate());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetDay()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetDay()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetDay());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetDayOfYear()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetDayOfYear()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetDayOfYear());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetHour()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetHour()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetHour());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetHour12()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetHour12()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetHour12());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMillisecond()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetMillisecond()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetMillisecond());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMinute()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetMinute()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetMinute());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMonth()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetMonth()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetMonth());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetSecond()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetSecond()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetSecond());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetTimeOfDay()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetTimeOfDay()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetTimeOfDay());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetYear()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.GetYear()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetYear());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsAfternoon()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.IsAfternoon()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.IsAfternoon());
	}
	{
		FDateTime Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:IsMorning()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.IsMorning()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.IsMorning());
	}
	{
		int32 Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.DateTime.DaysInYear(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.DaysInYear()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FDateTime::DaysInYear(Arg1));
	}
	{
		int32 Arg1 = 2000;
		int32 Arg2 = 12;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.DateTime.DaysInMonth(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.DaysInMonth()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FDateTime::DaysInMonth(Arg1, Arg2));
	}
	{
		int32 Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.DateTime.IsLeapYear(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.IsLeapYear()"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1) == FDateTime::IsLeapYear(Arg1));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.DateTime.MaxValue()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.MaxValue()"), FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test")) == FDateTime::MaxValue());
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.DateTime.MinValue()")
			TEXT("\n	end"));

		TestTrue(TEXT("DateTime.MinValue()"), FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test")) == FDateTime::MinValue());
	}
	{
		FDateTime DateTime1 = FDateTime::Now();

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.DateTime.Now()")
			TEXT("\n	end"));

		FDateTime DateTime2 = FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test"));
		FDateTime DateTime3 = FDateTime::Now();

		TestTrue(TEXT("DateTime.Now()"), (FTimespan(0) <= DateTime2 - DateTime1) && (DateTime2 - DateTime1 <= DateTime3 - DateTime1));
	}
	{
		FDateTime DateTime1 = FDateTime::Today();

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.DateTime.Today()")
			TEXT("\n	end"));

		FDateTime DateTime2 = FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test"));
		FDateTime DateTime3 = FDateTime::Today();

		TestTrue(TEXT("DateTime.Today()"), (FTimespan(0) <= DateTime2 - DateTime1) && (DateTime2 - DateTime1 <= DateTime3 - DateTime1));
	}
	{
		FDateTime DateTime1 = FDateTime::UtcNow();

		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.DateTime.UtcNow()")
			TEXT("\n	end"));

		FDateTime DateTime2 = FSUE4LuaFunction::CallGlobal<FDateTime>(VM.ToSharedRef(), TEXT("Test"));
		FDateTime DateTime3 = FDateTime::UtcNow();

		TestTrue(TEXT("DateTime.UtcNow()"), (FTimespan(0) <= DateTime2 - DateTime1) && (DateTime2 - DateTime1 <= DateTime3 - DateTime1));
	}

	return true;
}


//=============================================================================================================================
// FSUE4LuaTestCaseMathTimespan
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathTimespan, "Silvervine.UE4Lua.MathTimespan", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathTimespan::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return -arg1")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan unary minus"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == (-Arg1));
	}
	{
		FTimespan Arg1(2000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 + arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan + Timespan"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 + Arg2));
	}
	{
		FTimespan Arg1(2000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 - arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan - Timespan"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 - Arg2));
	}
	{
		FTimespan Arg1(2000);
		float Arg2 = 1000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 * arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan * Scalar"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 * Arg2));
	}
	{
		FTimespan Arg1(2000);
		float Arg2 = 1000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 / arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan / Scalar"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 / Arg2));
	}
	{
		FTimespan Arg1(1000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan == Timespan #1"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2));
	}
	{
		FTimespan Arg1(2000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1 == arg2")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan == Timespan #2"), FSUE4LuaFunction::CallGlobal<bool>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == (Arg1 == Arg2));
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetDays()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetDays()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetDays());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetDuration()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetDuration()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetDuration());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetHours()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetHours()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetHours());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMilliseconds()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetMilliseconds()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetFractionMilli());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetMinutes()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetMinutes()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetMinutes());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetSeconds()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetSeconds()"), FSUE4LuaFunction::CallGlobal<int32>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetSeconds());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetTotalDays()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetTotalDays()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetTotalDays());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetTotalHours()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetTotalHours()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetTotalHours());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetTotalMilliseconds()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetTotalMilliseconds()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetTotalMilliseconds());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetTotalMinutes()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetTotalMinutes()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetTotalMinutes());
	}
	{
		FTimespan Arg1(2000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:GetTotalSeconds()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.GetTotalSeconds()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.GetTotalSeconds());
	}
	{
		float Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Timespan.FromDays(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.FromDays()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == UKismetMathLibrary::FromDays(Arg1));
	}
	{
		float Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Timespan.FromHours(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.FromHours()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == UKismetMathLibrary::FromHours(Arg1));
	}
	{
		float Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Timespan.FromMilliseconds(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.FromMilliseconds()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == UKismetMathLibrary::FromMilliseconds(Arg1));
	}
	{
		float Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Timespan.FromMinutes(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.FromMinutes()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == UKismetMathLibrary::FromMinutes(Arg1));
	}
	{
		float Arg1 = 2000;

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return UE4.Timespan.FromSeconds(arg1)")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.FromSeconds()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test"), Arg1) == UKismetMathLibrary::FromSeconds(Arg1));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.Timespan.MaxValue()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.MaxValue()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test")) == FTimespan::MaxValue());
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.Timespan.MinValue()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.MinValue()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test")) == FTimespan::MinValue());
	}
	{
		FTimespan Arg1(2000);
		FTimespan Arg2(1000);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return UE4.Timespan.Ratio(arg1, arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.Ratio()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == FTimespan::Ratio(Arg1, Arg2));
	}
	{
		VM->ExecuteString(
			TEXT("\n	function Test()")
			TEXT("\n		return UE4.Timespan.ZeroValue()")
			TEXT("\n	end"));

		TestTrue(TEXT("Timespan.ZeroValue()"), FSUE4LuaFunction::CallGlobal<FTimespan>(VM.ToSharedRef(), TEXT("Test")) == FTimespan::Zero());
	}

	return true;
}


//=============================================================================================================================
// FSUE4LuaTestCaseMathQualifiedFrameTime
//=============================================================================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSUE4LuaTestCaseMathQualifiedFrameTime, "Silvervine.UE4Lua.MathQualifiedFrameTime", SUE4LUA_ATF_UNITTEST)
bool FSUE4LuaTestCaseMathQualifiedFrameTime::RunTest(const FString& Parameters)
{
	auto VM = FSUE4LuaVirtualMachine::Create();
	if (!VM.IsValid())
	{
		return false;
	}

	{
		FQualifiedFrameTime Arg1(FFrameTime(10, 0.1), FFrameRate(10, 10));

		VM->ExecuteString(
			TEXT("\n	function Test(arg1)")
			TEXT("\n		return arg1:AsSeconds()")
			TEXT("\n	end"));

		TestTrue(TEXT("QualifiedFrameTime.AsSeconds()"), FSUE4LuaFunction::CallGlobal<double>(VM.ToSharedRef(), TEXT("Test"), Arg1) == Arg1.AsSeconds());
	}
	{
		FQualifiedFrameTime Arg1(FFrameTime(10, 0.1), FFrameRate(10, 10));
		FFrameRate Arg2(20, 20);

		VM->ExecuteString(
			TEXT("\n	function Test(arg1, arg2)")
			TEXT("\n		return arg1:ConvertTo(arg2)")
			TEXT("\n	end"));

		TestTrue(TEXT("QualifiedFrameTime.ConvertTo()"), FSUE4LuaFunction::CallGlobal<FFrameTime>(VM.ToSharedRef(), TEXT("Test"), Arg1, Arg2) == Arg1.ConvertTo(Arg2));
	}

	return true;
}