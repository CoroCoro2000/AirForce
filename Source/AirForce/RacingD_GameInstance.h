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
		RACE						UMETA(DisplayName = "Race"),
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
	
	

private:
	AGameManager* m_pGameManager;					//レベルごとのマネージャークラス
};
