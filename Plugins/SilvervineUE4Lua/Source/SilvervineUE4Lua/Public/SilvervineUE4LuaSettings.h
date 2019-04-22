// SilvervineUE4Lua / devCAT studio
// Copyright 2016 - 2019. Nexon Korea Corporation. All rights reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

#include "SilvervineUE4LuaSettings.generated.h"


//
// SUE4Lua 설정 파일 클래스
//
UCLASS(config = SilvervineUE4Lua)
class SILVERVINEUE4LUA_API USUE4LuaSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const USUE4LuaSettings* Get();

	// 번들 파일 로더를 사용
	UPROPERTY(config, EditAnywhere, Category = FileLoader)
	bool bUseBundleFileLoader = false;

	// 번들 파일 로더의 가상 경로 매핑
	UPROPERTY(config, EditAnywhere, Category = FileLoader, meta = (EditCondition = "bUseBundleFileLoader"))
	TMap<FString, FString> BundleFileLoadMap;

	// 로컬 파일 로더의 가상 경로 매핑
	UPROPERTY(config, EditAnywhere, Category = FileLoader, meta = (EditCondition = "!bUseBundleFileLoader"))
	TMap<FString, FString> LocalFileLoadMap;

	// 스태틱 바인딩을 사용
	UPROPERTY(config, EditAnywhere, Category = Binding)
	bool bEnableStaticBinding = true;

	void SetFileLoader() const;

public:
	// Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
	// End UDeveloperSettings Interface
private:
	// Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End UObject Interface
#endif	// WITH_EDITOR
};