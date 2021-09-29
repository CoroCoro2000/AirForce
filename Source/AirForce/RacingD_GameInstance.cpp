
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"
#include "Engine/LatentActionManager.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

//�R���X�g���N�^
URacingD_GameInstance::URacingD_GameInstance()
	: m_LatentAction(0, 1, TEXT("Completed"), this)
	, m_bLoadComplete(false)
{}

void URacingD_GameInstance::Init()
{
	Super::Init();

	pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));
	m_LatentAction = FLatentActionInfo(0, 1, TEXT("Completed"), this);
	m_bLoadComplete = false;
}

void URacingD_GameInstance::Shutdown()
{
	pLoadingScreenSystem.Reset();
	Super::Shutdown();
}

//���x���̃��[�h����
void URacingD_GameInstance::LoadLevel(const FName& _level)
{
	m_bLoadComplete = false;
	UGameplayStatics::LoadStreamLevel(this, _level, false, false, m_LatentAction);
}

//���x���̃A�����[�h����
void URacingD_GameInstance::UnloadLevel(const FName& _level)
{
	m_bLoadComplete = false;
	UGameplayStatics::UnloadStreamLevel(this, _level, m_LatentAction, false);
}

//���x���̕\������
bool URacingD_GameInstance::ShowLevel(const FName& _level) const
{
	ULevelStreaming* levelstream = UGameplayStatics::GetStreamingLevel(GetWorld(), _level);
	check(levelstream != nullptr);
	levelstream->SetShouldBeVisible(true);
	return levelstream->IsLevelVisible();
}

//���x���̔�\������
bool URacingD_GameInstance::HideLevel(const FName& _level) const
{
	ULevelStreaming* levelstream = UGameplayStatics::GetStreamingLevel(GetWorld(), _level);
	check(levelstream != nullptr);
	levelstream->SetShouldBeVisible(false);
	return !levelstream->IsLevelVisible();
}