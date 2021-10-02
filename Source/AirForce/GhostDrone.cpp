// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostDrone.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Misc/FileHelper.h"

//�R���X�g���N�^
AGhostDrone::AGhostDrone()
	: PlaybackFlame(0)
	, m_LoadVelocity(FVector::ZeroVector)
	, m_LoadQuat(FQuat::Identity)
	, m_StartLocation(FVector::ZeroVector)
	, m_StartQuaternion(FQuat::Identity)
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


	//�����ʒu�ƃ��b�V���̉�]��ۑ�
	m_StartLocation = GetActorLocation();
	m_StartQuaternion = m_pBodyMesh->GetComponentQuat();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *(m_StartLocation.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *(m_StartQuaternion.ToString()));
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
			if (m_isReplay)
			{
				SetActorLocation(m_StartLocation);
				m_pBodyMesh->SetWorldRotation(m_StartQuaternion);
				PlaybackFlame = 0;
			}
			else
			{
				Destroy();
			}
		}
	}
}

//�h���[���̉�]����
void AGhostDrone::UpdateRotation(const float& DeltaTime)
{
	//�ǂݍ��񂾈ړ��ʂ̃e�L�X�g�t�@�C����float�ɕϊ�����
	bool IsValidTextArray = true;
	bool IsValidAxisTextArray = true;

	for (int index = 0; index < VECTOR4_COMPONENT_NUM; ++index)
	{
		IsValidTextArray = m_SaveQuatText.IsValidIndex(index);
		IsValidAxisTextArray = m_SaveQuatText[index].IsValidIndex(PlaybackFlame);
		if (!IsValidTextArray || IsValidAxisTextArray)
		{
			break;
		}
	}

	if (IsValidTextArray && IsValidAxisTextArray)
	{
		m_LoadQuat.X = FCString::Atof(*(m_SaveQuatText[0][PlaybackFlame]));
		m_LoadQuat.Y = FCString::Atof(*(m_SaveQuatText[1][PlaybackFlame]));
		m_LoadQuat.Z = FCString::Atof(*(m_SaveQuatText[2][PlaybackFlame]));
		m_LoadQuat.W = FCString::Atof(*(m_SaveQuatText[3][PlaybackFlame]));
	}
	m_pBodyMesh->SetWorldRotation(m_LoadQuat * MOVE_CORRECTION);
}

//�ړ�����
void AGhostDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	//�ǂݍ��񂾈ړ��ʂ̃e�L�X�g�t�@�C����float�ɕϊ�����
	int index = 0;
	for (const TArray<FString> SaveVelocityText : m_SaveVelocityText)
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
	m_SaveVelocityText.SetNum(VECTOR3_COMPONENT_NUM);

	//�ݒ肵���p�X�����Ɠ������ݒ肳��Ă���΃��[�h����
	if (m_SaveVelocityText.Num() == m_SaveVelocityLoadPath.Num())
	{
		//�t�@�C�����J���ĕۑ�����Ă���l��ǂݍ���
		for (int index = 0; index < (int)m_SaveVelocityText.Num(); ++index)
		{
			FString LoadFilePath = FPaths::ProjectDir() + SaveFolderPath + m_SaveVelocityLoadPath[index];
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
	m_SaveQuatText.SetNum(VECTOR4_COMPONENT_NUM);

	//�ݒ肵���p�X�����Ɠ������ݒ肳��Ă���΃��[�h����
	if (m_SaveQuatText.Num() == m_SaveQuatLoadPath.Num())
	{
		//�t�@�C�����J���ĕۑ�����Ă���l��ǂݍ���
		for (int index = 0; index < (int)m_SaveQuatText.Num(); ++index)
		{
			FString LoadFilePath = FPaths::ProjectDir() + SaveFolderPath + m_SaveQuatLoadPath[index];
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