// Fill out your copyright notice in the Description page of Project Settings.

#include "CheckPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckPoint::ACheckPoint()
	: m_pGateMesh(NULL)
	, m_pCheckPointCollision(NULL)
	, m_CheckNumber(0)
	, m_bSameNumberNext(false)
	, m_bPassed(false)
	, m_bActive(false)
	, m_RadiusOfSearchRange(50.f)
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

//チェックポイントの範囲内にいるかどうか判定
bool ACheckPoint::IsWithinRangeOfCheckpoint()const
{
	//範囲内にいるかの判定
	bool inRange = false;

	if (m_bActive)
	{
		//検索範囲の座標を設定
		FVector SearchRangeLocation = GetActorLocation();
		//ヒット結果を格納する配列
		TArray<FHitResult> OutHits;
		//トレースする対象の無視する項目を設定(自身は無視)
		FCollisionQueryParams CollisionParam;
		CollisionParam.AddIgnoredActor(this);

		//球状のレイを飛ばし、範囲内のオブジェクトを取得する
		bool isHit = GetWorld()->SweepMultiByChannel(
			OutHits,
			SearchRangeLocation,
			SearchRangeLocation,
			FQuat::Identity,
			ECollisionChannel::ECC_Pawn,
			FCollisionShape::MakeSphere(m_RadiusOfSearchRange),
			CollisionParam);

		if (isHit)
		{
			//ヒットしたアクターの中にドローンがあればフラグを立てる
			for (const FHitResult& HitResult : OutHits)
			{
				if (HitResult.GetActor())
				{
					if (HitResult.GetActor()->ActorHasTag(TEXT("Drone")))
					{
						inRange = true;

						break;
					}
				}
			}
		}
	}

	return inRange;
}

//オーバーラップ開始時に呼び出されるイベント関数
void ACheckPoint::OnComponentOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor && m_bActive)
	{
		//オーバーラップした対象がドローンなら通過フラグを立てる
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			m_bPassed = true;
			m_bActive = false;
		}
	}
}