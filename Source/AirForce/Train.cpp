// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pFrontTrainMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Front")))
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

	if (m_pFrontTrainMesh)
	{
		RootComponent = m_pFrontTrainMesh;
	}

	m_pTrainMeshes.Empty();

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
	for (int32 index = 0; index < m_pMeshes.Num(); ++index)
	{
		//���b�V���R���|�[�l���g����
		UStaticMeshComponent* pTrainMesh = NewObject<UStaticMeshComponent>(this);
		if (pTrainMesh)
		{
			pTrainMesh->RegisterComponent();
			m_pTrainMeshes.Add(pTrainMesh);

			if (m_pMeshes.IsValidIndex(index))
			{
				pTrainMesh->SetStaticMesh(m_pMeshes[index]);
			}

			//�ԗ��̃A�^�b�`
			const int32 PrevIndex = index - 1;
			if (m_pTrainMeshes.IsValidIndex(PrevIndex))
			{
				m_pTrainMeshes[index]->AttachToComponent(m_pTrainMeshes[PrevIndex], FAttachmentTransformRules::KeepRelativeTransform, TEXT("Coupler"));
			}
			else
			{
				m_pTrainMeshes[index]->AttachToComponent(m_pFrontTrainMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Coupler"));
			}
		}
	}
}

//���x�̍X�V
void ATrain::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (m_pTrainMeshes.Num() == 0) { return; }

	m_CurrentSpeed = FMath::Lerp(m_CurrentSpeed, m_MaxSpeed, DeltaTime * m_Acceleration);
}

//�ړ��̍X�V
void ATrain::UpdateMove(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (m_pTrainMeshes.Num() == 0) { return; }

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
	if (m_pTrainMeshes.Num() == 0) { return; }
	
	TArray<UStaticMeshComponent*> pTrainMeshes = m_pTrainMeshes;
	pTrainMeshes.Insert(m_pFrontTrainMesh, 0);

	for (UStaticMeshComponent* pTrainMesh : pTrainMeshes)
	{
		if (pTrainMesh)
		{
			FRotator NewRotation = m_pSplineActor->GetSpline()->FindDirectionClosestToWorldLocation(pTrainMesh->GetComponentLocation(), ESplineCoordinateSpace::World).ToOrientationRotator();
			NewRotation.Yaw += 90.f;
			pTrainMesh->SetWorldRotation(NewRotation, true);
		}
	}
}