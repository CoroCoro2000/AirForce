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
public:
	//コンストラクタ
	FLoadingScreenSystem(URacingD_GameInstance* InGameInstance);
	//デストラクタ
	~FLoadingScreenSystem();
	//CycleCounterを返す関数
	virtual TStatId GetStatId() const  override{ RETURN_QUICK_DECLARE_CYCLE_STAT(FSimpleLoadingScreenSystem, STATGROUP_Tickables); }
	//毎フレーム実行される関数
	virtual void Tick(float DeltaTime) override;
	//読み込みをするパッケージ名の設定
	void SetPackageNameForLoadingProgress(FName InPackageName) { m_PackageName = InPackageName; }
	//ロードの進捗を取得する関数
	float GetLoadingProgress();
	//ロード開始時に呼び出される関数
	void OnBeginLoadingScreen(const FString& MapName);
	//ロード完了時に呼び出される関数
	void OnEndLoadingScreen(UWorld* world);
	//ロードの更新処理
	void OnAsyncLoadingFlushUpdate();

private:
	URacingD_GameInstance* m_pGameInstance;								//ゲームインスタンス
	TSharedPtr<SWidget>	m_pLoadingScreenWidget;							//ロード中に表示するスレートウィジェット
	bool m_bLoading;													//ロード中かどうか
	double m_LastTickTime;												//最後に実行されたTickの処理時間
	FName m_PackageName;												//ロードするパッケージ名(path)
	float m_Progress;													//ロードの進行状況
};


/**
 *		//Blueprint library
 */
UCLASS(meta = (ScriptName = "LoadingScreen"))
class AIRFORCE_API ULoadingScreenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer);
public:
	//ロードの進行状況を受け取るパッケージ名を設定
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetTargetPackageForLoadingProgress(const UObject* WorldContextObject, FName InPackageName);

	/** 0~100 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static float GetLoadingProgress(const UObject* WorldContextObject);

	//レベルのロード後に開く関数
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void LoadAndOpenLevel(const UObject* WorldContextObject, const FName PersistentLevelName, const bool bAbsolute = true, const FString Options = FString(TEXT("")));
};
