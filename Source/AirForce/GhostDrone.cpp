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

	PrimaryActorTick.TickInterval = 0.f;
}

//���t���[������
void AGhostDrone::Tick(float DeltaTime)
{
	if (m_isControl)
	{
		if (PlaybackFlame < m_SaveVelocityX.Num() - 1)
		{
			//�H�̉�]�X�V����
			UpdateWingRotation(DeltaTime);

			//�h���[���̉�]����
			UpdateRotation(DeltaTime);

			//�ړ�����
			UpdateSpeed(DeltaTime);

			PlaybackFlame++;
		}

		else
		{
			Destroy();
		}
	}
}

//�h���[���̉�]����
void AGhostDrone::UpdateRotation(const float& DeltaTime)
{
	if(m_SaveQuatX.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.X = FCString::Atof(*(m_SaveQuatX[PlaybackFlame]));
	}
	if (m_SaveQuatY.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.Y = FCString::Atof(*(m_SaveQuatY[PlaybackFlame]));
	}
	if (m_SaveQuatZ.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.Z = FCString::Atof(*(m_SaveQuatZ[PlaybackFlame]));
	}
	if (m_SaveQuatW.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.W = FCString::Atof(*(m_SaveQuatW[PlaybackFlame]));
	}

	m_pBodyMesh->SetRelativeRotation(m_SaveQuat * MOVE_CORRECTION);
}

//�ړ�����
void AGhostDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	m_SaveVelocity.X = FCString::Atof(*(m_SaveVelocityX[PlaybackFlame]));
	m_SaveVelocity.Y = FCString::Atof(*(m_SaveVelocityY[PlaybackFlame]));
	m_SaveVelocity.Z = FCString::Atof(*(m_SaveVelocityZ[PlaybackFlame]));

	AddActorWorldOffset(m_SaveVelocity * MOVE_CORRECTION);
}

//���[�X�̍��W�t�@�C���ǂݍ���
void AGhostDrone::LoadingRaceVectorFile()
{
	//�e�L�X�g�t�@�C���ǂݍ���
	FFileHelper::LoadFileToStringArray(m_SaveVelocityX, *(FPaths::ProjectDir() + FString("Record/VX.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveVelocityY, *(FPaths::ProjectDir() + FString("Record/VY.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveVelocityZ, *(FPaths::ProjectDir() + FString("Record/VZ.txt")));
}

//���[�X�̃N�I�[�^�j�I���t�@�C���ǂݍ���
void AGhostDrone::LoadingRaceQuaternionFile()
{
	//�e�L�X�g�t�@�C���ǂݍ���
	FFileHelper::LoadFileToStringArray(m_SaveQuatX, *(FPaths::ProjectDir() + FString("Record/QX.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveQuatY, *(FPaths::ProjectDir() + FString("Record/QY.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveQuatZ, *(FPaths::ProjectDir() + FString("Record/QZ.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveQuatW, *(FPaths::ProjectDir() + FString("Record/QW.txt")));
}