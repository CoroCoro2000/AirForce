//-----------------------------------------------------------------------------------------------------------------
// ファイル名		:RingManager.cpp
// 概要				:リングを管理するアクタークラス
// 作成日			:2021/08/20
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/08/20	池村凌太		:	ゲーム開始時にすべてのリングに番号を割り当てる処理を作成
//						:2021/08/20	池村凌太		:	リングがくぐられたら配列から削除する処理を作成
//-----------------------------------------------------------------------------------------------------------------

//インクルード
#include "RingManager.h"
#include "GameManager.h"
#include "Ring.h"
#include "DroneBase.h"
#include "GameUtility.h"
#include "Kismet/GameplayStatics.h"

//コンストラクタ
ARingManager::ARingManager()
	: m_MaxRingCount(0)
	, m_RingCount(0)
	, m_RingDrawUpNumber(5)
	, m_pDrone(NULL)
	, m_pGameManager(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ呼ばれる処理
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//配置したリングの数を取得
	m_MaxRingCount = (int)m_pChildRings.Num();
	m_RingCount = m_MaxRingCount;

	//TArrayに格納された若い要素のリングから順に番号付けする
	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			m_pChildRings[index]->SetRingNumber(index);
		}
	}

	//ドローンとゲームマネージャーを検索し、情報を取得
	TSubclassOf<AActor> findClass;
	findClass = AActor::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), findClass, actors);

	if ((int)actors.Num() > 0)
	{
		for (AActor* pActor : actors)
		{
			if (pActor->ActorHasTag(TEXT("GameManager")))
			{
				m_pGameManager = Cast<AGameManager>(pActor);
			}
			if (pActor->ActorHasTag(TEXT("Drone")))
			{
				m_pDrone = Cast<ADroneBase>(pActor);
			}
		}
	}
}

//毎フレーム呼ばれる処理
void ARingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//リング情報を更新
	UpdateRingInfo();
}

//リングを描画するかどうかの判定
bool ARingManager::IsDraw(const int& _ringIndex) const
{
	return (m_pChildRings[_ringIndex]->GetRingNumber() - m_pDrone->GetRingAcquisition() <= m_RingDrawUpNumber) ? true : false;
}

//リング情報の更新処理
void ARingManager::UpdateRingInfo()
{
	if (!m_pDrone) { return; }

	//リングの数が0以下なら
	if (m_RingCount <= 0)
	{
		if (m_pGameManager)
		{
			m_pGameManager->SetIsGoal(true);
		}
	}

	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			//リングの状態更新
			m_pChildRings[index]->SetActivate(IsDraw(index));

			//通過した瞬間
			if (m_pChildRings[index]->IsPassBegin())
			{
				//リングの数を減らす
				--m_RingCount;
				m_pChildRings[index]->SetPassBegin(false);
				UE_LOG(LogTemp, Warning, TEXT("m_RingCount%i"), m_RingCount);
			}

			//通過されたリングにドローンの座標を渡す
			if (m_pChildRings[index]->IsPassed())
			{
				m_pChildRings[index]->SetDroneLocation(m_pDrone->GetActorLocation());
			}

			//リングが破壊されたら配列から削除する
			if (m_pChildRings[index]->IsDestroy())
			{
				m_pChildRings.RemoveAt(index);
			}
		}
	}
}
