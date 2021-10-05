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
	TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem;

};
