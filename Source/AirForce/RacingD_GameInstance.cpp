
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"
#include "Blueprint/UserWidget.h"
#include "SaveRecord.h"
#include "ConfigParameter.h"
#include "Kismet/GameplayStatics.h"

//コンストラクタ
URacingD_GameInstance::URacingD_GameInstance()
	: m_pLoadingScreenSystem(nullptr)
{

}

//ゲーム開始時に実行する関数
void URacingD_GameInstance::Init()
{
	Super::Init();

	//ローディングシステムの生成
	m_pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));

	//コンフィグパラメーターの初期化
	CConfigParameter::InitConfigParameter();
}

//ゲーム終了時に実行する関数
void URacingD_GameInstance::Shutdown()
{
	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem.Reset();
		m_pLoadingScreenSystem = nullptr;
	}

	Super::Shutdown();
}

//ゲームインスタンスの取得
URacingD_GameInstance* URacingD_GameInstance::Get()
{
	URacingD_GameInstance* pGameInstance = nullptr;
	if (GEngine)
	{
		if (const FWorldContext* pContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
		{
			pGameInstance = Cast<URacingD_GameInstance>(pContext->OwningGameInstance);
		}
	}
	return pGameInstance;
}