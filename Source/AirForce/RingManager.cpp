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
#include "Kismet/GameplayStatics.h"

//コンストラクタ
ARingManager::ARingManager()
	: m_pDrone(NULL)
	, m_pGameManager(NULL)
	, m_ColorState(ECOLOR_STATE::RED)
	, m_Color(FLinearColor::Red)
{
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ呼ばれる処理
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//リングの色の初期設定
	//InitializeRingColor();

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

	UpdateColor(DeltaTime);
	if (m_pGameManager && m_pChildRings[0])
	{
		m_pGameManager->SetIsGoal(m_pChildRings[0]->GetIsPassed());
	}
	
}

//リングの色の初期設定
void ARingManager::InitializeRingColor()
{
	//リングの数を数えて順番にグラデーションをかける
	TArray<TEnumAsByte<ECOLOR_STATE::Type>> RingsColorState;
	int RingCount = (int)m_pChildRings.Num();
	for (int index = 0; index < RingCount; ++index)
	{
		//若い要素のリングから順に、全体の何％の位置にあるか調べる
		float rate = FMath::Clamp((float)index / (float)RingCount, 0.f, 1.f);
		//全体の割合からどの色でグラデーションをかけるか設定する
		RingsColorState.Add(TEnumAsByte<ECOLOR_STATE::Type>(int32(ECOLOR_STATE::NUM * rate)));
	}

	//7色のステートのうちどれに属しているか調べる
	TArray<FLinearColor> RingsColor;
	RingsColor.Reserve(m_pChildRings.Num());
	for (int ColorNum = 0; ColorNum < ECOLOR_STATE::NUM; ++ColorNum)
	{
		//見つかったステートの数でグラデーションさせる
		TArray<TEnumAsByte<ECOLOR_STATE::Type>> FoundStates = RingsColorState.FilterByPredicate([&](const TEnumAsByte<ECOLOR_STATE::Type>& _colorState)
			{
				return TEnumAsByte<ECOLOR_STATE::Type>(int32(ColorNum)) == _colorState;
			});

		int ColorStateNum = (int)FoundStates.Num();
		for (int index = 0; index < ColorStateNum; ++index)
		{
			float rate = FMath::Clamp((float)((float)index / (float)ColorStateNum), 0.f, 1.f);
			RingsColor.Insert(LerpGradient(RingsColorState[ColorNum + index], rate), ColorNum + index);
		}
	}

	//リングに設定したカラーを適用
	for (int index = 0; index < RingCount; ++index)
	{
		if (m_pChildRings[index])
		{
			if (m_pChildRings[index]->GetMesh())
			{
				m_pChildRings[index]->GetMesh()->SetVectorParameterValueOnMaterials(TEXT("BaseColor"), FVector(RingsColor[index]));
			}
		}
	}
}

//リングの色ステート更新
FLinearColor ARingManager::UpdateTargetColor()
{
	FLinearColor TargetColor = m_Color;
	switch (m_ColorState)
	{
	case ECOLOR_STATE::RED:
		m_ColorState = (m_Color == LINEARCOLOR_ORANGE ? ECOLOR_STATE::ORANGE: ECOLOR_STATE::RED);
		TargetColor = FLinearColor::Red;
		break;
	case ECOLOR_STATE::ORANGE:
		m_ColorState = (m_Color == FLinearColor::Yellow ? ECOLOR_STATE::YELLOW : ECOLOR_STATE::ORANGE);
		TargetColor = LINEARCOLOR_ORANGE;
		break;
	case ECOLOR_STATE::YELLOW:
		m_ColorState = (m_Color == FLinearColor::Green ? ECOLOR_STATE::GREEN : ECOLOR_STATE::YELLOW);
		TargetColor = FLinearColor::Yellow;
		break;
	case ECOLOR_STATE::GREEN:
		m_ColorState = (m_Color == FLinearColor::Blue ? ECOLOR_STATE::BLUE : ECOLOR_STATE::GREEN);
		TargetColor = FLinearColor::Green;
		break;
	case ECOLOR_STATE::BLUE:
		m_ColorState = (m_Color == LINEARCOLOR_INDIGO ? ECOLOR_STATE::INDIGO : ECOLOR_STATE::BLUE);
		TargetColor = FLinearColor::Blue;
		break;
	case ECOLOR_STATE::INDIGO:
		m_ColorState = (m_Color ==LINEARCOLOR_PURPLE ? ECOLOR_STATE::PURPLE : ECOLOR_STATE::INDIGO);
		TargetColor = LINEARCOLOR_INDIGO;
		break;
	case ECOLOR_STATE::PURPLE:
		m_ColorState = (m_Color == FLinearColor::Red ? ECOLOR_STATE::RED : ECOLOR_STATE::PURPLE);
		TargetColor = LINEARCOLOR_PURPLE;
		break;
	default:
		break;
	}
	return TargetColor;
}

//リングの色更新
void ARingManager::UpdateColor(const float& DeltaTime)
{
	FLinearColor TargetColor = UpdateTargetColor();
	m_Color = FLinearColor::LerpUsingHSV(m_Color, TargetColor, DeltaTime * 3.f);

	for (ARing* pRing : m_pChildRings)
	{
		if (pRing)
		{
			if (pRing->GetMesh())
			{
				pRing->GetMesh()->SetVectorParameterValueOnMaterials(TEXT("RingColor"), FVector(m_Color));
			}
		}
	}
}

//カラーステートとグラデーションの割合で補間
FLinearColor ARingManager::LerpGradient(const TEnumAsByte<ECOLOR_STATE::Type>& _colorState, const float& _progress)
{
	FLinearColor color;
	switch (_colorState)
	{
	case ECOLOR_STATE::RED:
		color = FLinearColor::LerpUsingHSV(FLinearColor::Red, LINEARCOLOR_ORANGE, _progress);
		break;
	case ECOLOR_STATE::ORANGE:
		color = FLinearColor::LerpUsingHSV(LINEARCOLOR_ORANGE, FLinearColor::Yellow, _progress);
		break;
	case ECOLOR_STATE::YELLOW:
		color = FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Green, _progress);
		break;
	case ECOLOR_STATE::GREEN:
		color = FLinearColor::LerpUsingHSV(FLinearColor::Green, FLinearColor::Blue, _progress);
		break;
	case ECOLOR_STATE::BLUE:
		color = FLinearColor::LerpUsingHSV(FLinearColor::Blue, LINEARCOLOR_INDIGO, _progress);
		break;
	case ECOLOR_STATE::INDIGO:
		color = FLinearColor::LerpUsingHSV(LINEARCOLOR_INDIGO, LINEARCOLOR_PURPLE, _progress);
		break;
	case ECOLOR_STATE::PURPLE:
		color = FLinearColor::LerpUsingHSV(LINEARCOLOR_PURPLE, FLinearColor::Red, _progress);
		break;
	default:
		break;
	}
	return color;
}