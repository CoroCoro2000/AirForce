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
#include "DroneBase.h"
#include "Ring.generated.h"

//前方宣言
class UStaticMeshComponent;
class UColorLightComponent;
class UNiagaraComponent;
class APlayerDrone;
class UCurveFloat;

#define DEBUG_HIT

UCLASS(HideCategories = (Input, Rendering, Replication, LOD, Cooking))
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
	bool isDraw();
	int GetRingNumber() { return m_RingNumber; }
protected:
	//オーバーラップ時に呼ばれるイベント関数を登録
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere)
		//リングの色を管理するコンポーネント
		UColorLightComponent* m_pColorLightComp;
	UPROPERTY(EditAnywhere)
		//リングのメッシュ
		UStaticMeshComponent* m_pRingMesh;
	UPROPERTY(EditAnywhere, Category = "Ring")
		int m_RingNumber;							//リングの番号割り当て
	UPROPERTY(EditAnywhere, Category = "Ring")
		int m_RingDrawUpNumber;						//プレイヤー何個先のリングまで描画するか決めるナンバー
	UPROPERTY(EditAnywhere, Category = "Ring")
		UNiagaraComponent* m_pNiagaraEffectComp;	//リングを通過した際に出すエフェクト
	UPROPERTY(EditAnywhere, Category = "Ring")
		bool m_bIsPassed;							//このリングが通過されたか判定

	UPROPERTY(EditAnywhere, Category = "Ring")
		APlayerDrone* m_pDrone;

	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_MakeInvisibleCnt;
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_MakeInvisibleTime;
	UPROPERTY(EditAnywhere, Category = "Ring")
		UCurveFloat* m_pScaleCurve;
};
