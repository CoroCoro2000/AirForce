
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"

//コンストラクタ
URacingD_GameInstance::URacingD_GameInstance()
{}

void URacingD_GameInstance::Init()
{
	Super::Init();

	pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));

	UE_LOG(LogTemp, Warning, TEXT("Init"));
}

void URacingD_GameInstance::Shutdown()
{
	if (pLoadingScreenSystem.IsValid())
	{
		pLoadingScreenSystem.Reset();
	}
	Super::Shutdown();

	UE_LOG(LogTemp, Warning, TEXT("Shutdown"));
}