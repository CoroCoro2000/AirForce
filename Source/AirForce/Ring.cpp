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
#include "PlayerDrone.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintFunctionUtility.h"

//コンストラクタ
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pNiagaraEffectComp(NULL)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.5f)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_PassedSceleMax(3.f)
	, m_pPassedDrone(NULL)
	, m_bIsUnRotation(false)
	, m_HSV(30.f, 40.f, 30.f)
	, m_InitialTransform(FTransform(FQuat::Identity, FVector::ZeroVector, FVector::OneVector))
	, m_RingHitSE(NULL)
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

	//初期のトランスフォームを保持
	m_InitialTransform = GetActorTransform();

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
		else
		{
			Reset();
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

	//リングのスケール
	FVector RingScale = m_InitialTransform.GetScale3D();

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
		SetActorScale3D(RingScale * ScaleMultiplier);
	}
	//リングが通過されたら大きくする
	else
	{
		if (m_pPassedDrone)
		{
			const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);

			//経過率で大きさを変える
			FVector Scale = FMath::Lerp(RingScale, RingScale * 0.05f, elapsedRate);
			SetActorScale3D(Scale);

			FRotator TargetRotation = m_bIsUnRotation ?
				m_pPassedDrone->GetBodyMeshRotation() * -1.f :
				m_pPassedDrone->GetBodyMeshRotation();
			
			//徐々に座標と回転をプレイヤーに合わせる
			SetActorLocationAndRotation(
				FMath::Lerp(GetActorLocation(), m_pPassedDrone->GetActorLocation(), elapsedRate),
				FMath::Lerp(GetActorRotation(), TargetRotation, elapsedRate));
		}
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
		float ColorScale = m_HSV.R;
		ColorScale = FMath::Lerp(m_HSV.R, m_HSV.R * 0.5f, elapsedRate);

		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(FLinearColor(ColorScale, m_HSV.G, m_HSV.B).HSVToLinearRGB()));
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
		FLinearColor PaticleColor = m_HSV;
		PaticleColor.B = 2.f;
		PaticleColor = FLinearColor::LerpUsingHSV(PaticleColor, PaticleColor * 0.5f, DeltaTime * 0.5f);
		m_pNiagaraEffectComp->SetVariableLinearColor(TEXT("User.Color"), FVector(PaticleColor.HSVToLinearRGB()));
	}
}

//リングが逆回転するかのフラグ
bool ARing::IsUnRotation()
{
	if (!m_pPassedDrone) { return false; }

	//ドローンの向きに合わせてリングの向きを変更する
	FRotator RingRotation = GetActorRotation();
	FRotator DroneRotation = m_pPassedDrone->GetBodyMeshRotation();
	FVector DroneRotationVector = DroneRotation.Vector();

	FVector RotationVector = RingRotation.RotateVector(DroneRotationVector);
	FVector UnRotationVector = RingRotation.UnrotateVector(DroneRotationVector);

	//逆回転のほうがドローンの回転に近ければtrueを返す
	return FVector::Dist(RotationVector, DroneRotationVector) > FVector::Dist(UnRotationVector, DroneRotationVector);
}

//リングの初期化
void ARing::Reset()
{
	m_bIsPassed = false;
	m_MakeInvisibleCnt = 0.f;
	m_HSV = FLinearColor(30.f, 40.f, 30.f);
	float ScrollSpeed = 0.5f;
	m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);
	SetActorTransform(m_InitialTransform);
	m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), 1.f);
	m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_pPassedDrone = NULL;
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
				m_bIsUnRotation = IsUnRotation();

				//マテリアルの回転速度を上げる
				float ScrollSpeed = 1.f;
				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);

				m_pPassedDrone = Cast<ADroneBase>(OtherActor);
				//エフェクトの再生
				m_pNiagaraEffectComp->Activate();
				//SEの再生
				UGameplayStatics::PlaySound2D(GetWorld(), m_RingHitSE);
				//リングの当たり判定を切る
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}