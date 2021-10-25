
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

//コンストラクタ
URacingD_GameInstance::URacingD_GameInstance()
	: m_pLoadingScreenSystem()
	, m_LoadingUMGClass()
{
	//デフォルトのロード用ウィジェットクラスを設定
	FString path = TEXT("/Game/UI/Load/LoadWidget.LoadWidget_C");
	TSubclassOf<UUserWidget> widgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*path)).LoadSynchronous();
	m_LoadingUMGClass = widgetClass;
}

//ゲーム開始時に実行する関数
void URacingD_GameInstance::Init()
{
	Super::Init();

	//ローディングシステムの生成
	m_pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));
	//ロード時に呼び出す関数のバインド
	BindLoadingContent();
}

//ゲーム終了時に実行する関数
void URacingD_GameInstance::Shutdown()
{
	Super::Shutdown();

	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem.Reset();
	}
}

//ロード時に呼び出す関数をバインド
void URacingD_GameInstance::BindLoadingContent()
{
	//ロード開始時と終了時に呼び出す関数を登録
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &URacingD_GameInstance::OnBeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URacingD_GameInstance::OnEndLoadingScreen);
}

//ゲームインスタンスの取得
URacingD_GameInstance* URacingD_GameInstance::Get()
{
	URacingD_GameInstance* pGameInstance = nullptr;
	if (GEngine)
	{
		FWorldContext* pContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
		if (pContext)
		{
			pGameInstance = Cast<URacingD_GameInstance>(pContext->OwningGameInstance);
		}
	}
	return pGameInstance;
}

//ロード開始時に呼び出す関数
void URacingD_GameInstance::OnBeginLoadingScreen(const FString& MapName)
{
	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem->OnBeginLoadingScreen(MapName);
	}
}

//ロード終了時に呼び出す関数
void URacingD_GameInstance::OnEndLoadingScreen(UWorld* world)
{
	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem->OnEndLoadingScreen(world);
	}
}