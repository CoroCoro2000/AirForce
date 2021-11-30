//------------------------------------------------------------------------
// ファイル名		:Ring.h
// 概要				:コースの進行先を示すリングのクラス
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ring.generated.h"

//前方宣言
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ADroneBase;

USTRUCT(BlueprintType)
struct FFollowingEffectDronePair
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = FollowingEffectDronePair)
		ADroneBase* pDrone;
	UPROPERTY(EditAnywhere, Category = FollowingEffectDronePair)
		UNiagaraComponent* pFollowingEffect;

	//コンストラクタ
	FFollowingEffectDronePair()
		: pDrone(nullptr)
		, pFollowingEffect(nullptr)
	{}
	FFollowingEffectDronePair(ADroneBase* Drone, UNiagaraComponent* FollowingEffect)
		: pDrone(Drone)
		, pFollowingEffect(FollowingEffect)
	{}
};

UCLASS()
class AIRFORCE_API ARing : public AActor
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
	FORCEINLINE bool GetIsPassed()const { return m_bIsPassed; }
	//メッシュ取得
	UStaticMeshComponent* GetMesh()const { return m_pRingMesh; }

private:
	//サインカーブの値を更新
	void UpdateSineCurve(const float& DeltaTime);
	//リングのサイズ更新
	void UpdateScale(const float& DeltaTime);
	//リングのマテリアル更新
	void UpdateMaterial(const float& DeltaTime);
	//リングのエフェクト更新
	void UpdateEffect(const float& DeltaTime);

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;														//リングのメッシュ
	UPROPERTY(EditAnywhere)
		TArray<struct FFollowingEffectDronePair> m_pFollowingEffectDronePairs;		//通過したドローンと追従エフェクトのペア
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* m_pEffect;																		//通過時に出すエフェクト
	UPROPERTY(VisibleAnywhere)
		bool m_bIsPassed;																						//このリングが通過されたか判定
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
		FLinearColor m_HSV;																					//リングの色
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_RingHitSE;																		//リング衝突SE
};
