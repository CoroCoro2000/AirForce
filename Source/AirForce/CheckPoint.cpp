// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckPoint::ACheckPoint()
	: m_pGateMesh(NULL)
	, m_pCheckPointCollision(NULL)
	, m_bPassed(false)
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

}

//�I�[�o�[���b�v�J�n���ɌĂяo�����C�x���g�֐�
void ACheckPoint::OnComponentOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor)
	{
		//�I�[�o�[���b�v�����Ώۂ��h���[���Ȃ�ʉ߃t���O�𗧂Ă�
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			m_bPassed = true;
		}
	}
}