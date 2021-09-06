// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingD_GameInstance.generated.h"

//前方宣言
class AGameManager;								//ゲームマネージャークラス

//現行レベルの列挙
UENUM(BlueprintType)
namespace ECURRENT_LEVEL
{
	enum Type
	{
		TITLE						UMETA(DisplayName = "Title"),
		SELECT						UMETA(DisplayName = "Select"),
		RACE							UMETA(DisplayName = "Race"),
		RESULT						UMETA(DisplayName = "	Result"),
		NUM							UMETA(Hidden),
	};
}

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
private:
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		FLatentActionInfo m_LatentAction;
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		bool m_bLoadComplete;
};
