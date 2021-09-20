// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostDrone.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Misc/FileHelper.h"

//コンストラクタ
AGhostDrone::AGhostDrone()
	: PlaybackFlame(1)
	, Time(0.f)
	, FlameCnt(0)
{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = m_pDroneCollision;

	if (m_pBodyMesh)
	{
		m_pBodyMesh->SetupAttachment(m_pDroneCollision);
	}


}
//ゲーム開始時に1度だけ処理
void AGhostDrone::BeginPlay()
{
	Super::BeginPlay();

	//レースの座標ファイル読み込み
	LoadingRaceVectorFile();
	//レースのクオータニオンファイル読み込み
	LoadingRaceQuaternionFile();

	PrimaryActorTick.TickInterval = 0.f;
}

//毎フレーム処理
void AGhostDrone::Tick(float DeltaTime)
{
	if (m_isControl)
	{
		if (PlaybackFlame < m_SaveVelocityX.Num() - 1)
		{
			//羽の回転更新処理
			UpdateWingRotation(DeltaTime);

			//ドローンの回転処理
			UpdateRotation(DeltaTime);

			//移動処理
			UpdateSpeed(DeltaTime);

			PlaybackFlame++;
		}

		else
		{
			Destroy();
		}
	}
}

//ドローンの回転処理
void AGhostDrone::UpdateRotation(const float& DeltaTime)
{
	if(m_SaveQuatX.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.X = FCString::Atof(*(m_SaveQuatX[PlaybackFlame]));
	}
	if (m_SaveQuatY.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.Y = FCString::Atof(*(m_SaveQuatY[PlaybackFlame]));
	}
	if (m_SaveQuatZ.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.Z = FCString::Atof(*(m_SaveQuatZ[PlaybackFlame]));
	}
	if (m_SaveQuatW.IsValidIndex(PlaybackFlame))
	{
		m_SaveQuat.W = FCString::Atof(*(m_SaveQuatW[PlaybackFlame]));
	}

	m_pBodyMesh->SetRelativeRotation(m_SaveQuat * MOVE_CORRECTION);
}

//移動処理
void AGhostDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	m_SaveVelocity.X = FCString::Atof(*(m_SaveVelocityX[PlaybackFlame]));
	m_SaveVelocity.Y = FCString::Atof(*(m_SaveVelocityY[PlaybackFlame]));
	m_SaveVelocity.Z = FCString::Atof(*(m_SaveVelocityZ[PlaybackFlame]));

	AddActorWorldOffset(m_SaveVelocity * MOVE_CORRECTION);
}

//レースの座標ファイル読み込み
void AGhostDrone::LoadingRaceVectorFile()
{
	//テキストファイル読み込み
	FFileHelper::LoadFileToStringArray(m_SaveVelocityX, *(FPaths::ProjectDir() + FString("Record/VX.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveVelocityY, *(FPaths::ProjectDir() + FString("Record/VY.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveVelocityZ, *(FPaths::ProjectDir() + FString("Record/VZ.txt")));
}

//レースのクオータニオンファイル読み込み
void AGhostDrone::LoadingRaceQuaternionFile()
{
	//テキストファイル読み込み
	FFileHelper::LoadFileToStringArray(m_SaveQuatX, *(FPaths::ProjectDir() + FString("Record/QX.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveQuatY, *(FPaths::ProjectDir() + FString("Record/QY.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveQuatZ, *(FPaths::ProjectDir() + FString("Record/QZ.txt")));
	FFileHelper::LoadFileToStringArray(m_SaveQuatW, *(FPaths::ProjectDir() + FString("Record/QW.txt")));
}