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
	, m_bCanMove(true)
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

	m_CurrentSpeed = m_bCanMove ?
		FMath::Lerp(m_CurrentSpeed, m_MaxSpeed, FMath::Clamp(DeltaTime * m_Acceleration, 0.f, 1.f)) :
		FMath::Lerp(m_CurrentSpeed, 0.f, FMath::Clamp(DeltaTime * m_Deceleration, 0.f, 1.f));
}

//�ړ��̍X�V
void ATrain::UpdateMove(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (m_pTrainMeshes.Num() == 0) { return; }

	//�i�񂾋������X�V
	float Speed = m_CurrentSpeed * DeltaTime;
	m_MoveDistance += Speed;

	//�ړ��������X�v���C���̒������z���Ă��Ȃ����m�F����
	CheckMoveDistance();

	//�X�v���C���̍��W���擾
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, false);
	//���W���X�V
	SetActorLocation(NewLocation, true);
}

//��]�X�V����
void ATrain::UpdateRotation(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (m_pTrainMeshes.Num() == 0) { return; }
	
	//�擪�̃��b�V�����܂߂��ԗ��̃��b�V���̔z����쐬
	TArray<UStaticMeshComponent*> pTrainMeshes = m_pTrainMeshes;
	pTrainMeshes.Insert(m_pFrontTrainMesh, 0);
	float RotationSpeed = FMath::Clamp(DeltaTime * 10.f, 0.f, 1.f);
	FRotator PrevRotation = FRotator::ZeroRotator;
	FRotator RotatorCorrection = FRotator::ZeroRotator;

	for (UStaticMeshComponent* pTrainMesh : pTrainMeshes)
	{
		if (pTrainMesh)
		{
			//�e�ԗ����X�v���C���̌����ɍ��킹��
			FRotator NewWorldRotation = m_pSplineActor->GetSpline()->FindRotationClosestToWorldLocation(pTrainMesh->GetComponentLocation(), ESplineCoordinateSpace::World);
			//�q�̎ԗ����e�̉�]�ʂ̉e�����󂯂Ȃ��悤�ɕ␳����
			if (PrevRotation.IsZero())
			{
				PrevRotation = NewWorldRotation;
			}
			else
			{
				RotatorCorrection += (PrevRotation - RotatorCorrection) - NewWorldRotation;
				NewWorldRotation -= RotatorCorrection;
				PrevRotation = NewWorldRotation;
			}

			//��]�̍X�V
			pTrainMesh->SetWorldRotation(FQuat::FastLerp(pTrainMesh->GetComponentQuat(), NewWorldRotation.Quaternion(), RotationSpeed), true);
		}
	}
}

//�X�v���C���̏I�_�ɓ������Ă��邩�m�F���鏈��
void ATrain::CheckMoveDistance()
{
	if (!m_pSplineActor) { return; }

	//�ړ������������X�v���C���̒������z������
	if (m_pSplineActor->GetSpline()->GetSplineLength() <= m_MoveDistance)
	{
		//���[�v�t���O�������Ă�����J�n�n�_�Ɉړ�������
		if (m_bLoop)
		{
			m_MoveDistance = 0.f;
			FVector StartLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, false);
			SetActorLocation(StartLocation, true);
		}
		//���[�v�t���O������Ă���ꍇ�͍폜����
		else
		{
			Destroy();
		}
	}
}

//������
void ATrain::Init()
{
	m_CurrentSpeed = 0.f;
	m_MoveDistance = 0.f;
}