// Fill out your copyright notice in the Description page of Project Settings.

#include "CheckPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ACheckPoint::ACheckPoint()
	: m_pGateMesh(NULL)
	, m_pCheckPointCollision(NULL)
	, m_CheckNumber(0)
	, m_bSameNumberNext(false)
	, m_bPassed(false)
	, m_bIsDroneInRange(false)
	, m_RadiusOfSearchRange(50.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//���b�V������
	m_pGateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	if (m_pGateMesh)
	{
		RootComponent = m_pGateMesh;
	}

	//�R���W��������
	m_pCheckPointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointCollision"));
	if (m_pCheckPointCollision && m_pGateMesh)
	{
		m_pCheckPointCollision->SetupAttachment(m_pGateMesh);
	}
}

// Called when the game starts or when spawned
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
	//�I�[�o�[���b�v�J�n���Ɏ��s����C�x���g�֐���o�^
	if (m_pCheckPointCollision)
	{
		m_pCheckPointCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnComponentOverlapBegin);
	}
}

// Called every frame
void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�͈͓��̃h���[����T��
	FindDroneInRange();
}

//�͈͓��̃h���[����T������
void ACheckPoint::FindDroneInRange()
{
	//�����͈͂̍��W��ݒ�
	FVector FindRangeLocation = GetActorLocation();
	//�q�b�g���ʂ��i�[����z��
	TArray<FHitResult> OutHits;
	//�g���[�X����Ώۂ̖������鍀�ڂ�ݒ�(���g�͖���)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	//����̃��C���΂��A�͈͓��̃I�u�W�F�N�g���擾����
	bool isHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		FindRangeLocation,
		FindRangeLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(m_RadiusOfSearchRange),
		CollisionParam);

	if (isHit)
	{
		//�q�b�g�����A�N�^�[�̒��Ƀh���[��������΃t���O�𗧂Ă�
		bool isDroneInRange = false;
		for (const FHitResult& HitResult : OutHits)
		{
			if (HitResult.GetActor())
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Drone")))
				{
					isDroneInRange = true;
					break;
				}
			}
		}
		m_bIsDroneInRange = isDroneInRange;
	}
	else
	{
		m_bIsDroneInRange = false;
	}
#ifdef DEBUG_IsWithinRangeOfCheckpoint
	FColor SphereColor = m_bIsDroneInRange ? FColor::Blue : FColor::Yellow;
	DrawDebugSphere(GetWorld(), FindRangeLocation, m_RadiusOfSearchRange, 32, SphereColor);
#endif //DEBUG_IsWithinRangeOfCheckpoint
}

//�I�[�o�[���b�v�J�n���ɌĂяo�����C�x���g�֐�
void ACheckPoint::OnComponentOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor && m_bIsDroneInRange)
	{
		//�I�[�o�[���b�v�����Ώۂ��h���[���Ȃ�ʉ߃t���O�𗧂Ă�
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			m_bPassed = true;
		}
	}
}