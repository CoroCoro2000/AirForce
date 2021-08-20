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
#include "Ring.h"
#include "DroneBase.h"
#include "Utility/GameUtility.h"

//コンストラクタ
ARingManager::ARingManager()
	: m_MaxRingCount(0)
	, m_RingDrawUpNumber(5)
	, m_pDrone(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ呼ばれる処理
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//ドローンを検索し、情報を取得
	AActor* FindActor = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (FindActor)
	{
		m_pDrone = Cast<ADroneBase>(FindActor);
	}

	//配置したリングの数を取得
	m_MaxRingCount = (int)m_pChildRings.Num();

	//TArrayに格納された若い要素のリングから順に番号付けする
	int index = 1;
	for (ARing* pRing : m_pChildRings)
	{
		if (pRing)
		{
			pRing->SetRingNumber(index);
		}
		++index;
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

	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			//リングの状態更新
			m_pChildRings[index]->SetActivate(IsDraw(index));

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
