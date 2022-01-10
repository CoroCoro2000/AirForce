// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pSplineActor(NULL)
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

	//�^�O�̒ǉ�
	Tags.Add(TEXT("Train"));
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
	//���b�V���̏�����
	InitializeMesh();
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

//���b�V���̏�����
void ATrain::InitializeMesh()
{
	//���b�V���R���|�[�l���g����
	for (UStaticMesh* pMesh : m_pMeshes)
	{
		if (pMesh)
		{
			if (UStaticMeshComponent* pTrainMesh = NewObject<UStaticMeshComponent>())
			{
				pTrainMesh->SetStaticMesh(pMesh);
				pTrainMesh->RegisterComponent();
				m_pTrainMeshes.Add(pTrainMesh);
			}
		}
	}
	//�ԗ��̃A�^�b�`
	for (int32 index = 0; index < m_pTrainMeshes.Num(); ++index)
	{
		if (m_pTrainMeshes.IsValidIndex(index))
		{
			if (index == 0)
			{
				RootComponent = m_pTrainMeshes[index];
			}
			else
			{
				int32 PrevIndex = index - 1;
				if (m_pTrainMeshes.IsValidIndex(PrevIndex))
				{
					m_pTrainMeshes[index]->AttachToComponent(m_pTrainMeshes[PrevIndex], FAttachmentTransformRules::KeepRelativeTransform, TEXT("Coupler"));
				}
			}
		}
	}
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

	if (m_bLoop)
	{
		if (m_pSplineActor->GetSpline()->GetSplineLength() <= m_MoveDistance)
		{
			m_MoveDistance = 0.f;
			//�n�_���W�Ɉړ�
			FVector StartLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, false);
			SetActorLocation(StartLocation, true);
		}
	}

	//�X�v���C���̍��W���擾
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, false);
	//���W�Ɉړ�
	SetActorLocation(NewLocation, true);
}

//��]�X�V����
void ATrain::UpdateRotation(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	
}