// Fill out your copyright notice in the Description page of Project Settings.

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "DroneBase.h"
#include "GhostDrone.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API AGhostDrone : public ADroneBase
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	AGhostDrone();
protected:
	//ゲーム開始時に1度だけ処理
	virtual void BeginPlay() override;

public:
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;

	//レースの座標ファイル読み込み
	void LoadingRaceVectorFile();

	//レースのクオータニオンファイル読み込み
	void LoadingRaceQuaternionFile();

private:
	//回転処理
	void UpdateRotation(const float& DeltaTime);

	//移動処理
	void UpdateSpeed(const float& DeltaTime)override;

private:
	UPROPERTY(EditAnywhere, Category = "Drone")
		int PlaybackFlame;

	UPROPERTY(EditAnywhere, Category = "Drone")
		int m_PlayableFramesNum;
	UPROPERTY(EditAnywhere, Category = "Drone")
		float Time;

	UPROPERTY(EditAnywhere, Category = "Drone")
		int FlameCnt;

	UPROPERTY(EditAnywhere, Category = "Ghost")
		FVector m_LoadVelocity;														
	UPROPERTY(EditAnywhere, Category = "Ghost")
		FVector4 m_LoadQuat;
};
