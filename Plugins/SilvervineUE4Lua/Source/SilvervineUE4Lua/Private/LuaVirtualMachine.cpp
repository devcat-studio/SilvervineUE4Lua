// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#include "LuaVirtualMachine.h"

#include "LuaAPI.h"
#include "LuaCall.h"
#include "LuaLibrary.h"
#include "LuaLog.h"
#include "LuaFileLoader.h"
#include "LuaThread.h"
#include "LuaUObject.h"
#include "LuaUProperty.h"
#include "LuaUStruct.h"
#include "SilvervineUE4Lua.h"


//
// FSUE4LuaVirtualMachine의 내용
//
class FSUE4LuaVMContext
{
public:
	static void* LuaAlloc(void* Ud, void* Ptr, size_t OldSize, size_t NewSize)
	{
		if (NewSize != 0)
		{
			return FMemory::Realloc(Ptr, NewSize);
		}
		else
		{
			FMemory::Free(Ptr);
			return nullptr;
		}
	}

	static int32 LuaPanic(lua_State* L)
	{
		SUE4LVM_ERROR(L, TEXT("[PANIC] %s"), ANSI_TO_TCHAR(lua_tostring(L, 1)));

		return 0;
	}

	static FString FormatMessage(lua_State* L)
	{
		int ArgCount = lua_gettop(L);
		FString Message;

		for (int ArgIndex = 1; ArgIndex <= ArgCount; ++ArgIndex)
		{
			if (1 < ArgIndex)
			{
				Message += TEXT("\t");
			}

			lua_getglobal(L, "tostring");
			lua_pushvalue(L, ArgIndex);
			lua_pcall(L, 1, 1, 0);

			const char* StrPtr = lua_tostring(L, -1);
			if (StrPtr != nullptr)
			{
				Message += UTF8_TO_TCHAR(StrPtr);
			}
			else
			{
				Message += TEXT("?");
			}

			lua_pop(L, 1);
		}

		return Message;
	}

	static int32 LuaPrint(lua_State* L)
	{
		return LuaLog(L);
	}

	static int32 LuaLog(lua_State* L)
	{
		SUE4LVM_LOG(L, TEXT("%s"), *FormatMessage(L));

		return 0;
	}

	static int32 LuaWarning(lua_State* L)
	{
		SUE4LVM_WARNING(L, TEXT("%s"), *FormatMessage(L));

		return 0;
	}

	static int32 LuaError(lua_State* L)
	{
		SUE4LVM_ERROR(L, TEXT("%s"), *FormatMessage(L));

		return 0;
	}

	static int32 LuaErrorHandler(lua_State* L)
	{
		// #todo: 재귀호출 방지

		TArray<FString> Lines;
		FormatMessage(L).ParseIntoArrayLines(Lines);

		bool bFirstLine = true;
		for (const auto& Line : Lines)
		{
			if (bFirstLine)
			{
				SUE4LVM_ERROR(L, TEXT("FATAL: %s"), *Line);
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("       %s"), *Line);
			}
		}

		return 0;
	}

	static FString MakeChunkName(const TCHAR* Filename)
	{
		// 청크 이름이 '@'으로 시작하면 디버깅할 때 소스파일 경로가 조금 더 이쁘게 나옵니다.
		FString ChunkName = FString(TEXT("@")) + FSUE4LuaFileLoaderHelper::GetLocalFilePath(Filename);

		SUE4LuaLog::AddFilename(Filename, ChunkName);

		return ChunkName;
	}

	// VM이 초기화되기 전에 lua파일을 실행해야 할 때 사용됩니다.
	static bool RawExecuteFile(lua_State* L, const TCHAR* Filename)
	{
		FString Source;
		if (!FSUE4LuaFileLoaderHelper::LoadFileToString(Source, Filename))
		{
			UE_LOG(LogSUE4L, Error, TEXT("'%s' not found. [%s]"), Filename, TEXT(__FUNCTION__));
			return false;
		}

		bool bSuccess = false;

		FTCHARToUTF8 SourceUTF8(*Source);
		if (luaL_loadbuffer(L, SourceUTF8.Get(), SourceUTF8.Length(), TCHAR_TO_UTF8(*MakeChunkName(Filename))) == 0)
		{
			if (lua_pcall(L, 0, 0, 0) == 0)
			{
				bSuccess = true;
			}
			else
			{
				UE_LOG(LogSUE4L, Error, TEXT("lua_pcall() failed: %s [%s]"), ANSI_TO_TCHAR(lua_tostring(L, -1)), TEXT(__FUNCTION__));
			}
		}
		else
		{
			UE_LOG(LogSUE4L, Error, TEXT("luaL_loadbuffer() failed: %s [%s]"), ANSI_TO_TCHAR(lua_tostring(L, -1)), TEXT(__FUNCTION__));
		}

		return bSuccess;
	}

	FSUE4LuaVMContext()
	{
		LuaMainState = lua_newstate(LuaAlloc, nullptr);
		if (LuaMainState != nullptr)
		{
			// 여기서는 실패할 수 없는 가장 기본적인 초기화 몇 가지만 수행하고,
			// 나머지 복잡한 초기화는 FSUE4LuaVirtualMachine::Create()에서 합니다.
			{
				lua_atpanic(LuaMainState, &LuaPanic);
				luaL_openlibs(LuaMainState);

				lua_pushcfunction(LuaMainState, &LuaPrint);
				lua_setglobal(LuaMainState, "print");
			}

			LuaCurState = LuaMainState;

			FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddRaw(this, &FSUE4LuaVMContext::OnPreGarbageCollect);
			FSilvervineUE4LuaModule::Get().FileModifiedDelegate.AddRaw(this, &FSUE4LuaVMContext::OnFileModified);
		}
	}
	~FSUE4LuaVMContext()
	{
		Dispose();
	}

	void Dispose()
	{
		if (LuaMainState != nullptr)
		{
			SUE4LVM_LOG(LuaMainState, TEXT("Disposing '%p'..."), LuaMainState);
			
			FCoreUObjectDelegates::GetPreGarbageCollectDelegate().RemoveAll(this);
			FSilvervineUE4LuaModule::Get().FileModifiedDelegate.RemoveAll(this);

			if (LuaMainThread.IsValid())
			{
				LuaMainThread->SetDisposed();
			}

			LuaMainThread.Reset();
			CachedLuaFiles.Empty();

			lua_close(LuaMainState);
			LuaMainState = nullptr;
			LuaCurState = nullptr;

			UE_CLOG(0 < AllUserDatas.Num(), LogSUE4L, Error, TEXT("Found %d remaing UserDatas."), AllUserDatas.Num());
		}
	}

	void OnPreGarbageCollect()
	{
		if (LuaMainState == nullptr)
		{
			return;
		}

		auto L = LuaCurState;

		lua_gc(L, LUA_GCCOLLECT, 0);
	}

	void OnTick(float DeltaTime)
	{
		if (LuaMainState == nullptr)
		{
			return;
		}

		auto L = LuaCurState;

		lua_getglobal(L, "SUE4Lua");
		lua_getfield(L, -1, "OnTick");
		lua_remove(L, -2);
		// Stack: OnTick

		if (lua_isfunction(L, -1))
		{
			FSUE4LuaStack::Push(L, DeltaTime);

			SUE4LuaCall::PCall(L, 1, 0);
		}
		else
		{
			lua_pop(L, 1);
			// Stack: (empty)
		}
	}

	void OnFileModified(const FString& Filename)
	{
		if (LuaMainState == nullptr)
		{
			return;
		}

		auto L = LuaCurState;

		// 파일 캐시 갱신
		if (bDebuggable)
		{
			FName Name(*Filename);
			if (CachedLuaFiles.Contains(Name))
			{
				auto LuaFunction = LoadFile(L, *Filename, true);
				if (LuaFunction.IsValid())
				{
					CachedLuaFiles[Name] = LuaFunction;

					// @todo: BP가 걸리도록 디버거에 알려줘야 함
				}
			}
		}

		// SUE4LuaBinding.ReloadDispatchHandler()
		{
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "ReloadDispatchHandler");
			lua_remove(L, -2);

			FSUE4LuaStack::Push(L, Filename);

			SUE4LuaCall::PCall(L, 1, 0);
		}

		// SUE4Lua.OnFileModifiled()
		{
			lua_getglobal(L, "SUE4Lua");
			lua_getfield(L, -1, "OnFileModifiled");
			lua_remove(L, -2);
			// Stack: OnFileModifiled

			if (lua_isfunction(L, -1))
			{
				FSUE4LuaStack::Push(L, Filename);

				SUE4LuaCall::PCall(L, 1, 0);
			}
			else
			{
				lua_pop(L, 1);
				// Stack: (empty)
			}
		}
	}

	void SetWorldContextObject(UObject* InWorldContextObject)
	{
		if (LuaMainState == nullptr)
		{
			return;
		}

		WorldContextObject = InWorldContextObject;

		auto L = LuaCurState;

		// SUE4Lua.WorldContextObject = WorldContextObject
		{
			lua_getglobal(L, "SUE4Lua");
			FSUE4LuaStack::Push(L, InWorldContextObject);
			lua_setfield(L, -2, "WorldContextObject");
			lua_pop(L, 1);
		}
	}

	TSharedPtr<FSUE4LuaFunction> LoadBuffer(lua_State* L, const TCHAR* Source, const TCHAR* ChunkName)
	{
		if (LuaMainState == nullptr)
		{
			return nullptr;
		}

		TSharedPtr<FSUE4LuaFunction> RetFunction;

		if (Source != nullptr)
		{
			FTCHARToUTF8 SourceUTF8(Source);
			if (luaL_loadbuffer(L, SourceUTF8.Get(), SourceUTF8.Length(), TCHAR_TO_UTF8(ChunkName)) == 0)
			{
				RetFunction = FSUE4LuaFunction::CreateFromStack(L, -1);
			}
			else
			{
				SUE4LVM_ERROR(L, TEXT("%s"), ANSI_TO_TCHAR(lua_tostring(L, -1)));
			}

			lua_pop(L, 1);
		}

		return RetFunction;
	}

	TSharedPtr<FSUE4LuaFunction> LoadFile(lua_State* L, const TCHAR* Filename, bool bIgnoreCache = false)
	{
		if (LuaMainState == nullptr)
		{
			return nullptr;
		}

		if (bDebuggable && !bIgnoreCache)
		{
			if (auto Found = CachedLuaFiles.Find(FName(Filename)))
			{
				return *Found;
			}
		}

		FString LuaSource;
		if (FSUE4LuaFileLoaderHelper::LoadFileToString(LuaSource, Filename))
		{
			return LoadBuffer(L, *LuaSource, *MakeChunkName(Filename));
		}
		else
		{
			SUE4LVM_WARNING(L, TEXT("Failed to load file \"%s\""), Filename);
		}

		return nullptr;
	}

	void PushErrorHandler(lua_State* L)
	{
		lua_pushcfunction(L, &LuaErrorHandler);
	}

	void PopErrorHandler(lua_State* L)
	{
		if (lua_tocfunction(L, -1) == &LuaErrorHandler)
		{
			lua_pop(L, 1);
		}
		else
		{
			UE_LOG(LogSUE4L, Error, TEXT("Trying to pop invalid value as error handler. [%s]"), TEXT(__FUNCTION__));
		}
	}

	void LoadDispatchHandler(lua_State* L, UClass* DispatchClass, TSharedPtr<FSUE4LuaFunction> HandlerChunk, bool bForce)
	{
		if (LuaMainState == nullptr)
		{
			return;
		}

		if (DispatchClass == nullptr)
		{
			return;
		}

		const bool bAlreadyRegistered = DispatchClassNames.Find(DispatchClass->GetFName()) != nullptr;

		if (bAlreadyRegistered && !bForce)
		{
			// 이미 등록되어 있음
			return;
		}

		FString DispatchHandlerFilename;

		if (!HandlerChunk.IsValid())
		{
			if (!LuaDispatchHandlerFactory.IsValid())
			{
				SUE4LVM_ERROR(L, TEXT("LoadDispatchHandler(): Invalid DispatchHandlerFactory. Check if SetDispatchHandlerFactory() was called properly."));
				return;
			}

			DispatchHandlerFilename = LuaDispatchHandlerFactory->Call<FString>(DispatchClass);

			// 핸들러 로딩에 실패했어도 핫 리로딩으로 재연결될 수 있도록 리턴하지 않고 계속 진행
			if (!DispatchHandlerFilename.IsEmpty())
			{
				HandlerChunk = LoadFile(L, *DispatchHandlerFilename);

				if (!HandlerChunk.IsValid())
				{
					SUE4LVM_ERROR(L, TEXT("LoadDispatchHandler(): Failed to load dispatch handler: %s(%s)"), *DispatchClass->GetName(), *DispatchHandlerFilename);
				}
			}
		}

		if (!bAlreadyRegistered)
		{
			DispatchClassNames.Add(DispatchClass->GetFName());
		}

		// SUE4LuaBinding.RegisterDispatchHandler()
		{
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "RegisterDispatchHandler");
			lua_remove(L, -2);

			FSUE4LuaStack::Push(L, DispatchClass);
			FSUE4LuaStack::Push(L, HandlerChunk);
			FSUE4LuaStack::Push(L, DispatchHandlerFilename);

			SUE4LuaCall::PCall(L, 3, 0);
		}
	}

	bool PushDispatchHandler(lua_State* L, const UObject* DispatchObject, const FString& FunctionName)
	{
		if (DispatchObject == nullptr)
		{
			SUE4LVM_ERROR(L, TEXT("PushDispatchHandler(): DispatchObject is null."));
			return false;
		}

		// 호출 요청이 있을 때 로딩
		LoadDispatchHandler(L, DispatchObject->GetClass(), nullptr, false);

		// SUE4LuaBinding.FindDispatchHandler()
		{
			lua_getglobal(L, "SUE4LuaBinding");
			lua_getfield(L, -1, "FindDispatchHandler");
			lua_remove(L, -2);

			FSUE4LuaStack::Push(L, FunctionName);
			FSUE4LuaStack::Push(L, DispatchObject);

			if (!SUE4LuaCall::PCall(L, 2, 1))
			{
				return false;
			}
		}

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			return false;
		}

		return true;
	}

public:
	// 메인 스레드
	lua_State* LuaMainState = nullptr;
	// 현재 사용 중인 스레드. 코루틴을 사용할 경우 이 스레드는 메인 스레드가 아닐 수 있습니다.
	lua_State* LuaCurState = nullptr;
	// 메인 스레드 객체
	TSharedPtr<FSUE4LuaThread> LuaMainThread;
	// WorldContextObject
	TWeakObjectPtr<UObject> WorldContextObject;
	// 디스패치 핸들러를 반환하는 함수
	TSharedPtr<FSUE4LuaFunction> LuaDispatchHandlerFactory;
	// 디스패치를 사용하는 UClass의 이름들
	TSet<FName> DispatchClassNames;

public:
	//
	// 이하 디버깅 용
	//

	// 디버깅 가능한 VM
	bool bDebuggable = false;
	// 디버깅을 위해 미리 로딩된 lua 파일들
	TMap<FName, TSharedPtr<FSUE4LuaFunction>> CachedLuaFiles;
	// 생성된 유저데이터
	TSet<FSUE4LuaUserData*> AllUserDatas;
};

TSharedPtr<FSUE4LuaVirtualMachine> FSUE4LuaVirtualMachine::Create(bool bDebuggable)
{
	auto VM = TSharedPtr<FSUE4LuaVirtualMachine>(new FSUE4LuaVirtualMachine);

	// VM 필수 기능을 설치
	{
		auto L = VM->Context->LuaMainState;

		if (!FSUE4LuaVMContext::RawExecuteFile(L, TEXT("SUE4Lua/Framework/Prerequisite.lua")))
		{
			UE_LOG(LogSUE4L, Error, TEXT("VM is not initialized properly. [%s]"), TEXT(__FUNCTION__));

			VM.Reset();
		}
	}

	if (VM.IsValid())
	{
		auto L = VM->Context->LuaMainState;

		// 스레드 객체 생성
		VM->Context->LuaMainThread = FSUE4LuaThread::Create(VM.ToSharedRef(), L);

		// SUE4Lua 필드 설정
		{
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_setglobal(L, "SUE4Lua");
			// Stack: SUE4Lua

			// SUE4Lua.VM = VM
			{
				lua_pushlightuserdata(L, VM.Get());
				lua_setfield(L, -2, "VM");
			}

			// SUE4Lua.ErrorHandler = LuaErrorHandler
			{
				lua_pushcfunction(L, &FSUE4LuaVMContext::LuaErrorHandler);
				lua_setfield(L, -2, "ErrorHandler");
			}

			// SUE4Lua.Log = LuaLog
			{
				lua_pushcfunction(L, &FSUE4LuaVMContext::LuaLog);
				lua_setfield(L, -2, "Log");
			}
			// SUE4Lua.Warning = LuaWarning
			{
				lua_pushcfunction(L, &FSUE4LuaVMContext::LuaWarning);
				lua_setfield(L, -2, "Warning");
			}
			// SUE4Lua.Error = LuaError
			{
				lua_pushcfunction(L, &FSUE4LuaVMContext::LuaError);
				lua_setfield(L, -2, "Error");
			}

			// SUE4Lua.Build
			{
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "Build");

				// SUE4Lua.Build.WITH_EDITOR
				{
					lua_pushboolean(L, WITH_EDITOR);
					lua_setfield(L, -2, "WITH_EDITOR");
				}

				lua_pop(L, 1);
			}

			// Stack: SUE4Lua
			lua_pop(L, 1);
			check(lua_gettop(L) == 0);
			// Stack: (empty)
		}

		// 필수 lua 파일 실행
		{
			FSUE4LuaVMContext::RawExecuteFile(L, TEXT("SUE4Lua/Framework/SUE4Lua.lua"));
			FSUE4LuaVMContext::RawExecuteFile(L, TEXT("SUE4Lua/Framework/SUE4LuaBinding.lua"));
			FSUE4LuaVMContext::RawExecuteFile(L, TEXT("SUE4Lua/Framework/UE4.lua"));
			FSUE4LuaVMContext::RawExecuteFile(L, TEXT("SUE4Lua/Framework/DefaultParameters.lua"));
			FSUE4LuaVMContext::RawExecuteFile(L, TEXT("SUE4Lua/Framework/CodeGen/DefaultParameters.g.lua"));
		}

		// 타입 관련 초기화
		{
			SUE4LuaUObject::Register(L);
			SUE4LuaUProperty::Register(L);
			SUE4LuaUStruct::Register(L);
		}

		// c++ 라이브러리 등록
		{
			SUE4LuaLibrary::RegisterSUE4LuaLibrary(L);
			SUE4LuaLibrary::RegisterPlatformTimeLibrary(L);
			SUE4LuaLibrary::RegisterUE4Library(L);
			SUE4LuaLibrary::RegisterMathLibrary(L);
			SUE4LuaLibrary::RegisterVectorLibrary(L);
			SUE4LuaLibrary::RegisterVector2DLibrary(L);
			SUE4LuaLibrary::RegisterTransformLibrary(L);
			SUE4LuaLibrary::RegisterRotatorLibrary(L);
			SUE4LuaLibrary::RegisterQuatLibrary(L);
			SUE4LuaLibrary::RegisterColorLibrary(L);
			SUE4LuaLibrary::RegisterLinearColorLibrary(L);
			SUE4LuaLibrary::RegisterTextFormatterLibrary(L);
			SUE4LuaLibrary::RegisterDateTimeLibrary(L);
			SUE4LuaLibrary::RegisterTimespanLibrary(L);
			SUE4LuaLibrary::RegisterQualifiedFrameTimeLibrary(L);
			SUE4LuaLibrary::RegisterPointerEventLibrary(L);
		}

		// lua 라이브러리 등록
		VM->ExecuteFile(L, TEXT("SUE4Lua/Libraries/AllLibraries.lua"));
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// 디버깅 지원
	if (VM.IsValid() && bDebuggable)
	{
		lua_State* L = VM->Context->LuaMainState;

		// 브레이크포인트가 잡히려면 디버거가 붙기 전에 파일을 모두 로드해야 합니다.
		{
			TArray<FString> AllFilenames;
			AllFilenames.Append(FSUE4LuaFileLoaderHelper::GetFilenames(TEXT("SUE4Lua")));
			AllFilenames.Append(FSUE4LuaFileLoaderHelper::GetFilenames(TEXT("Game")));

			for (const auto& Filename : AllFilenames)
			{
				if (FPaths::GetExtension(Filename) == "lua")
				{
					FName Name(*Filename);
					if (!VM->Context->CachedLuaFiles.Contains(Name))
					{
						auto LuaFunction = VM->Context->LoadFile(L, *Filename, true);
						if (LuaFunction.IsValid())
						{
							VM->Context->CachedLuaFiles.Add(Name, LuaFunction);
						}
					}
					else
					{
						UE_LOG(LogSUE4L, Error, TEXT("'%s' file already exists. check the filename case sensitivity. [%s]"), *Filename, TEXT(__FUNCTION__));
					}
				}
			}
		}

		VM->Context->bDebuggable = true;
	}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	return VM;
}

TSharedPtr<FSUE4LuaVirtualMachine> FSUE4LuaVirtualMachine::PinVM(lua_State* L)
{
	if (L != nullptr)
	{
		if (lua_status(L) == LUA_YIELD)
		{
			UE_LOG(LogSUE4L, Error, TEXT("PinVM() was called in yield state."));
			return nullptr;
		}

		lua_getglobal(L, "SUE4Lua");
		lua_getfield(L, -1, "VM");
		lua_remove(L, -2);
		// Stack: VM

		auto VM = reinterpret_cast<FSUE4LuaVirtualMachine*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		// Stack: (empty)

		if (VM != nullptr && !VM->IsDisposed())
		{
			return VM->AsShared();
		}
		else
		{
			UE_LOG(LogSUE4L, Warning, TEXT("PinVM() was failed."));
		}
	}

	return nullptr;
}

FSUE4LuaVirtualMachine::FSUE4LuaVirtualMachine()
{
	Context = new FSUE4LuaVMContext();
}

FSUE4LuaVirtualMachine::~FSUE4LuaVirtualMachine()
{
	delete Context;
	Context = nullptr;
}

bool FSUE4LuaVirtualMachine::IsDisposed() const
{
	return Context->LuaMainState == nullptr;
}

bool FSUE4LuaVirtualMachine::IsDebuggable() const
{
	return Context->bDebuggable;
}

void FSUE4LuaVirtualMachine::Dispose()
{
	Context->Dispose();
}

UObject* FSUE4LuaVirtualMachine::GetWorldContextObject() const
{
	return Context->WorldContextObject.Get();
}

void FSUE4LuaVirtualMachine::SetWorldContextObject(UObject* WorldContextObject)
{
	Context->SetWorldContextObject(WorldContextObject);
}

bool FSUE4LuaVirtualMachine::ExecuteString(const TCHAR* Source)
{
	return ExecuteString(Context->LuaCurState, Source);
}

bool FSUE4LuaVirtualMachine::ExecuteFile(const TCHAR* Filename)
{
	return ExecuteFile(Context->LuaCurState, Filename);
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaVirtualMachine::LoadString(const TCHAR* Source)
{
	return LoadString(Context->LuaCurState, Source);
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaVirtualMachine::LoadFile(const TCHAR* Filename)
{
	return LoadFile(Context->LuaCurState, Filename);
}

TSharedPtr<FSUE4LuaThread> FSUE4LuaVirtualMachine::GetLuaMainThread() const
{
	return Context->LuaMainThread;
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaVirtualMachine::GetDispatchHandlerFactory() const
{
	return Context->LuaDispatchHandlerFactory;
}

void FSUE4LuaVirtualMachine::SetDispatchHandlerFactory(TSharedPtr<FSUE4LuaFunction> NewFactory)
{
	Context->LuaDispatchHandlerFactory = NewFactory;
}

void FSUE4LuaVirtualMachine::SetDispatchHandler(UClass* DispatchClass, TSharedPtr<FSUE4LuaFunction> HandlerChunk)
{
	if (DispatchClass != nullptr && HandlerChunk.IsValid())
	{
		Context->LoadDispatchHandler(Context->LuaCurState, DispatchClass, HandlerChunk, true);
	}
}

void FSUE4LuaVirtualMachine::Tick(float DeltaTime)
{
	Context->OnTick(DeltaTime);
}

bool FSUE4LuaVirtualMachine::ExecuteString(lua_State* L, const TCHAR* Source)
{
	auto LuaFunction = LoadString(L, Source);
	if (LuaFunction.IsValid())
	{
		LuaFunction->Call<void>();
		return true;
	}

	return false;
}

bool FSUE4LuaVirtualMachine::ExecuteFile(lua_State* L, const TCHAR* Filename)
{
	auto LuaFunction = LoadFile(L, Filename);
	if (LuaFunction.IsValid())
	{
		LuaFunction->Call<void>();
		return true;
	}

	return false;
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaVirtualMachine::LoadString(lua_State* L, const TCHAR* Source)
{
	return Context->LoadBuffer(L, Source, TEXT("(noname)"));
}

TSharedPtr<FSUE4LuaFunction> FSUE4LuaVirtualMachine::LoadFile(lua_State* L, const TCHAR* Filename)
{
	return Context->LoadFile(L, Filename);
}

lua_State* FSUE4LuaVirtualMachine::GetCurrentLuaState() const
{
	return Context->LuaCurState;
}

void FSUE4LuaVirtualMachine::SetCurrentLuaState(lua_State* L)
{
	if (!IsDisposed())
	{
		Context->LuaCurState = L;
	}
}

void FSUE4LuaVirtualMachine::PushErrorHandler(lua_State* L)
{
	Context->PushErrorHandler(L);
}

void FSUE4LuaVirtualMachine::PopErrorHandler(lua_State* L)
{
	Context->PopErrorHandler(L);
}

void FSUE4LuaVirtualMachine::AddUserData(FSUE4LuaUserData* UserData)
{
	if (UserData != nullptr)
	{
		bool bAlreadyExists = false;

		Context->AllUserDatas.Add(UserData, &bAlreadyExists);

		UE_CLOG(bAlreadyExists, LogSUE4L, Fatal, TEXT("Userdata already exists."));
	}
}

void FSUE4LuaVirtualMachine::RemoveUserData(FSUE4LuaUserData* UserData)
{
	if (UserData != nullptr)
	{
		if (Context->AllUserDatas.Remove(UserData) == 0)
		{
			UE_LOG(LogSUE4L, Fatal, TEXT("Userdata does not exists."));
		}
	}
}

bool FSUE4LuaVirtualMachine::PushDispatchHandler(lua_State* L, const UObject* DispatchObject, const FString& FunctionName)
{
	return Context->PushDispatchHandler(L, DispatchObject, FunctionName);
}