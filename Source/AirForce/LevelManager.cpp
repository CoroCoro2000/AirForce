// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

// Sets default values
ALevelManager::ALevelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetTickableWhenPaused(true);
}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelManager::Tick(float DeltaTime)
{
	SetTickableWhenPaused(true);
	Super::Tick(DeltaTime);
}

void ALevelManager::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	//���̓}�b�s���O
	InputComponent->BindAction(TEXT("InputUP"), EInputEvent::IE_Pressed, this, &ALevelManager::InputUp);
	InputComponent->BindAction(TEXT("InputDown"), EInputEvent::IE_Pressed, this, &ALevelManager::InputDown);
	InputComponent->BindAction(TEXT("InputLeft"), EInputEvent::IE_Pressed, this, &ALevelManager::InputLeft);
	InputComponent->BindAction(TEXT("InputRight"), EInputEvent::IE_Pressed, this, &ALevelManager::InputRight);
	InputComponent->BindAction(TEXT("InputA"), EInputEvent::IE_Pressed, this, &ALevelManager::InputA);
}

void ALevelManager::InputUp()
{
	//���͉\�łȂ��Ȃ珈�����Ȃ�
	if (!m_isInputEnable)
	{
		return;
	}
	if (m_LevelAddValue[EINPUTKEY::EINPUTKEY_VERTICAL] == 0)
	{
		return;
	}

	m_LevelIndex -= m_LevelAddValue[EINPUTKEY::EINPUTKEY_VERTICAL];

	if (m_LevelIndex < m_LevelIndexMin[EINPUTKEY::EINPUTKEY_VERTICAL])
	{
		m_LevelIndex = m_LevelIndexMax[EINPUTKEY::EINPUTKEY_VERTICAL];
	}
}

void ALevelManager::InputDown()
{
	//���͉\�łȂ��Ȃ珈�����Ȃ�
	if (!m_isInputEnable)
	{
		return;
	}
	if (m_LevelAddValue[EINPUTKEY::EINPUTKEY_VERTICAL] == 0)
	{
		return;
	}

	m_LevelIndex += m_LevelAddValue[EINPUTKEY::EINPUTKEY_VERTICAL];

	if (m_LevelIndex > m_LevelIndexMax[EINPUTKEY::EINPUTKEY_VERTICAL])
	{
		m_LevelIndex = m_LevelIndexMin[EINPUTKEY::EINPUTKEY_VERTICAL];
	}
}

void ALevelManager::InputLeft()
{
	//���͉\�łȂ��Ȃ珈�����Ȃ�
	if (!m_isInputEnable)
	{
		return;
	}
	if (m_LevelAddValue[EINPUTKEY::EINPUTKEY_HORIZONTAL] == 0)
	{
		return;
	}
	m_LevelIndex -= m_LevelAddValue[EINPUTKEY::EINPUTKEY_HORIZONTAL];

	if (m_LevelIndex < m_LevelIndexMin[EINPUTKEY::EINPUTKEY_HORIZONTAL])
	{
		m_LevelIndex = m_LevelIndexMax[EINPUTKEY::EINPUTKEY_HORIZONTAL];
	}
}

void ALevelManager::InputRight()
{
	//���͉\�łȂ��Ȃ珈�����Ȃ�
	if (!m_isInputEnable)
	{
		return;
	}
	if (m_LevelAddValue[EINPUTKEY::EINPUTKEY_HORIZONTAL] == 0)
	{
		return;
	}
	m_LevelIndex += m_LevelAddValue[EINPUTKEY::EINPUTKEY_HORIZONTAL];

	if (m_LevelIndex > m_LevelIndexMax[EINPUTKEY::EINPUTKEY_HORIZONTAL])
	{
		m_LevelIndex = m_LevelIndexMin[EINPUTKEY::EINPUTKEY_HORIZONTAL];
	}
}

void ALevelManager::InputA()
{
	//���͉\�łȂ��Ȃ珈�����Ȃ�
	if (!m_isInputEnable)
	{
		return;
	}

	//�I���������x����ǂݍ���
	UGameplayStatics::OpenLevel(GetWorld(), m_LevelName[m_LevelIndex]);
}