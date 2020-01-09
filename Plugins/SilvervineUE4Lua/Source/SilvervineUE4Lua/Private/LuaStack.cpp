// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2020. Nexon Korea Corporation. All rights reserved.

#include "LuaStack.h"

#include "Misc/QualifiedFrameTime.h"

#include "LuaAPI.h"
#include "LuaFunction.h"
#include "LuaLog.h"
#include "LuaThread.h"
#include "LuaUObject.h"
#include "LuaUProperty.h"
#include "LuaUStruct.h"
#include "SilvervineUE4LuaValue.h"


FSUE4LuaStackGuard::FSUE4LuaStackGuard(lua_State* InL)
	: L(InL)
{
	SavedTopIndex = lua_gettop(L);
}

FSUE4LuaStackGuard::~FSUE4LuaStackGuard()
{
	int32 CurTopIndex = lua_gettop(L);
	if (CurTopIndex != SavedTopIndex)
	{
		UE_LOG(LogSUE4L, Error, TEXT("Stack is not properly unwound. [%s]"), __SUE4LUA_FUNCTION__);

		lua_settop(L, SavedTopIndex);
	}
}

const FSUE4LuaStack::FNil FSUE4LuaStack::Nil;

void FSUE4LuaStack::LuaPushNil(lua_State* L)
{
	lua_pushnil(L);
}

void FSUE4LuaStack::LuaPop(lua_State* L, int32 N)
{
	lua_pop(L, N);
}

void FSUE4LuaStack::ToObject(lua_State*L, FNil&, int32 Index)
{
	if (!lua_isnil(L, Index))
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected nil."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushInt64(lua_State* L, int64 InValue)
{
	lua_pushinteger(L, InValue);
}

int64 FSUE4LuaStack::ToInt64(lua_State* L, int32 Index)
{
	if (lua_isinteger(L, Index))
	{
		return lua_tointeger(L, Index);
	}
	else if (lua_isnumber(L, Index))
	{
		// lua_tointeger()는 float를 int로 캐스팅할 때 소수부(소수점이하)가 0이 아니면 캐스팅 결과로 0을 반환하므로 주의
		return static_cast<int64>(lua_tonumber(L, Index));
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected integer type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		return 0;
	}
}

void FSUE4LuaStack::PushDouble(lua_State* L, double InValue)
{
	lua_pushnumber(L, InValue);
}

double FSUE4LuaStack::ToDouble(lua_State* L, int32 Index)
{
	if (lua_isnumber(L, Index))
	{
		return lua_tonumber(L, Index);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected number type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		return 0.0;
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, bool InValue)
{
	lua_pushboolean(L, InValue);
}

void FSUE4LuaStack::ToObject(lua_State* L, bool& OutValue, int32 Index)
{
	OutValue = !!lua_toboolean(L, Index);
}

void FSUE4LuaStack::PushObject(lua_State* L, const FName InValue)
{
	// NAME_None을 lua로 전달할 때, "None"이 아니라 ""를 사용 
	PushObject(L, InValue.IsNone() ? FString() : InValue.ToString());
}

void FSUE4LuaStack::ToObject(lua_State* L, FName& OutValue, int32 Index)
{
	auto Str = ToStringTCHAR(L, Index);
	OutValue = Str.IsEmpty() ? NAME_None : FName(*Str);
}

void FSUE4LuaStack::PushStringUTF8(lua_State* L, const char* InValue)
{
	lua_pushstring(L, InValue);
}

void FSUE4LuaStack::PushStringTCHAR(lua_State* L, const TCHAR* InValue)
{
	PushStringUTF8(L, TCHAR_TO_UTF8(InValue));
}

FString FSUE4LuaStack::ToStringTCHAR(lua_State* L, int32 Index)
{
	if (lua_isstring(L, Index))
	{
		return FString(UTF8_TO_TCHAR(lua_tostring(L, Index)));
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected string type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		return FString();
	}
}

bool FSUE4LuaStack::IsPrimitiveStructTypeOf(lua_State* L, int32 Index, const char* TypeName)
{
	bool bRet = false;

	if (lua_istable(L, Index))
	{
		lua_getmetatable(L, Index);
		// Stack: MT

		if (lua_istable(L, -1))
		{
			lua_getglobal(L, "UE4");
			lua_getfield(L, -1, TypeName);
			lua_getfield(L, -1, "metaTable");
			// Stack: MT, UE4, TypeName, metaTable

			if (!!lua_compare(L, -4, -1, LUA_OPEQ))
			{
				bRet = true;
			}

			lua_pop(L, 3);
			// Stack: MT			
		}

		lua_pop(L, 1);
		// Stack: (empty)
	}

	return bRet;
}

void FSUE4LuaStack::PushObject(lua_State* L, const FVector& InValue)
{
	// UE4.Vector.new(x, y, z)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Vector");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.X);
		lua_pushnumber(L, InValue.Y);
		lua_pushnumber(L, InValue.Z);

		lua_pcall(L, 3, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FVector& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Vector"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<float>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<float>(L);

		lua_getfield(L, Index, "Z");
		OutValue.Z = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Vector type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FVector(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FVector2D& InValue)
{
	// UE4.Vector2D.new(x, y)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Vector2D");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.X);
		lua_pushnumber(L, InValue.Y);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FVector2D& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Vector2D"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<float>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Vector2D type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FVector2D(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FVector4& InValue)
{
	// UE4.Vector4.new(x, y)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Vector4");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.X);
		lua_pushnumber(L, InValue.Y);
		lua_pushnumber(L, InValue.Z);
		lua_pushnumber(L, InValue.W);

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FVector4& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Vector4"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<float>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<float>(L);

		lua_getfield(L, Index, "Z");
		OutValue.Z = Pop<float>(L);

		lua_getfield(L, Index, "W");
		OutValue.W = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Vector4 type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FVector4(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FIntPoint& InValue)
{
	// UE4.IntPoint.new(x, y)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "IntPoint");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.X);
		lua_pushinteger(L, InValue.Y);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FIntPoint& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "IntPoint"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<int32>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.IntPoint type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FIntPoint(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FIntVector& InValue)
{
	// UE4.IntVector.new(x, y, z)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "IntVector");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.X);
		lua_pushinteger(L, InValue.Y);
		lua_pushinteger(L, InValue.Z);

		lua_pcall(L, 3, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FIntVector& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "IntVector"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<int32>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<int32>(L);

		lua_getfield(L, Index, "Z");
		OutValue.Z = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.IntVector type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FIntVector(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FPlane& InValue)
{
	// UE4.Plane.new(x, y)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Plane");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.X);
		lua_pushnumber(L, InValue.Y);
		lua_pushnumber(L, InValue.Z);
		lua_pushnumber(L, InValue.W);

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FPlane& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Plane"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<float>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<float>(L);

		lua_getfield(L, Index, "Z");
		OutValue.Z = Pop<float>(L);

		lua_getfield(L, Index, "W");
		OutValue.W = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Plane type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FPlane(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FMatrix& InValue)
{
	// UE4.Matrix.new(x, y, z, w)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Matrix");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, FPlane(InValue.M[0][0], InValue.M[0][1], InValue.M[0][2], InValue.M[0][3]));
		Push(L, FPlane(InValue.M[1][0], InValue.M[1][1], InValue.M[1][2], InValue.M[1][3]));
		Push(L, FPlane(InValue.M[2][0], InValue.M[2][1], InValue.M[2][2], InValue.M[2][3]));
		Push(L, FPlane(InValue.M[3][0], InValue.M[3][1], InValue.M[3][2], InValue.M[3][3]));

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FMatrix& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Matrix"))
	{
		lua_getfield(L, Index, "XPlane");
		auto XPlane = Pop<FPlane>(L);

		lua_getfield(L, Index, "YPlane");
		auto YPlane = Pop<FPlane>(L);

		lua_getfield(L, Index, "ZPlane");
		auto ZPlane = Pop<FPlane>(L);

		lua_getfield(L, Index, "WPlane");
		auto WPlane = Pop<FPlane>(L);

		OutValue = FMatrix(XPlane, YPlane, ZPlane, WPlane);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Matrix type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FMatrix(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FRotator& InValue)
{
	// UE4.Rotator.new(pitch, yaw, roll)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Rotator");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.Pitch);
		lua_pushnumber(L, InValue.Yaw);
		lua_pushnumber(L, InValue.Roll);

		lua_pcall(L, 3, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FRotator& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Rotator"))
	{
		lua_getfield(L, Index, "Pitch");
		OutValue.Pitch = Pop<float>(L);

		lua_getfield(L, Index, "Yaw");
		OutValue.Yaw = Pop<float>(L);

		lua_getfield(L, Index, "Roll");
		OutValue.Roll = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Rotator type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FRotator(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FQuat& InValue)
{
	// UE4.Quat.new(x, y, z, w)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Quat");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.X);
		lua_pushnumber(L, InValue.Y);
		lua_pushnumber(L, InValue.Z);
		lua_pushnumber(L, InValue.W);

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FQuat& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Quat"))
	{
		lua_getfield(L, Index, "X");
		OutValue.X = Pop<float>(L);

		lua_getfield(L, Index, "Y");
		OutValue.Y = Pop<float>(L);

		lua_getfield(L, Index, "Z");
		OutValue.Z = Pop<float>(L);

		lua_getfield(L, Index, "W");
		OutValue.W = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Quat type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FQuat(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FLinearColor& InValue)
{
	// UE4.LinearColor.new(r, g, b, a)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "LinearColor");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushnumber(L, InValue.R);
		lua_pushnumber(L, InValue.G);
		lua_pushnumber(L, InValue.B);
		lua_pushnumber(L, InValue.A);

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FLinearColor& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "LinearColor"))
	{
		lua_getfield(L, Index, "R");
		OutValue.R = Pop<float>(L);

		lua_getfield(L, Index, "G");
		OutValue.G = Pop<float>(L);

		lua_getfield(L, Index, "B");
		OutValue.B = Pop<float>(L);

		lua_getfield(L, Index, "A");
		OutValue.A = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.LinearColor type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FLinearColor(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FColor& InValue)
{
	// UE4.Color.new(r, g, b, a)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Color");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.R);
		lua_pushinteger(L, InValue.G);
		lua_pushinteger(L, InValue.B);
		lua_pushinteger(L, InValue.A);

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FColor& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Color"))
	{
		lua_getfield(L, Index, "R");
		OutValue.R = Pop<int32>(L);

		lua_getfield(L, Index, "G");
		OutValue.G = Pop<int32>(L);

		lua_getfield(L, Index, "B");
		OutValue.B = Pop<int32>(L);

		lua_getfield(L, Index, "A");
		OutValue.A = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Color type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FColor(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FTransform& InValue)
{
	// UE4.Transform.new(r, t, s)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Transform");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		PushObject(L, InValue.GetRotation());
		PushObject(L, InValue.GetTranslation());
		PushObject(L, InValue.GetScale3D());

		lua_pcall(L, 3, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FTransform& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Transform"))
	{		
		lua_getfield(L, Index, "Rotation");
		OutValue.SetRotation(Pop<FQuat>(L));

		lua_getfield(L, Index, "Translation");
		OutValue.SetTranslation(Pop<FVector>(L));

		lua_getfield(L, Index, "Scale3D");
		OutValue.SetScale3D(Pop<FVector>(L));
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Transform type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FBox& InValue)
{
	// UE4.Box.new(min, max)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Box");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		PushObject(L, InValue.Min);
		PushObject(L, InValue.Max);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FBox& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Box"))
	{
		lua_getfield(L, Index, "Min");
		auto Min = Pop<FVector>(L);

		lua_getfield(L, Index, "Max");
		auto Max = Pop<FVector>(L);

		OutValue = FBox(Min, Max);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Box type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FBox(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FBox2D& InValue)
{
	// UE4.Box2D.new(min, max)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Box2D");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		PushObject(L, InValue.Min);
		PushObject(L, InValue.Max);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FBox2D& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Box2D"))
	{
		lua_getfield(L, Index, "Min");
		auto Min = Pop<FVector2D>(L);

		lua_getfield(L, Index, "Max");
		auto Max = Pop<FVector2D>(L);

		OutValue = FBox2D(Min, Max);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Box2D type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FBox2D(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FBoxSphereBounds& InValue)
{
	// UE4.BoxSphereBounds.new(origin, extent, radius)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "BoxSphereBounds");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		PushObject(L, InValue.Origin);
		PushObject(L, InValue.BoxExtent);
		lua_pushnumber(L, InValue.SphereRadius);

		lua_pcall(L, 3, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FBoxSphereBounds& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "BoxSphereBounds"))
	{
		lua_getfield(L, Index, "Origin");
		OutValue.Origin = Pop<FVector>(L);

		lua_getfield(L, Index, "BoxExtent");
		OutValue.BoxExtent = Pop<FVector>(L);

		lua_getfield(L, Index, "SphereRadius");
		OutValue.SphereRadius = Pop<float>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.BoxSphereBounds type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = FBoxSphereBounds(ForceInit);
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FGuid& InValue)
{
	// UE4.Guid.new(a, b, c, d)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Guid");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.A);
		lua_pushinteger(L, InValue.B);
		lua_pushinteger(L, InValue.C);
		lua_pushinteger(L, InValue.D);

		lua_pcall(L, 4, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FGuid& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Guid"))
	{
		lua_getfield(L, Index, "A");
		OutValue.A = Pop<int32>(L);

		lua_getfield(L, Index, "B");
		OutValue.B = Pop<int32>(L);

		lua_getfield(L, Index, "C");
		OutValue.C = Pop<int32>(L);

		lua_getfield(L, Index, "D");
		OutValue.D = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Guid type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FDateTime& InValue)
{
	// UE4.DateTime.new(ticks)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "DateTime");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.GetTicks());

		lua_pcall(L, 1, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FDateTime& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "DateTime"))
	{
		lua_getfield(L, Index, "Ticks");
		OutValue = Pop<int64>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.DateTime type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = 0;
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FTimecode& InValue)
{
	// UE4.Timecode.new(hours, minutes, seconds, frames, dropFrameFormat)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Timecode");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.Hours);
		lua_pushinteger(L, InValue.Minutes);
		lua_pushinteger(L, InValue.Seconds);
		lua_pushinteger(L, InValue.Frames);
		lua_pushinteger(L, InValue.bDropFrameFormat);

		lua_pcall(L, 5, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FTimecode& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Timecode"))
	{
		lua_getfield(L, Index, "Hours");
		OutValue.Hours = Pop<int32>(L);

		lua_getfield(L, Index, "Minutes");
		OutValue.Minutes = Pop<int32>(L);

		lua_getfield(L, Index, "Seconds");
		OutValue.Seconds = Pop<int32>(L);

		lua_getfield(L, Index, "Frames");
		OutValue.Frames = Pop<int32>(L);

		lua_getfield(L, Index, "bDropFrameFormat");
		OutValue.bDropFrameFormat = Pop<bool>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Timecode type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FTimespan& InValue)
{
	// UE4.Timespan.new(ticks)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Timespan");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.GetTicks());

		lua_pcall(L, 1, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FTimespan& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Timespan"))
	{
		lua_getfield(L, Index, "Ticks");
		OutValue = Pop<int64>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Timespan type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));

		OutValue = 0;
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FRandomStream& InValue)
{
	// UE4.RandomStream.new(seed)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "RandomStream");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.GetInitialSeed());

		lua_pcall(L, 1, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FRandomStream& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "RandomStream"))
	{
		lua_getfield(L, Index, "Seed");
		auto Seed = Pop<int32>(L);

		OutValue = FRandomStream(Seed);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.RandomStream type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FFrameRate& InValue)
{
	// UE4.FrameRate.new(numerator, denominator)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "FrameRate");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.Numerator);
		lua_pushinteger(L, InValue.Denominator);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FFrameRate& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "FrameRate"))
	{
		lua_getfield(L, Index, "Numerator");
		OutValue.Numerator = Pop<int32>(L);

		lua_getfield(L, Index, "Denominator");
		OutValue.Denominator = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.FrameRate type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FFrameNumber& InValue)
{
	// UE4.FrameNumber.new(value)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "FrameNumber");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		lua_pushinteger(L, InValue.Value);

		lua_pcall(L, 1, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FFrameNumber& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "FrameNumber"))
	{
		lua_getfield(L, Index, "Value");
		OutValue.Value = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.FrameNumber type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FFrameTime& InValue)
{
	// UE4.FrameTime.new(frame, subFrame)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "FrameTime");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.GetFrame());
		lua_pushnumber(L, InValue.GetSubFrame());

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FFrameTime& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "FrameTime"))
	{
		lua_getfield(L, Index, "Frame");
		auto Frame = Pop<FFrameNumber>(L);

		lua_getfield(L, Index, "SubFrame");
		auto SubFrame = Pop<float>(L);

		OutValue = FFrameTime(Frame, SubFrame);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.FrameTime type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FQualifiedFrameTime& InValue)
{
	// UE4.QualifiedFrameTime.new(time, rate)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "QualifiedFrameTime");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.Time);
		Push(L, InValue.Rate);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FQualifiedFrameTime& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "QualifiedFrameTime"))
	{
		lua_getfield(L, Index, "Time");
		OutValue.Time = Pop<FFrameTime>(L);

		lua_getfield(L, Index, "Rate");
		OutValue.Rate = Pop<FFrameRate>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.QualifiedFrameTime type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FPrimaryAssetType& InValue)
{
	// UE4.PrimaryAssetType.new(name)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "PrimaryAssetType");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.GetName());

		lua_pcall(L, 1, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FPrimaryAssetType& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "PrimaryAssetType"))
	{
		lua_getfield(L, Index, "Name");
		OutValue = Pop<FName>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.PrimaryAssetType type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FPrimaryAssetId& InValue)
{
	// UE4.PrimaryAssetId.new(primaryAssetType, primaryAssetName)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "PrimaryAssetId");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.PrimaryAssetType);
		Push(L, InValue.PrimaryAssetName);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FPrimaryAssetId& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "PrimaryAssetId"))
	{
		lua_getfield(L, Index, "PrimaryAssetType");
		OutValue.PrimaryAssetType = Pop<FPrimaryAssetType>(L);

		lua_getfield(L, Index, "PrimaryAssetName");
		OutValue.PrimaryAssetName = Pop<FName>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.PrimaryAssetId type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FSoftObjectPath& InValue)
{
	// UE4.SoftObjectPath.new(assetPathName, subPathString)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "SoftObjectPath");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.GetAssetPathName());
		Push(L, InValue.GetSubPathString());

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FSoftObjectPath& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "SoftObjectPath"))
	{
		lua_getfield(L, Index, "AssetPathName");
		auto AssetPathName = Pop<FName>(L);

		lua_getfield(L, Index, "SubPathString");
		auto SubPathString = Pop<FString>(L);

		OutValue = FSoftObjectPath(AssetPathName, SubPathString);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.SoftObjectPath type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FFloatRangeBound& InValue)
{
	// UE4.FloatRangeBound.new(type, value)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "FloatRangeBound");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		if (InValue.IsOpen())
		{
			Push(L, ERangeBoundTypes::Open);
			Push(L, 0.0f);
		}
		else if (InValue.IsExclusive())
		{
			Push(L, ERangeBoundTypes::Exclusive);
			Push(L, InValue.GetValue());
		}
		else
		{
			check(InValue.IsInclusive());
			Push(L, ERangeBoundTypes::Inclusive);
			Push(L, InValue.GetValue());
		}

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FFloatRangeBound& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "FloatRangeBound"))
	{
		lua_getfield(L, Index, "Type");
		auto Type = Pop<ERangeBoundTypes::Type>(L);

		lua_getfield(L, Index, "Value");
		auto Value = Pop<float>(L);

		if (Type == ERangeBoundTypes::Open)
		{
			OutValue = FFloatRangeBound::Open();
		}
		else if (Type == ERangeBoundTypes::Exclusive)
		{
			OutValue = FFloatRangeBound::Exclusive(Value);
		}
		else
		{
			check(Type == ERangeBoundTypes::Inclusive);
			OutValue = FFloatRangeBound::Inclusive(Value);
		}
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.FloatRangeBound type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FFloatRange& InValue)
{
	// UE4.FloatRange.new(lowerBound, upperBound)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "FloatRange");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.GetLowerBound());
		Push(L, InValue.GetUpperBound());

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FFloatRange& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "FloatRange"))
	{
		lua_getfield(L, Index, "LowerBound");
		auto LowerBound = Pop<FFloatRangeBound>(L);

		lua_getfield(L, Index, "UpperBound");
		auto UpperBound = Pop<FFloatRangeBound>(L);

		OutValue = FFloatRange(LowerBound, UpperBound);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.FloatRange type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

void FSUE4LuaStack::PushObject(lua_State* L, const FInt32Interval& InValue)
{
	// UE4.Int32Interval.new(min, max)
	{
		lua_getglobal(L, "UE4");
		lua_getfield(L, -1, "Int32Interval");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);
		lua_remove(L, -2);

		Push(L, InValue.Min);
		Push(L, InValue.Max);

		lua_pcall(L, 2, 1, 0);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, FInt32Interval& OutValue, int32 Index)
{
	if (IsPrimitiveStructTypeOf(L, Index, "Int32Interval"))
	{
		lua_getfield(L, Index, "Min");
		OutValue.Min = Pop<int32>(L);

		lua_getfield(L, Index, "Max");
		OutValue.Max = Pop<int32>(L);
	}
	else
	{
		SUE4LVM_WARNING(L, TEXT("%s: expected UE4.Int32Interval type."), ANSI_TO_TCHAR(lua_typename(L, lua_type(L, Index))));
	}
}

const UScriptStruct* FSUE4LuaStack::GetCoreUObjectUStructStaticClass(const TCHAR* ClassName)
{
	static UPackage* CoreUObjectPkg = FindObjectChecked<UPackage>(nullptr, TEXT("/Script/CoreUObject"));

	return FindObject<UScriptStruct>(CoreUObjectPkg, ClassName);
}

void FSUE4LuaStack::PushObject(lua_State* L, TSharedPtr<FSUE4LuaFunction> InValue)
{
	if (InValue.IsValid())
	{
		InValue->Push(L);
	}
	else
	{
		lua_pushnil(L);
	}
}

void FSUE4LuaStack::ToObject(lua_State* L, TSharedPtr<FSUE4LuaFunction>& OutValue, int32 Index)
{
	OutValue = FSUE4LuaFunction::CreateFromStack(L, Index);
}

void FSUE4LuaStack::PushObject(lua_State* L, const UObject* InValue)
{
	auto LuaValue = Cast<const USUE4LuaValue>(InValue);
	if (LuaValue != nullptr)
	{
		LuaValue->Push(L);
		return;
	}
	
	SUE4LuaUObject::PushRef(L, const_cast<UObject*>(InValue));
}

UObject* FSUE4LuaStack::ToUObject(lua_State* L, int32 Index, UClass* Class)
{
	check(Class != nullptr);

	if (Class->IsChildOf<USUE4LuaValue>())
	{
		return USUE4LuaValue::CreateFromStack(L, Index);
	}

	return SUE4LuaUObject::ToRef(L, Index);
}

void FSUE4LuaStack::PushUStruct(lua_State* L, const UScriptStruct* Struct, const void* Src)
{
	SUE4LuaUStruct::PushValue(L, Struct, Src);
}

void FSUE4LuaStack::ToUStruct(lua_State* L, int32 Index, const UScriptStruct* Struct, void* Dest)
{
	SUE4LuaUStruct::CopyToNative(L, Index, Struct, Dest);
}

void* FSUE4LuaStack::GetUStructRef(lua_State* L, int32 Index, const UScriptStruct* Struct)
{
	return SUE4LuaUStruct::GetRef(L, Index, Struct);
}