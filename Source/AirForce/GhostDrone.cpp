// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostDrone.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Misc/FileHelper.h"

//コンストラクタ
AGhostDrone::AGhostDrone()
	: m_LoadVelocity(FVector::ZeroVector)
	, m_LoadQuat(FQuat::Identity)
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
}

//毎フレーム処理
void AGhostDrone::Tick(float DeltaTime)
{
	if (m_isControl)
	{
			//羽の回転更新処理
			UpdateWingRotation(DeltaTime);

			//ドローンの回転処理
			UpdateRotation(DeltaTime);

			//移動処理
			UpdateSpeed(DeltaTime);

			m_PlaybackFlame++;

			if (m_PlaybackFlame >= m_PlayableFramesNum)
			{
				Destroy();
			}
	}
}

//ドローンの回転処理
void AGhostDrone::UpdateRotation(const float& DeltaTime)
{
	//読み込んだ移動量のテキストファイルをfloatに変換する
	bool IsValidTextArray = true;
	bool IsValidAxisTextArray = true;

	for (int index = 0; index < VECTOR4_COMPONENT_NUM; ++index)
	{
		IsValidTextArray = m_SaveQuatText.IsValidIndex(index);
		IsValidAxisTextArray = m_SaveQuatText[index].IsValidIndex(m_PlaybackFlame);
		if (!IsValidTextArray || IsValidAxisTextArray)
		{
			break;
		}
	}

	if (IsValidTextArray && IsValidAxisTextArray)
	{
		m_LoadQuat.X = FCString::Atof(*(m_SaveQuatText[0][m_PlaybackFlame]));
		m_LoadQuat.Y = FCString::Atof(*(m_SaveQuatText[1][m_PlaybackFlame]));
		m_LoadQuat.Z = FCString::Atof(*(m_SaveQuatText[2][m_PlaybackFlame]));
		m_LoadQuat.W = FCString::Atof(*(m_SaveQuatText[3][m_PlaybackFlame]));
	}
	m_pBodyMesh->SetWorldRotation(m_LoadQuat * MOVE_CORRECTION);
}

//移動処理
void AGhostDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	//読み込んだ移動量のテキストファイルをfloatに変換する
	int index = 0;
	for (const TArray<FString> SaveVelocityText : m_SaveVelocityText)
	{
		if (SaveVelocityText.IsValidIndex(m_PlaybackFlame))
		{
			m_LoadVelocity[index] = FCString::Atof(*(SaveVelocityText[m_PlaybackFlame]));
		}
		++index;
	}
	AddActorWorldOffset(m_LoadVelocity * MOVE_CORRECTION);
}

//レースの座標ファイル読み込み
void AGhostDrone::LoadingRaceVectorFile()
{
	//移動軸の数だけ配列を用意する
	m_SaveVelocityText.Empty();
	m_SaveVelocityText.SetNum(VECTOR3_COMPONENT_NUM);

	//設定したパスが軸と同じ数設定されていればロードする
	if (m_SaveVelocityText.Num() == m_SaveVelocityLoadPath.Num())
	{
		//ファイルを開いて保存されている値を読み込む
		for (int index = 0; index < (int)m_SaveVelocityText.Num(); ++index)
		{
			FString LoadFilePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath + m_SaveVelocityLoadPath[index];
			FFileHelper::LoadFileToStringArray(m_SaveVelocityText[index], *LoadFilePath);

			//再生可能なフレーム数を取得
			int PlayableFrame = m_SaveVelocityText[index].Num() - 1;
			if (index != 0)
			{
				if (PlayableFrame < m_PlayableFramesNum)
				{ 
					m_PlayableFramesNum = PlayableFrame;
				}
			}
			else
			{
				m_PlayableFramesNum = PlayableFrame;
			}
		}
	}
}

//レースのクオータニオンファイル読み込み
void AGhostDrone::LoadingRaceQuaternionFile()
{
	//クォータニオンの軸の数だけ配列を用意する
	m_SaveQuatText.Empty();
	m_SaveQuatText.SetNum(VECTOR4_COMPONENT_NUM);

	//設定したパスが軸と同じ数設定されていればロードする
	if (m_SaveQuatText.Num() == m_SaveQuatLoadPath.Num())
	{
		//ファイルを開いて保存されている値を読み込む
		for (int index = 0; index < (int)m_SaveQuatText.Num(); ++index)
		{
			FString LoadFilePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath + m_SaveQuatLoadPath[index];
			FFileHelper::LoadFileToStringArray(m_SaveQuatText[index], *LoadFilePath);

			//再生可能なフレーム数を取得
			int PlayableFrame = m_SaveQuatText[index].Num() - 1;
			if (PlayableFrame < m_PlayableFramesNum)
			{
				m_PlayableFramesNum = PlayableFrame;
			}
		}
	}
}