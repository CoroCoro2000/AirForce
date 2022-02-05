// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Train.generated.h"

//前方宣言
class UStaticMesh;
class UStaticMeshComponent;
class ASplineActor;

UCLASS()
class AIRFORCE_API ATrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//メッシュの初期化
	void InitializeMesh();
	//速度更新処理
	void UpdateSpeed(const float& DeltaTime);
	//移動更新処理
	void UpdateMove(const float& DeltaTime);
	//回転更新処理
	void UpdateRotation(const float& DeltaTime);
	//スプラインの終点に到着しているか確認する処理
	void CheckMoveDistance();

public:
	//初期化
	void Init();

private:
	UPROPERTY(EditAnywhere)
		TArray<UStaticMesh*> m_pMeshes;										//電車のメッシュ
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pFrontTrainMesh;
	UPROPERTY(VisibleAnywhere)
		TArray<UStaticMeshComponent*> m_pTrainMeshes;				//電車のメッシュ
	UPROPERTY(EditAnywhere)
		ASplineActor* m_pSplineActor;												//電車の移動に使うスプラインを持つアクター
	UPROPERTY(EditAnywhere)
		float m_MaxSpeed;																	//最高速度
	UPROPERTY(VisibleAnywhere)
		float m_CurrentSpeed;															//現在の速度
	UPROPERTY(EditAnywhere)
		float m_Acceleration;																//加速度
	UPROPERTY(EditAnywhere)
		float m_Deceleration;																//減速度
	UPROPERTY(VisibleAnywhere)
		float m_MoveDistance;															//電車がスプラインの開始点から進んだ距離
	UPROPERTY(EditAnywhere)
		bool m_bLoop;																		//スプラインの終点に到着したら始点からループするかどうか
	UPROPERTY(EditAnywhere)
		bool m_bCanMove;																	//移動可能フラグ
};
