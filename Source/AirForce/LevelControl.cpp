// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelControl.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"

// Sets default values
ALevelControl::ALevelControl()
	: m_LatentAction(0, 1, TEXT("Completed"), this)
	, m_Complete(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelControl::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelControl::Completed()
{
	m_Complete = true;
}

// ���[�h���x��
void ALevelControl::LoadLevel(const FName& level)
{
	m_Complete = false;
	UGameplayStatics::LoadStreamLevel(this, level, false, false, m_LatentAction);
}

// �A�����[�h���x��
void ALevelControl::UnloadLevel(const FName& level)
{
	m_Complete = false;
	UGameplayStatics::UnloadStreamLevel(this, level, m_LatentAction, false);
}

// ���x���̕\��(true�Ŋ����Afalse�ŏ�����)
bool ALevelControl::ShowLevel(const FName& level) const
{
	ULevelStreaming* levelstream = UGameplayStatics::GetStreamingLevel(GetWorld(), level);
	check(levelstream != nullptr);
	levelstream->SetShouldBeVisible(true);
	return levelstream->IsLevelVisible();  
}

// ���x���̔�\��(true�Ŋ����Afalse�ŏ�����)
bool ALevelControl::HideLevel(const FName& level) const
{
	ULevelStreaming* levelstream = UGameplayStatics::GetStreamingLevel(GetWorld(), level);
	check(levelstream != nullptr);
	levelstream->SetShouldBeVisible(false);
	return !levelstream->IsLevelVisible();
}

// ���[�h/�A�����[�h�̊����m�F
bool ALevelControl::IsCompleted() const
{
	return m_Complete;
}