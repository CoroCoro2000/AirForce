// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pTrainMesh(NULL)
	, m_pSplineActor(NULL)
	, m_MaxSpeed(50.f)
	, m_CurrentSpeed(0.f)
	, m_Acceleration(3.f)
	, m_Deceleration(5.f)
	, m_MoveDistance(0.f)
	, m_bLoop(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���b�V������
	m_pTrainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TrainMesh"));
	if (m_pTrainMesh)
	{
		RootComponent = m_pTrainMesh;
	}
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pSplineActor)
	{
		if (USplineComponent* pSpline = m_pSplineActor->GetSpline())
		{
			
		}
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
	
	//�i�񂾋������X�V
	float Speed = m_CurrentSpeed * DeltaTime;
	m_MoveDistance += Speed;

	//�X�v���C���̍��W���擾
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, m_bLoop);

	//���W�Ɉړ�
	SetActorLocation(NewLocation, true);
}

//��]�X�V����
void ATrain::UpdateRotation(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	
	//�X�v���C���̍��W���擾
	FRotator NewRotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance, m_bLoop);

	//���W�Ɉړ�
	SetActorRotation(NewRotation);
}