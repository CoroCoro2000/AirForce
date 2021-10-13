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
 	//毎フレーム更新の必要がないのでTickは切る
	PrimaryActorTick.bCanEverTick = false;

	//ゲートのメッシュコンポーネント生成
	m_pGateMash = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	if (m_pGateMash)
	{
		RootComponent = m_pGateMash;
	}

	//ゴールの当たり判定生成
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
	
	//オーバーラップ時のイベント関数を登録
	if (m_pGoalCollision)
	{
		m_pGoalCollision->OnComponentBeginOverlap.AddDynamic(this, &AGoalGate::OnComponentOverlapBegin);
	}

	//ゲームマネージャーを検索して情報を保持する
	m_pGameManager = Cast<AGameManager>(CGameUtility::GetActorFromTag(this, TEXT("GameManager")));
}

// Called every frame
void AGoalGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//オーバーラップ時に呼び出されるイベント関数
void AGoalGate::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//ドローンがゴール判定に当たったらゴールフラグを立てる
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			//プレイヤーのリプレイを再生する
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