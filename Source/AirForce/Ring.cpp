//------------------------------------------------------------------------
// ファイル名		:Ring.cpp
// 概要				:コースの進行先を示すリングのクラス
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "Utility/GameUtility.h"
#include "Components/StaticMeshComponent.h"
#include "ColorLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DroneBase.h"
#include "PlayerDrone.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"

//コンストラクタ
ARing::ARing()
	: m_pColorLightComp(NULL)
	, m_pRingMesh(NULL)
	, m_RingNumber(0)
	, m_RingDrawUpNumber(0)
	, m_pNiagaraEffectComp(NULL)
	, m_bIsPassed(false)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(3.f)
	, m_pScaleCurve(NULL)
{
	//カラーコンポーネント生成
	m_pColorLightComp = CreateDefaultSubobject<UColorLightComponent>(TEXT("m_pColorLightComp"));

	//メッシュ生成
	m_pRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRingMesh"));
	if (m_pRingMesh)
	{
		RootComponent = m_pRingMesh;
	}

	//ナイアガラのエフェクトコンポーネント生成
	m_pNiagaraEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("m_pNiagaraEffectComp"));
	if (m_pNiagaraEffectComp)
	{
		m_pNiagaraEffectComp->SetupAttachment(m_pRingMesh);
	}

	//スケールの大きさの値を取り出すカーブオブジェクト生成
	ConstructorHelpers::FObjectFinder<UCurveFloat> pRingScaleCurve(TEXT("CurveFloat'/Game/Effect/Ring/Curve/RingScale_Curve.RingScale_Curve'"));
	if (pRingScaleCurve.Succeeded())
	{
		m_pScaleCurve = pRingScaleCurve.Object;
	}

	//毎フレームTickを呼び出すかどうかのフラグ
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時またはこのクラスのオブジェクトがスポーンされた時１度だけ呼び出される関数
void ARing::BeginPlay()
{
	Super::BeginPlay();

	if (!m_pRingMesh || !m_pColorLightComp || !m_pNiagaraEffectComp) { return; }

	//オーバーラップ開始時に呼ばれるイベント関数を登録
	m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnOverlapBegin);

	//ドローンを検索し、情報を保持する
	AActor* FindDrone = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (FindDrone)
	{
		m_pDrone = Cast<APlayerDrone>(FindDrone);
	}

	m_pColorLightComp->InitializeMaterialParameter(m_pRingMesh, true);
	m_pColorLightComp->Activate(true);

	m_pNiagaraEffectComp->SetRelativeLocation(FVector::ZeroVector);
	m_pNiagaraEffectComp->SetRelativeRotation(FRotator::ZeroRotator);
	m_pNiagaraEffectComp->SetRelativeScale3D(FVector::OneVector);
}

//毎フレーム呼び出される関数
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pColorLightComp && m_pRingMesh)
	{
		//リングの色を更新
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("EmissiveColor"), m_pColorLightComp->GetVectorColor());
	}
	m_pRingMesh->SetGenerateOverlapEvents(isDraw());
	m_pRingMesh->SetHiddenInGame(!isDraw());
	//リングが通過されたなら
	if (m_bIsPassed)
	{
		//エフェクトの色をメッシュと同じ色にする
		m_pNiagaraEffectComp->SetNiagaraVariableLinearColor(TEXT("User.Color"), m_pColorLightComp->GetVectorColor());
		if (m_MakeInvisibleTime > m_MakeInvisibleCnt)
		{
			//完全に見えなくなるまで時間をカウント
			m_MakeInvisibleCnt += DeltaTime;
			if (m_pRingMesh && m_pNiagaraEffectComp && m_pScaleCurve && m_pDrone)
			{
				//メッシュとエフェクトの不透明度を下げていく
				const float MeshCountRate = FMath::Clamp(m_MakeInvisibleCnt / (m_MakeInvisibleTime * 0.5f), 0.f, 1.f);
				const float EffectCountRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
				const float MeshOpacity = FMath::Lerp(1.f, 0.f, MeshCountRate);
				const float EffectOpacity = FMath::Lerp(1.f, 0.f, EffectCountRate);
				const FVector RingLocation = FMath::Lerp(GetActorLocation(), m_pDrone->GetActorLocation(), EffectCountRate);
				const float Scale = m_pScaleCurve->GetFloatValue(m_MakeInvisibleCnt);

				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), MeshOpacity);
				m_pNiagaraEffectComp->SetNiagaraVariableFloat(TEXT("User.Opacity"), EffectOpacity);

				//リングを小さくしていく

				if (Scale > 0.f)
				{
					SetActorScale3D(FVector(Scale));
					SetActorLocation(RingLocation);
				}
				
				if (EffectOpacity <= 0.f)
				{
					//リングが見えなくなったら破棄する
					this->Destroy();
				}
			}
		}
	}
}

bool ARing::isDraw()
{
	if (!m_pDrone)
	{
		return false;
	}

	return  (m_RingNumber - m_pDrone->GetRingAcquisition() <= m_RingDrawUpNumber ? true : false);
}

//オーバーラップ開始時に呼ばれる処理
void ARing::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//タグがPlayerだった場合
	if (OtherActor->ActorHasTag(TEXT("Drone")))
	{
		//このリングがまだ通過されていない場合
		if (!m_bIsPassed)
		{
			if (m_pNiagaraEffectComp && m_pRingMesh)
			{
				//通過された状態に変更
				m_bIsPassed = true;
				//エフェクトの再生
				m_pNiagaraEffectComp->Activate();
				//リングの当たり判定を切る
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

