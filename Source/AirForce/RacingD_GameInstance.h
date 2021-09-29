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
	virtual void Init() override;
	virtual void Shutdown() override;

public:
//レベルコントローラ関数
//-----------------------------------------------------------
//レベルロード完了時に呼び出される関数
	UFUNCTION()
		void Completed() { m_bLoadComplete = true; }
	//レベルのロード処理
	void LoadLevel(const FName& _level);
	//レベルのアンロード処理
	void UnloadLevel(const FName& _level);
	//レベルの表示処理
	bool ShowLevel(const FName& _level) const;
	//レベルの非表示処理
	bool HideLevel(const FName& _level) const;
	//ロード/アンロードが終了したか確認する関数
	bool IsCompleted() const { return m_bLoadComplete; }
	//-----------------------------------------------------------
public:
	TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem;

private:
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		FLatentActionInfo m_LatentAction;
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		bool m_bLoadComplete;
};
