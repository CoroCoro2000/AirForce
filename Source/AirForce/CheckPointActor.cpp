//--------------------------------------------------------------------------------------------
// ファイル名		:CheckPointActor.h
// 概要				:レースモード時のチェックポイントとして扱うアクター
// 作成日			:2021/08/27
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------

#include "CheckPointActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckPointActor::ACheckPointActor()
	: m_CheckPointCollision(NULL)
	, m_CheckPointNumber(0)
	, m_pNextCheckPointActor(NULL)
{
 	//毎フレーム更新する必要がないため、Tick処理を切る
	PrimaryActorTick.bCanEverTick = false;

	//ボックスコンポーネント生成
	m_CheckPointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointCollision"));
	if (m_CheckPointCollision)
	{
		RootComponent = m_CheckPointCollision;
	}

	//チェックポイントのタグを追加
	Tags.Add(TEXT("CheckPoint"));
}

// Called when the game starts or when spawned
void ACheckPointActor::BeginPlay()
{
	Super::BeginPlay();
	

	//次のチェックポイントの番号を設定
	if (m_pNextCheckPointActor)
	{
		m_pNextCheckPointActor->SetNumber(m_CheckPointNumber + 1);
	}
}

// Called every frame
void ACheckPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

