 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

//前方宣言
class UStaticMeshComponent;
class UBoxComponent;

#define DEBUG_IsWithinRangeOfCheckpoint

UCLASS()
class AIRFORCE_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//オーバーラップ開始時に呼び出されるイベント関数
	UFUNCTION()
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//識別番号取得
	FORCEINLINE int GetNumber()const { return m_CheckNumber; }
	//次も同じ番号にするかのフラグ取得
	FORCEINLINE int GetIsSameNumberNext()const { return m_bSameNumberNext; }
	//通過判定フラグを取得
	FORCEINLINE bool GetIsPassed()const { return m_bPassed; }
	//範囲内にドローンがいるかどうかの判定取得
	FORCEINLINE bool GetIsDroneInRange()const { return m_bIsDroneInRange; }
	//識別番号設定
	FORCEINLINE void SetNumber(const int _number) { m_CheckNumber = _number; }
	//チェックポイントを毎フレーム更新するかどうかを設定
	FORCEINLINE void SetEveryUpdate(const bool& _bEveryUpdate) { PrimaryActorTick.bCanEverTick = _bEveryUpdate; }
private:
	//範囲内のドローンを探す処理
	void FindDroneInRange();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pGateMesh;					//チェックポイントのモデルメッシュ
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pCheckPointCollision;				//チェックポイントの通過判定用コリジョン
	UPROPERTY(VisibleAnywhere)
		int m_CheckNumber;												//識別番号
	UPROPERTY(EditAnywhere)
		bool m_bSameNumberNext;									//次の要素も同じ番号にするかどうか
	UPROPERTY(VisibleAnywhere)
		bool m_bPassed;													//通過済みどうかのフラグ
	UPROPERTY(VisibleAnywhere)
		bool m_bIsDroneInRange;										//範囲内にドローンがいるか判定するフラグ
	UPROPERTY(EditAnywhere)
		float m_RadiusOfSearchRange;								//検索する範囲(半径)

};
