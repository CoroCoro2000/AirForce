
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

//�R���X�g���N�^
URacingD_GameInstance::URacingD_GameInstance()
	: m_pLoadingScreenSystem()
	, m_LoadingUMGClass()
{
	//�f�t�H���g�̃��[�h�p�E�B�W�F�b�g�N���X��ݒ�
	FString path = TEXT("/Game/UI/Load/LoadWidget.LoadWidget_C");
	TSubclassOf<UUserWidget> widgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*path)).LoadSynchronous();
	m_LoadingUMGClass = widgetClass;
}

//�Q�[���J�n���Ɏ��s����֐�
void URacingD_GameInstance::Init()
{
	Super::Init();

	//���[�f�B���O�V�X�e���̐���
	m_pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));
	//���[�h���ɌĂяo���֐��̃o�C���h
	BindLoadingContent();
}

//�Q�[���I�����Ɏ��s����֐�
void URacingD_GameInstance::Shutdown()
{
	Super::Shutdown();

	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem.Reset();
	}
}

//���[�h���ɌĂяo���֐����o�C���h
void URacingD_GameInstance::BindLoadingContent()
{
	//���[�h�J�n���ƏI�����ɌĂяo���֐���o�^
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &URacingD_GameInstance::OnBeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URacingD_GameInstance::OnEndLoadingScreen);
}

//�Q�[���C���X�^���X�̎擾
URacingD_GameInstance* URacingD_GameInstance::Get()
{
	URacingD_GameInstance* pGameInstance = nullptr;
	if (GEngine)
	{
		FWorldContext* pContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
		if (pContext)
		{
			pGameInstance = Cast<URacingD_GameInstance>(pContext->OwningGameInstance);
		}
	}
	return pGameInstance;
}

//���[�h�J�n���ɌĂяo���֐�
void URacingD_GameInstance::OnBeginLoadingScreen(const FString& MapName)
{
	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem->OnBeginLoadingScreen(MapName);
	}
}

//���[�h�I�����ɌĂяo���֐�
void URacingD_GameInstance::OnEndLoadingScreen(UWorld* world)
{
	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem->OnEndLoadingScreen(world);
	}
}