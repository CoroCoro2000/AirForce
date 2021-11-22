// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenLibrary.h"
#include "RacingD_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SWidget.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Framework/Application/SlateApplication.h"
#include "UnrealEngine.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreaming.h"
#include "Engine/GameViewportClient.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

//コンストラクタ
FLoadingScreenSystem::FLoadingScreenSystem(URacingD_GameInstance* InGameInstance)
	: m_pGameInstance(InGameInstance)
	, m_pLoadingScreenWidget()
	, m_LastTickTime(0.0)
	, m_PackageName(TEXT(""))
	, m_Progress(0.f)
	, m_AsyncLoadingFlushUpdateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
{
	//ロード中のスレート更新をする関数をバインド
	m_AsyncLoadingFlushUpdateHandle = FCoreDelegates::OnAsyncLoadingFlushUpdate.AddRaw(this, &FLoadingScreenSystem::OnAsyncLoadingFlushUpdate);
}

//デストラクタ
FLoadingScreenSystem::~FLoadingScreenSystem()
{
	//ロード中のスレート更新をする関数のバインドを解除
	if (m_AsyncLoadingFlushUpdateHandle.IsValid())
	{
		FCoreDelegates::OnAsyncLoadingFlushUpdate.Remove(m_AsyncLoadingFlushUpdateHandle);
	}
}

//毎フレーム実行される関数
void FLoadingScreenSystem::Tick(float DeltaTime)
{

}

//ロードウィジェットの表示
void FLoadingScreenSystem::ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass, const FName InPackageName)
{
	if (m_bShowing) { return; }
	if (!m_pGameInstance) { return; }

	if (UUserWidget* pLoadingWidget = CreateWidget(m_pGameInstance, WidgetClass))
	{
		m_pLoadingScreenWidget = pLoadingWidget->TakeWidget();
		if (m_pLoadingScreenWidget.IsValid())
		{
			if (UGameViewportClient* pGameViewportClient = m_pGameInstance->GetGameViewportClient())
			{
				//ロードするパッケージ名を表示
				m_PackageName = InPackageName;
				//ロード画面を表示
				const int32 ZOrder = 10000;
				pGameViewportClient->AddViewportWidgetContent(m_pLoadingScreenWidget.ToSharedRef(), ZOrder);

				// Widgetが表示されているはずなので3D描画を完全にカットする
				pGameViewportClient->bDisableWorldRendering = true;
				if (!GIsEditor)
				{
					FSlateApplication::Get().Tick();
				}
				//表示状態に切り替える
				m_bShowing = true;
			}
		}
	}
}

//ロードウィジェットの非表示
void FLoadingScreenSystem::HideLoadingScreen()
{
	if (!m_bShowing) { return; }
	if (!GEngine) { return; }
	if (!m_pGameInstance) { return; }
	if (!m_pLoadingScreenWidget.IsValid()) { return; }

	if (UGameViewportClient* pGameViewportClient = m_pGameInstance->GetGameViewportClient())
	{
		//3D描画をONにする
		pGameViewportClient->bDisableWorldRendering = false;
		//ロード画面を非表示にする
		pGameViewportClient->RemoveViewportWidgetContent(m_pLoadingScreenWidget.ToSharedRef());
		//ウィジェットのポインタを解放するためにGCを呼び出す
		GEngine->ForceGarbageCollection(true);
		//非表示状態に切り替える
		m_bShowing = false;
	}
}

//ロードの進捗を取得する関数
float FLoadingScreenSystem::GetLoadingProgress()
{
	float Sum = 0.f;
	int32 PackageNum = 1;
	UPackage* PersistentLevelPackage = FindObjectFast<UPackage>(NULL, m_PackageName, true, true);
	if (PersistentLevelPackage && (PersistentLevelPackage->GetLoadTime() > 0))
	{
		Sum += 100.0f;
	}
	else
	{
		const float LevelLoadPercentage = GetAsyncLoadPercentage(m_PackageName);
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
	m_Progress = Current * 0.05f + m_Progress * 0.95f;
	return m_Progress;
}

/*このデリゲート関数はロード中に高頻度で呼ばれるので、適切な間隔でスレートの更新を呼ぶようにする*/
void FLoadingScreenSystem::OnAsyncLoadingFlushUpdate()
{
	check(IsInGameThread());

	QUICK_SCOPE_CYCLE_COUNTER(STAT_LoadingScreenManager_OnAsyncLoadingFlushUpdate);

	const double CurrentTime = FPlatformTime::Seconds();
	const double DeltaTime = CurrentTime - m_LastTickTime;
	if (DeltaTime > 1.0f / 60.0f)
	{
		m_LastTickTime = CurrentTime;
		if (m_bShowing) 
		{
			// スレート更新
			FSlateApplication::Get().Tick();

			{
				TGuardValue<int32> DisableAsyncLoadDuringSync(GDoAsyncLoadingWhileWaitingForVSync, 0);
				FSlateApplication::Get().GetRenderer()->Sync();
			}
		}

		m_LastTickTime = CurrentTime;
	}
}

//コンストラクタ
ULoadingScreenLibrary::ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//ロード画面の表示
void ULoadingScreenLibrary::ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass, FName InPackageName)
{
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		if (TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem = pGameInstance->GetLoadingScreenSystem())
		{
			pLoadingScreenSystem->ShowLoadingScreen(WidgetClass, InPackageName);
		}
	}
}

//ロード画面の非表示
void ULoadingScreenLibrary::HideLoadingScreen()
{
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		if (TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem = pGameInstance->GetLoadingScreenSystem())
		{
			pLoadingScreenSystem->HideLoadingScreen();
		}
	}
}

float ULoadingScreenLibrary::GetLoadingProgress()
{
	float progress = 0.f;
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		if (TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem = pGameInstance->GetLoadingScreenSystem())
		{
			progress = pLoadingScreenSystem->GetLoadingProgress();
		}
	}
	return progress;
}