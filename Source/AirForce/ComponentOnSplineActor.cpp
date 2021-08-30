//--------------------------------------------------------------------------------------------
// ファイル名		:ComponentOnSplineActor.cpp
// 概要				:スプライン上にコンポーネントを自動生成するアクタークラス
// 作成日			:2021/08/30
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------
//インクルード
#include "ComponentOnSplineActor.h"
#include "Components/SplineComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AComponentOnSplineActor::AComponentOnSplineActor()
	: m_pSpline(NULL)
	, m_ComponentType(EComponent::Niagara)
	, m_pNiagaraEffect(NULL)
	, m_ComponentCount(1)
	, m_ComponentRelativeRotation(FRotator::ZeroRotator)
{
	//処理はエディタ上でしか実行しない為、Tickは無効にする
	PrimaryActorTick.bCanEverTick = false;

	//スプライン生成
	m_pSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	if (m_pSpline)
	{
		RootComponent = m_pSpline;
	}
}

// Called when the game starts or when spawned
void AComponentOnSplineActor::BeginPlay()
{
	Super::BeginPlay();

	if (m_ComponentType == EComponent::Niagara)
	{
		//スプライン上にNiagaraエフェクトを生成
		SpawnNiagaraOnSpline();
	}
}

// Called every frame
void AComponentOnSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//スプライン上に生成するNiagaraエフェクト情報の更新
void AComponentOnSplineActor::SpawnNiagaraOnSpline()
{
	if (!m_pSpline) { return; }
	if (!m_pNiagaraEffect) { return; }
	if (m_ComponentCount <= 0) { return; }

	//スプラインの長さを取得
	const float splineLength = m_pSpline->GetSplineLength();

	for (int index = 0; index < m_ComponentCount; ++index)
	{
		//スプライン上のどの位置にエフェクトを生成するか決める
		const float rate = (float)index / (float)m_ComponentCount;
		const float distance = rate * splineLength;
		const FVector initLocation = m_pSpline->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
		FRotator initRotation = m_pSpline->GetRotationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World) + m_ComponentRelativeRotation;

		//エフェクトコンポーネント生成
		m_pNiagaraEffectComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, m_pNiagaraEffect, initLocation, initRotation));
	}
}