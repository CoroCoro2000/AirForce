 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

//前方宣言
class UStaticMeshComponent;
class UBoxComponent;

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
	//通過判定フラグを取得
	FORCEINLINE bool GetIsPassed()const { return m_bPassed; }

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pGateMesh;					//チェックポイントのモデルメッシュ
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pCheckPointCollision;				//チェックポイントの通過判定用コリジョン
	UPROPERTY(VisibleAnywhere)
		bool m_bPassed;													//通過済みどうかのフラグ
};
