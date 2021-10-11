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
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

//コンストラクタ
ARingManager::ARingManager()
	: m_pDrone(NULL)
	, m_pGameManager(NULL)
	, m_ColorState(ECOLOR_STATE::RED)
	, m_RingColor(FLinearColor::Transparent)
	, m_FresnelColor(FLinearColor::Transparent)
	, m_TargetColor(FLinearColor::Transparent)
	, m_ColorTransitionSpeed(3.f)
	, m_DelayTempo(2)
{
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ呼ばれる処理
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//ドローンとゲームマネージャーを検索、保持する
	TSubclassOf<AActor> findClass = AActor::StaticClass();
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

	//カラーステート更新
	UpdateColorState();

	//カラー更新
	UpdateColor(DeltaTime);

	/*if (m_pGameManager && m_pChildRings[0])
	{
		m_pGameManager->SetIsGoal(m_pChildRings[0]->GetIsPassed());
	}*/
	if (m_pChildRings[0]->GetIsPassed())
	{
		m_pGameManager->SetIsGoal(true);
	}
}

//カラーステート更新
void ARingManager::UpdateColorState()
{
	switch (m_ColorState)
	{
	case ECOLOR_STATE::RED:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Red - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::ORANGE : ECOLOR_STATE::RED);
		m_TargetColor = FLinearColor::Red;
		break;
	}
	case ECOLOR_STATE::ORANGE:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(LINEARCOLOR_ORANGE - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::YELLOW : ECOLOR_STATE::ORANGE);
		m_TargetColor = LINEARCOLOR_ORANGE;
		break;
	}
	case ECOLOR_STATE::YELLOW:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Yellow - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::GREEN : ECOLOR_STATE::YELLOW);
		m_TargetColor = FLinearColor::Yellow;
		break;
	}
	case ECOLOR_STATE::GREEN:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Green - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::BLUE : ECOLOR_STATE::GREEN);
		m_TargetColor = FLinearColor::Green;
		break;
	}
	case ECOLOR_STATE::BLUE:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Blue - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::INDIGO : ECOLOR_STATE::BLUE);
		m_TargetColor = FLinearColor::Blue;
		break;
	}
	case ECOLOR_STATE::INDIGO:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(LINEARCOLOR_INDIGO - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::PURPLE : ECOLOR_STATE::INDIGO);
		m_TargetColor = LINEARCOLOR_INDIGO;
		break;
	}
	case ECOLOR_STATE::PURPLE:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(LINEARCOLOR_PURPLE - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::RED : ECOLOR_STATE::PURPLE);
		m_TargetColor = LINEARCOLOR_PURPLE;
		break;
	}
	default:
		break;
	}
}

//リングの色更新
void ARingManager::UpdateColor(const float& DeltaTime)
{
	if ((int)m_pChildRings.Num() <= 0) { return; }

	//現在の色からターゲットに向けて色を変えていく
	m_RingColor = FLinearColor::LerpUsingHSV(m_RingColor, m_TargetColor, DeltaTime * m_ColorTransitionSpeed);
	//フレネルの色は3テンポ先の色にする
	int32 FresnelState = (m_ColorState + m_DelayTempo) % ECOLOR_STATE::NUM;
	m_FresnelColor = FLinearColor::LerpUsingHSV(m_FresnelColor, GetTargetColor(FresnelState), DeltaTime * m_ColorTransitionSpeed);
	//マネージャーが管理しているすべてのリングのマテリアルに現在の色を適用する
	for (ARing* pRing : m_pChildRings)
	{
		if (pRing)
		{
			if (pRing->GetMesh())
			{
				pRing->GetMesh()->SetVectorParameterValueOnMaterials(TEXT("RingColor"), FVector(m_RingColor));
				pRing->GetMesh()->SetVectorParameterValueOnMaterials(TEXT("FresnelColor"), FVector(m_FresnelColor));
			}

			//リングが通過されていたらエフェクトの色も変化させる
			if (pRing->GetIsPassed())
			{
				if (pRing->GetEffectComponent())
				{
					pRing->GetEffectComponent()->SetVariableLinearColor(TEXT("User.Color"), m_RingColor);
				}
			}
		}
	}
}

//ステートからカラーターゲットを取得
FLinearColor ARingManager::GetTargetColor(const int32& _colorIndex)
{
	FLinearColor TargetColor;
	switch (_colorIndex)
	{
	case ECOLOR_STATE::RED:
		TargetColor = FLinearColor::Red;
		break;
	case ECOLOR_STATE::ORANGE:
		TargetColor = LINEARCOLOR_ORANGE;
		break;
	case ECOLOR_STATE::YELLOW:
		TargetColor = FLinearColor::Yellow;
		break;
	case ECOLOR_STATE::GREEN:
		TargetColor = FLinearColor::Green;
		break;
	case ECOLOR_STATE::BLUE:
		m_TargetColor = FLinearColor::Blue;
		break;
	case ECOLOR_STATE::INDIGO:
		TargetColor = LINEARCOLOR_INDIGO;
		break;
	case ECOLOR_STATE::PURPLE:
		TargetColor = LINEARCOLOR_PURPLE;
		break;
	default:
		break;
	}
	return TargetColor;
}