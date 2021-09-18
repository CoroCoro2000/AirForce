//------------------------------------------------------------------------
// ファイル名		:Ring.cpp
// 概要				:コースの進行先を示すリングのクラス
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "GameUtility.h"
#include "Components/StaticMeshComponent.h"
#include "ColorLightComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DroneBase.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"

//コンストラクタ
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pNiagaraEffectComp(NULL)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.5f)
	, m_pScaleCurve(NULL)
{
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

	if (!m_pRingMesh  || !m_pNiagaraEffectComp) { return; }

	//オーバーラップ開始時に呼ばれるイベント関数を登録
	m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnComponentOverlapBegin);

	//エフェクトコンポーネント初期化
	m_pNiagaraEffectComp->SetRelativeLocation(FVector::ZeroVector);
	m_pNiagaraEffectComp->SetRelativeRotation(FRotator::ZeroRotator);
	m_pNiagaraEffectComp->SetRelativeScale3D(FVector::OneVector);
}

//毎フレーム呼び出される関数
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//カラー更新
	UpdateColor(DeltaTime);

	//トランスフォーム更新
	UpdateTransform(DeltaTime);
}

//リングの色の更新処理
void ARing::UpdateColor(const float& DeltaTime)
{
	if (!m_pRingMesh || !m_pNiagaraEffectComp) { return; }
	
	//リングの色を更新
	//m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("EmissiveColor"), m_pColorLightComp->GetVectorColor());
	
	//リングをプレイヤーがくぐっていたら
	if (m_bIsPassed)
	{
		////エフェクトの色をメッシュと同じ色にする
		//m_pNiagaraEffectComp->SetNiagaraVariableLinearColor(TEXT("User.Color"), m_pColorLightComp->GetVectorColor());

		//完全に見えなくなるまで時間をカウント
		//if (m_MakeInvisibleTime > m_MakeInvisibleCnt)
		//{
		//	m_MakeInvisibleCnt += DeltaTime;
		//	const float MeshOpacity = FMath::Lerp(1.f, 0.f, m_InvisibleCntRate);

		//	//メッシュの不透明度を下げる
		//	m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), MeshOpacity);
		//}
	}
}

//リングのトランスフォーム更新
void ARing::UpdateTransform(const float& DeltaTime)
{
	//if (!m_pRingMesh || !m_pScaleCurve) { return; }

	////リングが通過されたら
	//if (m_bIsPassed)
	//{
	//	//カウントの進行度で座標と大きさを決める
	//	const FVector RingLocation = FMath::Lerp(GetActorLocation(), m_PassedDroneLocation, m_InvisibleCntRate);
	//	const float Scale = m_pScaleCurve->GetFloatValue(m_MakeInvisibleCnt);

	//	//リングを縮めながらプレイヤーを追うように移動する
	//	if (Scale > 0.f)
	//	{
	//		SetActorScale3D(FVector(Scale));
	//		SetActorLocation(RingLocation);
	//	}
	//	else
	//	{
	//		m_bDestroy = true;
	//	}

	//	if (m_bDestroy)
	//	{
	//		//リングが見えなくなったらリングを消す
	//		this->Destroy();
	//	}
	//}
	////リングが通過されていない間
	//else
	//{
	//	////サイン波の幅を設定
	//	//const float WaveWidth = 10.f * GetWorld()->GetTimeSeconds();
	//	////サイン波の大きさを0から1に正規化する
	//	//const float SinWave = FMath::Sin(WaveWidth) * 0.5f + 0.5f;

	//	////サイン波の値でリングの大きさを変える
	//	//const float Scale = FMath::Lerp(1.05f, 0.8f, SinWave);
	//	//m_pRingMesh->SetRelativeScale3D(FVector(Scale));
	//}
}

//リングをアクティブ化する
void ARing::SetActivate(const bool& _isActive)
{
	if (!m_pRingMesh) { return; }

	//メッシュのオーバーラップイベントと可視性のON/OFFを切り替える
	m_pRingMesh->SetGenerateOverlapEvents(true);
	m_pRingMesh->SetHiddenInGame(true);

	//見えていない間は更新の必要がないのでTickを切る
	PrimaryActorTick.bCanEverTick = _isActive;
}

//オーバーラップ開始時に呼ばれる処理
void ARing::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				//m_pNiagaraEffectComp->Activate();
				//リングの当たり判定を切る
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}