//------------------------------------------------------------------------
// ファイル名		:Ring.cpp
// 概要				:コースの進行先を示すリングのクラス
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//コンストラクタ
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pFollowingEffectDronePairs()
	, m_pEffect(NULL)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_SineCurveValue(0.f)
	, m_RingScale(1.f)
	, m_HSV(30.f, 40.f, 30.f)
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
		m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnComponentBeginOverlap);
	}

	//リングの初期スケールを保持
	m_RingScale = GetActorScale3D().GetMax();
}

//毎フレーム呼び出される関数
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//サインカーブの値を更新
	UpdateSineCurve(DeltaTime);

	//リングのサイズ更新
	UpdateScale(DeltaTime);

	//リングのマテリアル更新
	UpdateMaterial(DeltaTime);

	//エフェクトの更新
	UpdateEffect(DeltaTime);
}

//サインカーブの値を更新
void ARing::UpdateSineCurve(const float& DeltaTime)
{
	if (const UWorld* pWorld = GetWorld())
	{
		//サイン波の幅を設定
		const float WaveWidth = m_SineWidth * pWorld->GetTimeSeconds();
		//サイン波の大きさを0から1に正規化する
		m_SineCurveValue = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
	}
}

//リングのサイズ更新
void ARing::UpdateScale(const float& DeltaTime)
{
	//通過フラグが立っているならリングを拡大する
	if (m_bIsPassed)
	{

	}
	//通過フラグが立っていないときはSin波の間隔で拡大縮小を繰り返す
	else
	{
		//サイン波の値でリングの大きさを変える
		float ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, m_SineCurveValue);
		//新しいスケールを適用
		SetActorScale3D(FVector(m_RingScale * ScaleMultiplier));
	}
}

//リングのマテリアル更新
void ARing::UpdateMaterial(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	if (m_bIsPassed)
	{

	}
	//通過フラグが立っていない間はメッシュの拡縮に合わせてリングの色を変える
	else
	{
		m_HSV.R = FMath::Lerp(50.f, 30.f, m_SineCurveValue);
		m_HSV.B = FMath::Lerp(30.f, 50.f, m_SineCurveValue);
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
	}
}

//リングのエフェクト更新
void ARing::UpdateEffect(const float& DeltaTime)
{
	if (m_pFollowingEffectDronePairs.Num() <= 0) { return; }

	//エフェクトが消えていたら配列から削除するため、逆順にfor文を回す
	for (int32 index = m_pFollowingEffectDronePairs.Num() - 1; index >= 0; --index)
	{
		if (m_pFollowingEffectDronePairs.IsValidIndex(index))
		{
			if (const FFollowingEffectDronePair* EffectDronePair = &m_pFollowingEffectDronePairs[index])
			{
				//エフェクトをドローンに追従させる
				if (IsValid(EffectDronePair->pFollowingEffect))
				{
					if (EffectDronePair->pDrone)
					{
						FVector DroneLocation = EffectDronePair->pDrone->GetActorLocation();
						EffectDronePair->pFollowingEffect->SetVariableVec3(TEXT("User.Aim_position"), DroneLocation);
					}
				}
				//エフェクトが消えたら配列から削除する
				else
				{
					m_pFollowingEffectDronePairs.RemoveAt(index);
				}
			}
		}
	}
}

//オーバーラップ開始時に呼ばれる処理
void ARing::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//タグがPlayerだった場合
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			if (m_pEffect)
			{
				//ヒットしたドローンとエフェクトを配列に格納する
				m_pFollowingEffectDronePairs.Add(
					FFollowingEffectDronePair(
						Cast<ADroneBase>(OtherActor),
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, m_pEffect, GetActorLocation())));

				//マテリアルの回転速度を上げる
				float ScrollSpeed = 1.f;
				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);

				//SEの再生
				UGameplayStatics::PlaySound2D(GetWorld(), m_RingHitSE);
			}
		}
	}
}