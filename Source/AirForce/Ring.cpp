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
#include "NiagaraComponentPool.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//struct FFollowingDroneAndEffect----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//コンストラクタ
FFollowingDroneAndEffect::FFollowingDroneAndEffect()
	: m_pDrone(nullptr)
	, m_pNiagaraEffect(nullptr)
	, m_bIsEffectSpawned(false)
{

}
FFollowingDroneAndEffect::FFollowingDroneAndEffect(ADroneBase* pDrone, UNiagaraComponent* pNiagaraEffect)
	: m_pDrone(MakeWeakObjectPtr<ADroneBase>(pDrone))
	, m_pNiagaraEffect(MakeWeakObjectPtr<UNiagaraComponent>(pNiagaraEffect))
	, m_bIsEffectSpawned(m_pNiagaraEffect.IsValid())
{

}
//デストラクタ
FFollowingDroneAndEffect::~FFollowingDroneAndEffect()
{

}
//エフェクトを生成
void FFollowingDroneAndEffect::SpawnEffectAtLocation(const UObject* WorldContextObject, UNiagaraSystem* SystemTemplate, FVector SpawnLocation, FRotator SpawnRotation, FVector Scale, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod)
{
	m_pNiagaraEffect = MakeWeakObjectPtr<UNiagaraComponent>(UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SystemTemplate, SpawnLocation, SpawnRotation, Scale, bAutoDestroy, bAutoActivate, PoolingMethod));
	m_bIsEffectSpawned = true;
}

//ARing----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//コンストラクタ
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pEffect(NULL)
	, m_bIsPassed(false)
	, m_PassedTime(0.f)
	, m_ResetTime(1.f)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_SineCurveValue(0.f)
	, m_RingScale(1.f)
	, m_RingMaxScale(m_RingScale * 1.8f)
	, m_HSV(30.f, 40.f, 30.f)
	, m_pRingHitSE(NULL)
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

	//設定されたFPSの間隔でTickを更新する
	const float currentTime = GetWorld()->GetTimeSeconds();
	//想定される1フレームにかかる時間
	const float TimePerFrame = 1.f / m_TickFPS;
	//前回実行されてからの経過時間
	const float deltaTime = currentTime - m_LastTickTime;

	//処理可能なフレームであれば更新
	if (deltaTime > TimePerFrame && IsProcessableFrame(currentTime))
	{
		m_LastTickTime = currentTime;

		//サインカーブの値を更新
		UpdateSineCurve(currentTime);

		//リングのサイズ更新
		UpdateScale(deltaTime);

		//リングのマテリアル更新
		UpdateMaterial();
	}

	//エフェクトの更新
	UpdateEffect();
}

//サインカーブの値を更新
void ARing::UpdateSineCurve(const float& CurrentTime)
{
	//サイン波の幅を設定
	const float WaveWidth = m_SineWidth * CurrentTime;
	//サイン波の大きさを0から1に正規化する
	m_SineCurveValue = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
}

//リングのサイズ更新
void ARing::UpdateScale(const float& DeltaTime)
{
	float NewScale = m_RingScale;

	//通過フラグが立っているならリングを拡大する
	if (m_bIsPassed)
	{
		if (m_PassedTime < m_ResetTime)
		{
			m_PassedTime += DeltaTime;
		}
		else
		{
			m_bIsPassed = false;

			//マテリアルの回転速度を下げる
			float ScrollSpeed = 0.5f;
			m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);
		}

		NewScale = FMath::InterpExpoOut(m_RingScale, m_RingMaxScale, FMath::Clamp(m_PassedTime / m_ResetTime, 0.f, 1.f));
	}
	//通過フラグが立っていないときはSin波の間隔で拡大縮小を繰り返す
	else
	{
		//サイン波の値でリングの大きさを変える
		float ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, m_SineCurveValue);
		NewScale *= ScaleMultiplier;
	}
	float Speed = FMath::Clamp(m_bIsPassed ? DeltaTime * 10.f : DeltaTime * 8.f, 0.f, 1.f);
	SetActorScale3D(FMath::Lerp(GetActorScale3D(), FVector(NewScale), Speed));
}

//リングのマテリアル更新
void ARing::UpdateMaterial()
{
	if (!m_pRingMesh) { return; }

	if (m_bIsPassed)
	{
		//マテリアルの回転速度を上げる
		float ScrollSpeed = 1.5f;
		m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);
	}

	m_HSV.R = FMath::Lerp(50.f, 30.f, m_SineCurveValue);
	m_HSV.B = FMath::Lerp(30.f, 50.f, m_SineCurveValue);
	m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
}

//リングのエフェクト更新
void ARing::UpdateEffect()
{
	if (m_pFollowingDroneAndEffect.Num() <= 0) { return; }

	//エフェクトが消えていたら配列から削除するため、逆順にfor文を回す
	for (int32 index = m_pFollowingDroneAndEffect.Num() - 1; index >= 0; --index)
	{
		if (m_pFollowingDroneAndEffect.IsValidIndex(index))
		{
			FFollowingDroneAndEffect& DroneAndEffect = m_pFollowingDroneAndEffect[index];

			//生成済みのエフェクトが無効になっていたら
			if (DroneAndEffect.IsEffectSpawned() && (!DroneAndEffect.m_pNiagaraEffect.IsValid()))
			{
				//配列から取り除く
				m_pFollowingDroneAndEffect.RemoveAt(index);
			}
			//エフェクトがスポーンされていない場合はリングの拡大が終わっていたらスポーンさせる
			else if (!DroneAndEffect.IsEffectSpawned())
			{
				FVector scaleSubtract = FVector(m_RingMaxScale) - GetActorScale3D();
				if (scaleSubtract.IsNearlyZero(0.3f))
				{
					if (m_pEffect)
					{
						DroneAndEffect.SpawnEffectAtLocation(this, m_pEffect, GetActorLocation(), GetActorRotation(), GetActorScale3D());
						if (DroneAndEffect.m_pNiagaraEffect.IsValid())
						{
							int32 randomColor = FMath::RandRange(0, 2);
							FLinearColor particleColor = (randomColor == 0) ? FLinearColor::Red : (randomColor == 1) ? FLinearColor::Green : FLinearColor::Blue;
							particleColor *= 30.f;
							DroneAndEffect.m_pNiagaraEffect->SetVariableLinearColor(TEXT("User.Color"), particleColor);
						}
					}
				}
			}
		}
	}
}

//オーバーラップ開始時に呼ばれる処理
void ARing::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//リングの更新フレームが15FPSより少ない場合はオーバーラップ処理を行わない
	if (m_TickFPS < 15.f) { return; }

	if (OtherActor && OtherActor != this)
	{
		//タグがPlayerだった場合
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			if (ADroneBase* pDrone = Cast<ADroneBase>(OtherActor))
			{
				//通過済みのドローンか確認
				bool isPassed = false;
				for (const FFollowingDroneAndEffect& FollowingDroneAndEffect : m_pFollowingDroneAndEffect)
				{
					if (pDrone == FollowingDroneAndEffect.m_pDrone)
					{
						isPassed = true;
						break;
					}
				}

				if (!isPassed)
				{
					m_bIsPassed = true;
					m_PassedTime = 0.f;

					//ヒットしたドローンを配列に格納する
					m_pFollowingDroneAndEffect.Add(FFollowingDroneAndEffect(pDrone));

					//ヒットしたドローンが自身の操作するドローンの時のみSEを再生
					if (pDrone->GetisControl() && pDrone->IsPlayerControlled())
					{
						UGameplayStatics::PlaySound2D(GetWorld(), m_pRingHitSE);
					}
				}
			}
		}
	}
}