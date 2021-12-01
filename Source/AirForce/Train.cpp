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
	UE_LOG(LogTemp, Warning, TEXT("Speed[%f]"), m_CurrentSpeed);
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

	//1���ڂ���2���ڂ̃{�[���̋��������߂�
	float Dist1 = FVector::Dist(FrontBoneLocation, Joint1BoneLocation);
	float Dist2 = Dist1 + FVector::Dist(Joint1BoneLocation, Joint2BoneLocation);

	//�e�{�[���̉�]�ʂ����߂�
	FRotator FrontRotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance, m_bLoop);
	FRotator Joint1Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - Dist1, m_bLoop);
	FRotator Joint2Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - Dist2, m_bLoop);

	//�e�{�[������]������
	m_pTrainMesh->SetBoneRotationByName(TEXT("Front"), FrontRotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint1"), Joint1Rotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint2"), Joint2Rotation, EBoneSpaces::WorldSpace);
}