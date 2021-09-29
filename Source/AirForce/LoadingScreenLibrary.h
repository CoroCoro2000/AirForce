// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tickable.h"
#include "LoadingScreenLibrary.generated.h"

class UUserWidget;
class SWidget;
class URacingD_GameInstance;
class FObjectInitializer;

/**
 *  Core class
 */
class FLoadingScreenSystem : public FTickableGameObject
{
	TSharedPtr<SWidget>	LoadingScreenWidget;
	bool bShowing = false;
	URacingD_GameInstance* pGameInstance;
	double LastTickTime = 0.0;
	void OnAsyncLoadingFlushUpdate();
	FName PackageName;
	float Progress = 0.0f;
public:
	FLoadingScreenSystem(URacingD_GameInstance* InGameInstance);
	~FLoadingScreenSystem();
	virtual TStatId GetStatId() const  override{ RETURN_QUICK_DECLARE_CYCLE_STAT(FSimpleLoadingScreenSystem, STATGROUP_Tickables); }
	virtual void Tick(float DeltaTime) override;

	void SetWidget(TSharedPtr<SWidget> InWidget);
	void SetPackageNameForLoadingProgress(FName InPackageName) { PackageName = InPackageName; }
	float GetLoadingProgress();
	void ShowLoadingScreen();
	void HideLoadingScreen();
};


/**
 *		//Blueprint library
 */
UCLASS(meta = (ScriptName = "LoadingScreen"))
class AIRFORCE_API ULoadingScreenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintCallable)
	static void SetLoadingScreenWidget(UUserWidget* InWidget);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetTargetPackageForLoadingProgress(const UObject* WorldContextObject, FName InPackageName);

	/** 0~1 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static float GetLoadingProgress(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ShowSimpleLoadingScreen(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void HideSimpleLoadingScreen(const UObject* WorldContextObject);
};
