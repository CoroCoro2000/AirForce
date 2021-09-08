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

	//メッシュ生成
	m_pGateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	if (m_pGateMesh)
	{
		RootComponent = m_pGateMesh;
	}

	//コリジョン生成
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
	
	//オーバーラップ開始時に実行するイベント関数を登録
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

//オーバーラップ開始時に呼び出されるイベント関数
void ACheckPoint::OnComponentOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor)
	{
		//オーバーラップした対象がドローンなら通過フラグを立てる
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			m_bPassed = true;
		}
	}
}