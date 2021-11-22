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
	virtual TStatId GetStatId() const  override{ RETURN_QUICK_DECLARE_CYCLE_STAT(FLoadingScreenSystem, STATGROUP_Tickables); }
	//毎フレーム実行される関数
	virtual void Tick(float DeltaTime) override;
	//読み込みをするパッケージ名の設定
	void SetPackageNameForLoadingProgress(FName InPackageName) { m_PackageName = InPackageName; }
	//ロード画面の表示
	void ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass);
	//ロード画面の非表示
	void HideLoadingScreen();
	//ロードの進捗を取得する関数
	float GetLoadingProgress();


private:
	//スレートの更新処理
	void OnAsyncLoadingFlushUpdate();

private:
	URacingD_GameInstance* m_pGameInstance;								//ゲームインスタンス
	TSharedPtr<SWidget>	m_pLoadingScreenWidget;						//ロード中に表示するスレートウィジェット
	bool m_bShowing;																		//ウィジェットが表示されているか
	double m_LastTickTime;																//最後に実行されたTickの処理時間
	FName m_PackageName;															//ロードするパッケージ名(パス指定)
	float m_Progress;																		//ロードの進行状況
	FDelegateHandle m_AsyncLoadingFlushUpdateHandle;					//OnAsyncLoadingFlushUpdateのバインド、解除を行うデリゲートハンドル
};

/**
 *		//Blueprint library
 */
UCLASS(meta = (ScriptName = "LoadingScreen"))
class AIRFORCE_API ULoadingScreenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//コンストラクタ
	ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer);

public:
	//ロード画面の表示
	UFUNCTION(BlueprintCallable)
		static void ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass, FName InPackageName);
	//ロード画面の非表示
	UFUNCTION(BlueprintCallable)
		static void HideLoadingScreen();
	//ロードの進行状況を取得
	UFUNCTION(BlueprintCallable)
		static float GetLoadingProgress();

};
