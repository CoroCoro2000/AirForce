
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

//�R���X�g���N�^
URacingD_GameInstance::URacingD_GameInstance()
	: m_pLoadingScreenSystem(nullptr)
	, m_PCName(TEXT("PC_1"))
{

}

//�Q�[���J�n���Ɏ��s����֐�
void URacingD_GameInstance::Init()
{
	Super::Init();

	//���[�f�B���O�V�X�e���̐���
	m_pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));
}

//�Q�[���I�����Ɏ��s����֐�
void URacingD_GameInstance::Shutdown()
{
	Super::Shutdown();

	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem.Reset();
		m_pLoadingScreenSystem = nullptr;
	}
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