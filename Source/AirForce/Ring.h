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
class ADroneBase;

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
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	//通過されているかのフラグ取得
	FORCEINLINE bool GetIsPassed()const { return m_bIsPassed; }
	//メッシュ取得
	FORCEINLINE UStaticMeshComponent* GetMesh()const { return m_pRingMesh; }
	//ナイアガラコンポーネント取得
	FORCEINLINE UNiagaraComponent* GetEffectComponent()const { return m_pNiagaraEffectComp; }

private:
	//リングのサイズ更新
	void UpdateScale(const float& DeltaTime);

	//リングのマテリアル更新
	void UpdateMaterial(const float& DeltaTime);

	//リングのエフェクト更新
	void UpdateEffect(const float& DeltaTime);

	//リングの初期化
	void Reset();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;				//リングのメッシュ
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* m_pNiagaraEffectComp;		//リングを通過した際に出すエフェクト
	UPROPERTY(VisibleAnywhere)
		bool m_bIsPassed;												//このリングが通過されたか判定
	UPROPERTY(VisibleAnywhere)
		float m_MakeInvisibleCnt;									//リングが消えるまでのカウンター
	UPROPERTY(EditAnywhere)
		float m_MakeInvisibleTime;									//リングが消えるまでの時間
	UPROPERTY(EditAnywhere)
		float m_SineWidth;												//サイン波の間隔
	UPROPERTY(EditAnywhere)
		float m_SineScaleMin;											//サイン波の最小値
	UPROPERTY(EditAnywhere)
		float m_SineScaleMax;										//サイン波の最大値
	UPROPERTY(EditAnywhere)
		float m_PassedSceleMax;									//リング通過後の大きさ
	UPROPERTY(VisibleAnywhere)
		ADroneBase* m_pPassedDrone;							//このリングを通過したドローン
	UPROPERTY(EditAnywhere)
		FLinearColor m_HSV;											//リングの色
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_RingHIttSE;						//リング衝突SE
	UPROPERTY(VisibleAnywhere)
		FTransform m_InitialTransform;							//リングの初期トランスフォーム
};
