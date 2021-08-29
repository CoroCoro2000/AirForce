//--------------------------------------------------------------------------------------------
// ファイル名		:CheckPointActor.h
// 概要				:レースモード時のチェックポイントとして扱うアクター
// 作成日			:2021/08/27
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------

#include "CheckPointActor.h"
#include "Components/BoxComponent.h"
#include "PlayerDrone.h"

// Sets default values
ACheckPointActor::ACheckPointActor()
	: m_pCheckPointCollision(NULL)
	, m_pNextCheckPointActor(NULL)
{
 	//毎フレーム更新する必要がないため、Tick処理を切る
	PrimaryActorTick.bCanEverTick = false;

	//ボックスコンポーネント生成
	m_pCheckPointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointCollision"));
	if (m_pCheckPointCollision)
	{
		RootComponent = m_pCheckPointCollision;
	}

	//チェックポイントのタグを追加
	Tags.Add(TEXT("CheckPoint"));
}

// Called when the game starts or when spawned
void ACheckPointActor::BeginPlay()
{
	Super::BeginPlay();
	
	//コリジョンがオーバーラップした時呼び出すイベント関数を登録
	if (m_pCheckPointCollision)
	{
		m_pCheckPointCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckPointActor::OnComponentOverlapBegin);
	}
}

// Called every frame
void ACheckPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//オブジェクトがオーバーラップした時呼ばれるイベント関数
void ACheckPointActor::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//オーバーラップしたアクターのタグがDroneなら
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			APlayerDrone* pPlayerDrone = Cast<APlayerDrone>(OtherActor);

			if (pPlayerDrone)
			{
				//プレイヤーが指しているチェックポイントのアドレスが自身のアドレスと同じなら次のチェックポイント情報を渡し、削除する
				if (pPlayerDrone->GetCheckPoint() == this)
				{
					pPlayerDrone->SetNextCheckPoint(m_pNextCheckPointActor);
					Destroy();
				}
			}
		}
	}
}