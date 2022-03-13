//------------------------------------------------------------------------
// ファイル名		:Ring.h
// 概要				:コースの進行先を示すリングのクラス
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "TickLODActor.h"
#include "NiagaraComponentPool.h"
#include "Ring.generated.h"

//前方宣言
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ADroneBase;
class USoundBase;
class UCurveLinearColor;

//紐付けされたドローンとエフェクトを格納する構造体
USTRUCT(BlueprintType)
struct FFollowingDroneAndEffect
{
	GENERATED_BODY()
public:
	//コンストラクタ
	FFollowingDroneAndEffect();
	FFollowingDroneAndEffect(ADroneBase* pDrone, UNiagaraComponent* pNiagaraEffect = (UNiagaraComponent*)nullptr);
	//デストラクタ
	~FFollowingDroneAndEffect();
	//ドローン取得
	ADroneBase* GetDrone()const { return m_pDrone.Get(); }
	//エフェクト取得
	UNiagaraComponent* GetEffect()const { return m_pNiagaraEffect.Get(); }
	//エフェクトが生成済みかどうか
	bool IsEffectSpawned() const { return m_bIsEffectSpawned; }
	//エフェクトを生成
	void SpawnEffectAtLocation(const UObject* WorldContextObject, UNiagaraSystem* SystemTemplate, FVector SpawnLocation, FRotator SpawnRotation = FRotator::ZeroRotator, FVector Scale = FVector(1.F), bool bAutoDestroy = true, bool bAutoActivate = true, ENCPoolMethod PoolingMethod = ENCPoolMethod::None);

public:
	UPROPERTY(VisibleAnywhere)
		TWeakObjectPtr<ADroneBase> m_pDrone;
	UPROPERTY(VisibleAnywhere)
		TWeakObjectPtr<UNiagaraComponent> m_pNiagaraEffect;
	UPROPERTY(VisibleAnywhere)
		bool m_bIsEffectSpawned;
};

UCLASS()
class AIRFORCE_API ARing : public ATickLODActor
{
	GENERATED_BODY()
	
public:	
	//コンストラクタ
	ARing();

protected:
	//ゲーム開始時またはこのクラスのオブジェクトがスポーンされた時１度だけ呼び出される関数
	virtual void BeginPlay() override;

public:	
	//毎フレーム呼び出される関数
	virtual void Tick(float DeltaTime) override;

protected:
	//オーバーラップ時に呼ばれるイベント関数を登録
	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	//通過されているかのフラグ取得
	bool GetIsPassed()const { return m_bIsPassed; }
	//メッシュ取得
	UStaticMeshComponent* GetMesh()const { return m_pRingMesh; }

private:
	//サインカーブの値を更新
	void UpdateSineCurve(const float& CurrentTime);
	//リングのサイズ更新
	void UpdateScale(const float& DeltaTime);
	//リングのマテリアル更新
	void UpdateMaterial();
	//リングのエフェクト更新
	void UpdateEffect();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;														//リングのメッシュ
	UPROPERTY(EditAnywhere)
		TArray<struct FFollowingDroneAndEffect> m_pFollowingDroneAndEffect;			//通過したドローン
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* m_pEffect;																		//通過時に出すエフェクト
	UPROPERTY(VisibleAnywhere)
		bool m_bIsPassed;																						//このリングが通過されたか判定
	UPROPERTY(VisibleAnywhere)
		float m_PassedTime;																					//通過後の時間を計測
	UPROPERTY(VisibleAnywhere)
		float m_ResetTime;																					//通過後の時間を計測
	UPROPERTY(EditAnywhere)
		float m_SineWidth;																						//サイン波の間隔
	UPROPERTY(EditAnywhere)
		float m_SineScaleMin;																					//サイン波の最小値
	UPROPERTY(EditAnywhere)
		float m_SineScaleMax;																				//サイン波の最大値
	UPROPERTY(EditAnywhere)
		float m_SineCurveValue;																				//サイン波の値
	UPROPERTY(VisibleAnywhere)
		float m_RingScale;																						//リングのスケール
	UPROPERTY(EditAnywhere)
		float m_RingMaxScale;																				//リングの最大スケール
	UPROPERTY(EditAnywhere)
		FLinearColor m_HSV;																					//リングの色
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_pRingHitSE;																		//リング衝突SE
};
