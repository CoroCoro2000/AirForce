// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalGate.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameManager.h"
#include "PlayerDrone.h"
#include "GameUtility.h"

// Sets default values
AGoalGate::AGoalGate()
	: m_pGateMash(NULL)
	, m_pGoalCollision(NULL)
	, m_pGameManager(NULL)
{
 	//���t���[���X�V�̕K�v���Ȃ��̂�Tick�͐؂�
	PrimaryActorTick.bCanEverTick = false;

	//�Q�[�g�̃��b�V���R���|�[�l���g����
	m_pGateMash = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	if (m_pGateMash)
	{
		RootComponent = m_pGateMash;
	}

	//�S�[���̓����蔻�萶��
	m_pGoalCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalCollision"));
	if (m_pGoalCollision)
	{
		m_pGoalCollision->SetupAttachment(m_pGateMash);
		m_pGoalCollision->ComponentTags.Add(TEXT("Goal"));
	}
}

// Called when the game starts or when spawned
void AGoalGate::BeginPlay()
{
	Super::BeginPlay();
	
	//�I�[�o�[���b�v���̃C�x���g�֐���o�^
	if (m_pGoalCollision)
	{
		m_pGoalCollision->OnComponentBeginOverlap.AddDynamic(this, &AGoalGate::OnComponentOverlapBegin);
	}

	//�Q�[���}�l�[�W���[���������ď���ێ�����
	m_pGameManager = Cast<AGameManager>(CGameUtility::GetActorFromTag(this, TEXT("GameManager")));
}

// Called every frame
void AGoalGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�I�[�o�[���b�v���ɌĂяo�����C�x���g�֐�
void AGoalGate::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�h���[�����S�[������ɓ���������S�[���t���O�𗧂Ă�
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			//�v���C���[�̃��v���C���Đ�����
			APlayerDrone* pPlayer = Cast<APlayerDrone>(OtherActor);
			if (pPlayer)
			{
				pPlayer->InitializeReplay();
				pPlayer->SetisReplay(true);
			}

			if (m_pGameManager)
			{
				m_pGameManager->SetIsGoal(true);
			}
		}
	}
}