// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostDrone.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Misc/FileHelper.h"

//�R���X�g���N�^
AGhostDrone::AGhostDrone()
	: PlaybackFlame(1)
	, Time(0.f)
	, FlameCnt(0)
	, m_LoadVelocity(FVector::ZeroVector)
	, m_LoadQuat(FVector::ZeroVector)
{
	//���g��Tick()�𖈃t���[���Ăяo�����ǂ���
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = m_pDroneCollision;

	if (m_pBodyMesh)
	{
		m_pBodyMesh->SetupAttachment(m_pDroneCollision);
	}
}
//�Q�[���J�n����1�x��������
void AGhostDrone::BeginPlay()
{
	Super::BeginPlay();

	//���[�X�̍��W�t�@�C���ǂݍ���
	LoadingRaceVectorFile();
	//���[�X�̃N�I�[�^�j�I���t�@�C���ǂݍ���
	LoadingRaceQuaternionFile();
}

//���t���[������
void AGhostDrone::Tick(float DeltaTime)
{
	if (m_isControl)
	{
			//�H�̉�]�X�V����
			UpdateWingRotation(DeltaTime);

			//�h���[���̉�]����
			UpdateRotation(DeltaTime);

			//�ړ�����
			UpdateSpeed(DeltaTime);

			PlaybackFlame++;

		if(PlaybackFlame >= m_PlayableFramesNum)
		{
			Destroy();
		}
	}
}

//�h���[���̉�]����
void AGhostDrone::UpdateRotation(const float& DeltaTime)
{
	//�ǂݍ��񂾈ړ��ʂ̃e�L�X�g�t�@�C����float�ɕϊ�����
	int index = 0;
	for (const TArray<FString>& SaveQuatText : m_SaveQuatText)
	{
		if (SaveQuatText.IsValidIndex(PlaybackFlame))
		{
			m_LoadQuat[index] = FCString::Atof(*(SaveQuatText[PlaybackFlame]));
		}
		++index;
	}

	FQuat NewQuat = FQuat(m_LoadQuat, m_LoadQuat.W);
	m_pBodyMesh->SetRelativeRotation(NewQuat * MOVE_CORRECTION);
}

//�ړ�����
void AGhostDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	//�ǂݍ��񂾈ړ��ʂ̃e�L�X�g�t�@�C����float�ɕϊ�����
	int index = 0;
	for (const TArray<FString>& SaveVelocityText : m_SaveVelocityText)
	{
		if (SaveVelocityText.IsValidIndex(PlaybackFlame))
		{
			m_LoadVelocity[index] = FCString::Atof(*(SaveVelocityText[PlaybackFlame]));
		}
		++index;
	}
	AddActorWorldOffset(m_LoadVelocity * MOVE_CORRECTION);
}

//���[�X�̍��W�t�@�C���ǂݍ���
void AGhostDrone::LoadingRaceVectorFile()
{
	//�ړ����̐������z���p�ӂ���
	m_SaveVelocityText.Empty();
	m_SaveVelocityText.SetNum(3);

	//�ݒ肵���p�X�����Ɠ������ݒ肳��Ă���΃��[�h����
	if (m_SaveVelocityText.Num() == m_SaveVelocityLoadPath.Num())
	{
		//�t�@�C�����J���ĕۑ�����Ă���l��ǂݍ���
		for (int index = 0; index < (int)m_SaveVelocityText.Num(); ++index)
		{
			FString LoadFilePath = FPaths::ProjectDir() + m_SaveVelocityLoadPath[index];
			FFileHelper::LoadFileToStringArray(m_SaveVelocityText[index], *LoadFilePath);

			//�Đ��\�ȃt���[�������擾
			int PlayableFrame = m_SaveVelocityText[index].Num() - 1;
			if (index != 0)
			{
				if (PlayableFrame < m_PlayableFramesNum)
				{ 
					m_PlayableFramesNum = PlayableFrame;
				}
			}
			else
			{
				m_PlayableFramesNum = PlayableFrame;
			}
		}
	}
}

//���[�X�̃N�I�[�^�j�I���t�@�C���ǂݍ���
void AGhostDrone::LoadingRaceQuaternionFile()
{
	//�N�H�[�^�j�I���̎��̐������z���p�ӂ���
	m_SaveQuatText.Empty();
	m_SaveQuatText.SetNum(4);

	//�ݒ肵���p�X�����Ɠ������ݒ肳��Ă���΃��[�h����
	if (m_SaveQuatText.Num() == m_SaveQuatLoadPath.Num())
	{
		//�t�@�C�����J���ĕۑ�����Ă���l��ǂݍ���
		for (int index = 0; index < (int)m_SaveVelocityText.Num(); ++index)
		{
			FString LoadFilePath = FPaths::ProjectDir() + m_SaveQuatLoadPath[index];
			FFileHelper::LoadFileToStringArray(m_SaveQuatText[index], *LoadFilePath);

			//�Đ��\�ȃt���[�������擾
			int PlayableFrame = m_SaveQuatText[index].Num() - 1;
			if (PlayableFrame < m_PlayableFramesNum)
			{
				m_PlayableFramesNum = PlayableFrame;
			}
		}
	}
}