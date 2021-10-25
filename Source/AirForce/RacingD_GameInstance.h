// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;
class UUserWidget;
class SWidget;

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
	virtual void Init() override;
	virtual void Shutdown() override;

	//ロード時に実行される関数をバインド
	void BindLoadingContent();

public:
	//ゲームインスタンスの取得
	static URacingD_GameInstance* Get();
	//ロードスクリーン用のシステムを取得
	TSharedPtr<FLoadingScreenSystem> GetLoadingScreenSystem()const { return m_pLoadingScreenSystem; }
	//ロード開始時に呼び出す関数
	void OnBeginLoadingScreen(const FString& MapName);
	//ロード終了時に呼び出す関数
	void OnEndLoadingScreen(UWorld* world);

	//ロード中に表示するウィジェットクラスを取得
	TSubclassOf<UUserWidget> GetLoadingUMGClass()const { return m_LoadingUMGClass; }
protected:
	TSharedPtr<FLoadingScreenSystem> m_pLoadingScreenSystem;						//ロードスクリーンシステム
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UUserWidget> m_LoadingUMGClass;										//ロード中に表示するUMGクラス
};
