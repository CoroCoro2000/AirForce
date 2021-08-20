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
class UColorLightComponent;
class UNiagaraComponent;
class APlayerDrone;
class UCurveFloat;

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
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void SetActivate(const bool& _isActive);
	void SetDroneLocation(const FVector& _droneLocation) { m_PassedDroneLocation = _droneLocation; }
	void SetRingNumber(const int& _ringNumber) { m_RingNumber = _ringNumber; }
	int GetRingNumber() { return m_RingNumber; }
	bool IsPassed()const { return m_bIsPassed; }
	bool IsDestroy()const { return m_bDestroy; }

private:
	//リングの色の更新処理
	void UpdateColor(const float& DeltaTime);
	//リングのトランスフォーム更新
	void UpdateTransform(const float& DeltaTime);

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;				//リングのメッシュ
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* m_pNiagaraEffectComp;		//リングを通過した際に出すエフェクト
	UPROPERTY(EditAnywhere)
		UColorLightComponent* m_pColorLightComp;		//リングの色を管理するコンポーネント
	UPROPERTY(EditAnywhere)
		int m_RingNumber;											//リングの番号
	UPROPERTY(EditAnywhere)
		bool m_bIsPassed;												//このリングが通過されたか判定
	UPROPERTY(EditAnywhere)
		bool m_bDestroy;												//リングを消すかどうかのフラグ
	UPROPERTY(EditAnywhere)
		float m_MakeInvisibleCnt;									//リングを消す際のカウンター
	UPROPERTY(EditAnywhere)
		float m_MakeInvisibleTime;									//リングが消えるまでの時間
	UPROPERTY(EditAnywhere)
		float m_InvisibleCntRate;									//リングを消すカウンターの進行度
	UPROPERTY(EditAnywhere)
		UCurveFloat* m_pScaleCurve;								//リングの大きさを決めるカーブ
	UPROPERTY(EditAnywhere)
		FVector m_PassedDroneLocation;						//リングをくぐったドローンの座標
};
