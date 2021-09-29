// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenLibrary.h"
#include "RacingD_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreaming.h"
#include "UnrealEngine.h"
#include "UObject/UObjectGlobals.h"
#include "Framework/Application/SlateApplication.h"

DEFINE_LOG_CATEGORY_STATIC(LogSimpleLoadingScreen, Log, All);

FLoadingScreenSystem::FLoadingScreenSystem(URacingD_GameInstance* InGameInstance)
	: pGameInstance(InGameInstance)
{
	FCoreDelegates::OnAsyncLoadingFlushUpdate.AddRaw(this, &FLoadingScreenSystem::OnAsyncLoadingFlushUpdate);
}

void FLoadingScreenSystem::SetWidget(TSharedPtr<SWidget> InWidget)
{
	LoadingScreenWidget = InWidget;
}

FLoadingScreenSystem::~FLoadingScreenSystem()
{
}

void FLoadingScreenSystem::Tick(float DeltaTime)
{

}

float FLoadingScreenSystem::GetLoadingProgress()
{
	float Sum = 0.f;
	int32 PackageNum = 1;
	UPackage* PersistentLevelPackage = FindObjectFast<UPackage>(NULL, PackageName);
	if (PersistentLevelPackage && (PersistentLevelPackage->GetLoadTime() > 0))
	{
		Sum += 100.0f;
	}
	else
	{
		const float LevelLoadPercentage = GetAsyncLoadPercentage(PackageName);
		if (LevelLoadPercentage >= 0.0f)
		{
			Sum += LevelLoadPercentage;
		}
	}

	if (PersistentLevelPackage)
	{
		UWorld* World = UWorld::FindWorldInPackage(PersistentLevelPackage);
		TArray<FName>	PackageNames;
		PackageNames.Reserve(World->GetStreamingLevels().Num());
		for (ULevelStreaming* LevelStreaming : World->GetStreamingLevels())
		{
			if (LevelStreaming
				&& !LevelStreaming->GetWorldAsset().IsNull()
				&& LevelStreaming->GetWorldAsset() != World)
			{
				PackageNames.Add(LevelStreaming->GetWorldAssetPackageFName());
			}
		}
		for (FName& LevelName : PackageNames)
		{
			PackageNum++;
			UPackage* LevelPackage = FindObjectFast<UPackage>(NULL, LevelName);

			if (LevelPackage && (LevelPackage->GetLoadTime() > 0))
			{
				Sum += 100.0f;
			}
			else
			{
				const float LevelLoadPercentage = GetAsyncLoadPercentage(LevelName);
				if (LevelLoadPercentage >= 0.0f)
				{
					Sum += 100.0f;
				}
			}
		}
	}

	float Current = Sum / PackageNum;
	Progress = Current * 0.05f + Progress * 0.95f;
	UE_LOG(LogSimpleLoadingScreen, Verbose, TEXT("%6.3f %6.3f SubLevels %d"), Progress, Current, PackageNum);
	return Progress / 100.0f;
}

/*このデリゲート関数はロード中に高頻度で呼ばれるので、適切な間隔でスレートの更新を呼ぶようにする*/
void FLoadingScreenSystem::OnAsyncLoadingFlushUpdate()
{
	check(IsInGameThread());

	QUICK_SCOPE_CYCLE_COUNTER(STAT_LoadingScreenManager_OnAsyncLoadingFlushUpdate);

	const double CurrentTime = FPlatformTime::Seconds();
	const double DeltaTime = CurrentTime - LastTickTime;
	if (DeltaTime > 1.0f / 60.0f)
	{
		LastTickTime = CurrentTime;
		if (bShowing) {
			// スレート更新
			FSlateApplication::Get().Tick();

			{
				TGuardValue<int32> DisableAsyncLoadDuringSync(GDoAsyncLoadingWhileWaitingForVSync, 0);
				FSlateApplication::Get().GetRenderer()->Sync();
			}
		}

		LastTickTime = CurrentTime;
	}
}

void FLoadingScreenSystem::ShowLoadingScreen()
{
	if (bShowing)
	{
		return;
	}

	bShowing = true;
	Progress = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("Show loading screen"));
	UE_LOG(LogSimpleLoadingScreen, Log, TEXT("Show loading screen"));

	if (pGameInstance)
	{
		UGameViewportClient* GameViewportClient = pGameInstance->GetGameViewportClient();
		if (GameViewportClient)
		{
			const int32 ZOrder = 10000;
			GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), ZOrder);

			// Widgetが表示されているはずなので3D描画を完全にカットする
			GameViewportClient->bDisableWorldRendering = true;
			if (!GIsEditor)
			{
				FSlateApplication::Get().Tick();
			}
		}
	}
}

void FLoadingScreenSystem::HideLoadingScreen()
{
	if (!bShowing)
	{
		return;
	}

	UE_LOG(LogSimpleLoadingScreen, Log, TEXT("Hide loading screen"));
	GEngine->ForceGarbageCollection(true);

	if (pGameInstance)
	{
		UGameViewportClient* GameViewportClient = pGameInstance->GetGameViewportClient();
		if (GameViewportClient && LoadingScreenWidget.IsValid())
		{
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}

		if (GameViewportClient)
		{
			GameViewportClient->bDisableWorldRendering = false;
		}
	}
	bShowing = false;
}

ULoadingScreenLibrary::ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ULoadingScreenLibrary::SetLoadingScreenWidget(UUserWidget* InWidget)
{
	UWorld* World = GEngine->GetWorldFromContextObject(InWidget, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) { return; }
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	TSharedRef<SWidget> TakenWidget = InWidget->TakeWidget();
	GameInstance->pLoadingScreenSystem->SetWidget(TakenWidget);
}

void ULoadingScreenLibrary::SetTargetPackageForLoadingProgress(const UObject* WorldContextObject, FName InPackageName)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) { return; }
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	return GameInstance->pLoadingScreenSystem->SetPackageNameForLoadingProgress(InPackageName);
}

float ULoadingScreenLibrary::GetLoadingProgress(const UObject* WorldContextObject)
{
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(WorldContextObject->GetOuter());
	if (GameInstance == nullptr)	//Try to get a gameinstance through a world.
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World == nullptr) { return 0; }
		GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
		if (GameInstance == nullptr) { return 0; }
	}
	return GameInstance->pLoadingScreenSystem->GetLoadingProgress();
}

void ULoadingScreenLibrary::ShowSimpleLoadingScreen(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) { return; }
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	GameInstance->pLoadingScreenSystem->ShowLoadingScreen();
}

void ULoadingScreenLibrary::HideSimpleLoadingScreen(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) { return; }
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	GameInstance->pLoadingScreenSystem->HideLoadingScreen();
}
