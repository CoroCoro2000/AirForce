// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pTrainMesh(CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("TrainMesh")))
	, m_pSplineActor(NULL)
	, m_MaxSpeed(500.f)
	, m_CurrentSpeed(0.f)
	, m_Acceleration(3.f)
	, m_Deceleration(5.f)
	, m_MoveDistance(0.f)
	, m_bLoop(true)
	, m_FrontBoneRotation(FRotator::ZeroRotator)
	, m_Joint1BoneRotation(FRotator::ZeroRotator)
	, m_Joint2BoneRotation(FRotator::ZeroRotator)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���b�V���̃A�^�b�`
	if (m_pTrainMesh)
	{
		RootComponent = m_pTrainMesh;
	}
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pTrainMesh)
	{
		m_FrontBoneRotation = m_pTrainMesh->GetBoneRotationByName(TEXT("Front"), EBoneSpaces::WorldSpace);
		m_FrontBoneRotation.Yaw -= 90.096451f;
		m_Joint1BoneRotation = m_pTrainMesh->GetBoneRotationByName(TEXT("joint1"), EBoneSpaces::WorldSpace);
		m_Joint1BoneRotation.Yaw -= 90.096451f;
		m_Joint2BoneRotation = m_pTrainMesh->GetBoneRotationByName(TEXT("joint2"), EBoneSpaces::WorldSpace);
		m_Joint2BoneRotation.Yaw -= 90.096451f;
	}
}

// Called every frame
void ATrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//���x�X�V����
	UpdateSpeed(DeltaTime);

	//�ړ��X�V����
	UpdateMove(DeltaTime);

	//��]�X�V����
	UpdateRotation(DeltaTime);
}

//���x�̍X�V
void ATrain::UpdateSpeed(const float& DeltaTime)
{
	m_CurrentSpeed = FMath::Lerp(m_CurrentSpeed, m_MaxSpeed, DeltaTime * m_Acceleration);
}

//�ړ��̍X�V
void ATrain::UpdateMove(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (!m_pTrainMesh) { return; }
	
	//�i�񂾋������X�V
	float Speed = m_CurrentSpeed * DeltaTime;
	m_MoveDistance += Speed;

	//�X�v���C���̍��W���擾
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, m_bLoop);

	//���W�Ɉړ�
	m_pTrainMesh->SetBoneLocationByName(TEXT("Front"), NewLocation, EBoneSpaces::WorldSpace);
}

//��]�X�V����
void ATrain::UpdateRotation(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (!m_pTrainMesh) { return; }
	
	//�e�ԗ��̃{�[�����W���擾
	FVector FrontBoneLocation = m_pTrainMesh->GetBoneLocationByName(TEXT("Front"), EBoneSpaces::WorldSpace);
	FVector Joint1BoneLocation = m_pTrainMesh->GetBoneLocationByName(TEXT("joint1"), EBoneSpaces::WorldSpace);
	FVector Joint2BoneLocation = m_pTrainMesh->GetBoneLocationByName(TEXT("joint2"), EBoneSpaces::WorldSpace);

	//�{�[���̋��������߂�
	float Dist1 = FMath::Abs(FVector::Dist(FrontBoneLocation, Joint1BoneLocation));
	float Dist2 = FMath::Abs(FVector::Dist(Joint1BoneLocation, Joint2BoneLocation));

	//�e�{�[���̉�]�ʂ����߂�
	FRotator FrontRotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance, m_bLoop);
	FRotator Joint1Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - Dist1, m_bLoop);
	FRotator Joint2Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - (Dist1 + Dist2), m_bLoop);

	//�e�{�[������]������
	m_pTrainMesh->SetBoneRotationByName(TEXT("Front"), FrontRotation + m_FrontBoneRotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint1"), Joint1Rotation + m_Joint1BoneRotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint2"), Joint2Rotation + m_Joint2BoneRotation, EBoneSpaces::WorldSpace);
}