//------------------------------------------------------------------------
// ファイル名		:Ring.cpp
// 概要				:コースの進行先を示すリングのクラス
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "GameUtility.h"
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Curves/CurveFloat.h"

//コンストラクタ
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pNiagaraEffectComp(NULL)
	, m_RingScale(1.f)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.5f)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_PassedSceleMax(3.f)
	, m_pPassedDrone(NULL)
	, m_HSV(30.f, 40.f, 30.f)
{
	//毎フレームTickを呼び出すかどうかのフラグ
	PrimaryActorTick.bCanEverTick = true;

	//メッシュ生成
	m_pRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRingMesh"));
	if (m_pRingMesh)
	{
		RootComponent = m_pRingMesh;
	}

	//ナイアガラのエフェクトコンポーネント生成
	m_pNiagaraEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("m_pNiagaraEffectComp"));
	if (m_pNiagaraEffectComp && m_pRingMesh)
	{
		m_pNiagaraEffectComp->SetupAttachment(m_pRingMesh);
	}

	//タグの追加
	Tags.Add(TEXT("Ring"));
}

//ゲーム開始時またはこのクラスのオブジェクトがスポーンされた時１度だけ呼び出される関数
void ARing::BeginPlay()
{
	Super::BeginPlay();

	if (m_pRingMesh)
	{
		//オーバーラップ開始時に呼ばれるイベント関数を登録
		m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnComponentOverlapBegin);
	}

	//リングの大きさを保持
	m_RingScale = GetActorScale().GetAbsMax();
	SetActorScale3D(FVector(m_RingScale));

	m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
}

//毎フレーム呼び出される関数
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_bIsPassed)
	{
		//指定の時間を越えるまで時間を計測
		if (m_MakeInvisibleCnt < m_MakeInvisibleTime)
		{
			m_MakeInvisibleCnt += DeltaTime;
		}
	}

	//リングのサイズ更新
	UpdateScale(DeltaTime);

	//リングのマテリアル更新
	UpdateMaterial(DeltaTime);

	//エフェクトの更新
	UpdateEffect(DeltaTime);
}

//リングのサイズ更新
void ARing::UpdateScale(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	//リングが通過されていない間は一定の周期で大きさを変える
	if (!m_bIsPassed)
	{
		//サイン波の幅を設定
		const float WaveWidth = m_SineWidth * GetWorld()->GetTimeSeconds();
		//サイン波の大きさを0から1に正規化する
		const float SinWave = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
		//サイン波の値でリングの大きさを変える
		float ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, SinWave);

		//新しいスケールを適用
		SetActorScale3D(FVector(m_RingScale * ScaleMultiplier));
	}
	//リングが通過されたら大きくする
	else
	{
		const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
		FVector DroneLocation = m_pPassedDrone->GetActorLocation();
		FVector RingLocation = GetActorLocation();
		FRotator TargetRotation = FRotationMatrix::MakeFromX(DroneLocation - RingLocation).Rotator();

		//経過率で大きさを変える
		float Scale = FMath::Lerp(m_RingScale, m_RingScale * 0.2f, elapsedRate);
		SetActorScale3D(FVector(Scale));

		//徐々に座標と回転をプレイヤーに合わせる
		SetActorLocationAndRotation(
			FMath::Lerp(RingLocation, DroneLocation, elapsedRate),
			FMath::Lerp(GetActorQuat(), TargetRotation.Quaternion(), elapsedRate));
	}
}

//リングのマテリアル更新
void ARing::UpdateMaterial(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	if (m_bIsPassed)
	{
		//くぐられてからの経過率を求める
		const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
		m_HSV = FLinearColor::LerpUsingHSV(m_HSV, FLinearColor(5.f, m_HSV.G, 70.f), elapsedRate);

		m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), 1.f);
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
		m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), 1.f - elapsedRate);
	}
	else
	{
		//サイン波の幅を設定
		const float WaveWidth = m_SineWidth * GetWorld()->GetTimeSeconds();
		//サイン波の大きさを0から1に正規化する
		const float SinWave = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
		//サイン波の値でリングの色相を変える
		m_HSV.R = FMath::Lerp(50.f, 30.f, SinWave);
		m_HSV.B = FMath::Lerp(30.f, 50.f, SinWave);
		
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
	}
}

//リングのエフェクト更新
void ARing::UpdateEffect(const float& DeltaTime)
{
	if (!m_pNiagaraEffectComp || !m_pPassedDrone) { return; }

	if (m_bIsPassed)
	{
		//ドローンに近づく速度
		const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
		FLinearColor EffectColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Blue, elapsedRate);

		//経過時間に合わせて消していく
		m_pNiagaraEffectComp->SetVariableLinearColor(TEXT("User.Color"), EffectColor);
	}

}

//オーバーラップ開始時に呼ばれる処理
void ARing::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent && OtherActor && OtherActor != this)
	{
		//タグがPlayerだった場合
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			//このリングがまだ通過されていない場合
			if (!m_bIsPassed)
			{
				//通過された状態に変更
				m_bIsPassed = true;

				m_pPassedDrone = Cast<ADroneBase>(OtherActor);
				//エフェクトの再生
				m_pNiagaraEffectComp->Activate();
				//リングの当たり判定を切る
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}