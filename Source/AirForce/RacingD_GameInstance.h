// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;
class USaveRecord;
class UAsyncActionHandleSaveGame;

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
	UFUNCTION(BlueprintCallable)
	static URacingD_GameInstance* Get();
	//ロードスクリーン用のシステムを取得
	TSharedPtr<FLoadingScreenSystem> GetLoadingScreenSystem()const { return m_pLoadingScreenSystem; }
	//ゲームデータのロード
	void LoadGameData(const FString& SlotName, const int32& SlotIndex);
	//ゲームデータの非同期ロード(C++)
	void AsyncLoadGameData(const FString& SlotName, const int32 SlotIndex, FAsyncLoadGameFromSlotDelegate Callback);
	//ゲームデータの非同期ロード(C++)
	void AsyncLoadGameData(const FString& SlotName, const int32 SlotIndex);
	//ゲームデータのセーブ
	void SaveGameData(const FString& SlotName, const int32& SlotIndex)const;
	//ゲームデータの非同期セーブ(C++)
	void AsyncSaveGameData(const FString& SlotName, const int32 SlotIndex, FAsyncSaveGameToSlotDelegate Callback);
	//ゲームデータの非同期セーブ(C++)
	void AsyncSaveGameData(const FString& SlotName, const int32 SlotIndex);
	//セーブデータの設定
	void SetSeveRecord(USaveRecord* pSaveRecord){m_pSaveRecord = pSaveRecord;}
	//セーブデータの取得
	UFUNCTION(BlueprintCallable)
	USaveRecord* GetSaveRecord()const { return m_pSaveRecord; }
	//ゲームデータの保存名取得
	UFUNCTION(BlueprintCallable)
	static FString GetSaveSlotName();

private:
	//非同期ロード完了時に呼ばれるコールバック
	UFUNCTION()
	void LoadCompleted(const FString& SlotName, const int32 SlotIndex, USaveGame* LoadData);
	//非同期セーブ完了時に呼ばれるコールバック
	UFUNCTION()
	void SaveCompleted(const FString& SavedSlotName, const int32 SavedSlotIndex, bool bSuccess);

private:
	TSharedPtr<FLoadingScreenSystem> m_pLoadingScreenSystem;						//ロードスクリーンシステム
	UPROPERTY(EditAnywhere)
		USaveRecord* m_pSaveRecord;													//セーブデータ

public:
	static const FString SaveSlotName;												//セーブ用のスロット名
};