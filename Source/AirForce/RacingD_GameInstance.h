// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;

/**
 * 
 */
UCLASS()
class AIRFORCE_API URacingD_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//コンストラクタ
	URacingD_GameInstance();

private:
	//ゲーム起動時に実行される関数
	virtual void Init() override;
	//ゲーム終了時に実行される関数
	virtual void Shutdown() override;

public:
	//ゲームインスタンスの取得
	static URacingD_GameInstance* Get();
	//ロードスクリーン用のシステムを取得
	TSharedPtr<FLoadingScreenSystem> GetLoadingScreenSystem()const { return m_pLoadingScreenSystem; }
	//ストリーミングロードのID取得
	int32 GenerateUUID() { return ++m_LoadStreamID; }
	//ストリーミングロードのID削除
	void DeleteUUID() { --m_LoadStreamID; }

private:
	TSharedPtr<FLoadingScreenSystem> m_pLoadingScreenSystem;						//ロードスクリーンシステム
	int32 m_LoadStreamID;																					//ストリーミングロードのID
};
