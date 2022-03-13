// Fill out your copyright notice in the Description page of Project Settings.


#include "RingManager.h"
#include "Ring.h"
#include "PlayerDrone.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARingManager::ARingManager()
	: m_pPlayer(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	////リングのTickLOD設定
	//m_TickLODSettings.Add(FTickLODSetting());
}

// Called when the game starts or when spawned
void ARingManager::BeginPlay()
{
	Super::BeginPlay();

	////リングの初期化
	//InitializeRing();
}

// Called every frame
void ARingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	////リングの更新
	//UpdateRings();
}

//リングの初期化
void ARingManager::InitializeRing()
{
//	//レベル上に存在するリングをすべて配列に格納
//	TArray<AActor*> OutActors;
//	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), OutActors);
//
//	for (AActor* pActor : OutActors)
//	{
//		if (pActor)
//		{
//			if (pActor->ActorHasTag(TEXT("Ring")))
//			{
//				m_pRings.Add(Cast<ARing>(pActor));
//			}
//
//			if (pActor->ActorHasTag(TEXT("Player")))
//			{
//				m_pPlayer = Cast<APlayerDrone>(pActor);
//			}
//		}
//	}
//
//#if WITH_EDITOR
//	FString str = TEXT("RingCount[") + FString::FromInt(m_pRings.Num()) + TEXT("]");
//	UKismetSystemLibrary::PrintString(this, str, true, false);
//#endif // WITH_EDITOR
}

//リングの更新
void ARingManager::UpdateRings()
{
	//if (!m_pPlayer) { return; }

	//FVector PlayerLocation = m_pPlayer->GetActorLocation();

	//for (ARing* pRing : m_pRings)
	//{
	//	if (pRing)
	//	{
	//		//プレイヤーとリングの距離
	//		const float Distance = FVector::Dist(PlayerLocation, pRing->GetActorLocation());

	//		//LOD設定の一番遠い距離より遠くにいるリングは最低FPSに設定
	//		bool isLODSettingRange = true;
	//		{
	//			int32 LastIndex = m_TickLODSettings.Num() - 1;
	//			if (m_TickLODSettings.IsValidIndex(LastIndex))
	//			{
	//				if (m_TickLODSettings[LastIndex].Distance < Distance)
	//				{
	//					if (pRing->GetTickFPS() != m_TickLODSettings[LastIndex].FPS)
	//					{
	//						pRing->SetTickFPS(m_TickLODSettings[LastIndex].FPS);
	//					}
	//						isLODSettingRange = false;
	//				}
	//			}
	//		}

	//		//LOD設定の距離内にいる場合は階層を設定
	//		if (isLODSettingRange)
	//		{
	//			for (const FTickLODSetting& TickLODSetting : m_TickLODSettings)
	//			{
	//				if (TickLODSetting.Distance >= Distance)
	//				{
	//					if (pRing->GetTickFPS() != TickLODSetting.FPS)
	//					{
	//						pRing->SetTickFPS(TickLODSetting.FPS);
	//					}
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}
}

//配置時に実行される関数
void ARingManager::OnConstruction(const FTransform& Transform)
{
	//if (m_TickLODSettings.Num() <= 1) { return; }

	////LOD設定の配列をDistanceの値が小さい順にソートする
	//int32 i = 0, j = 0;
	//int32 Max = m_TickLODSettings.Num();
	//FTickLODSetting tmpTickLODSetting;
	//for (i = 0; i < Max; ++i)
	//{
	//	for (j = i + 1; j < Max; ++j)
	//	{
	//		if (m_TickLODSettings.IsValidIndex(i) && m_TickLODSettings.IsValidIndex(j))
	//		{
	//			//距離を比較し、並び替える
	//			if (m_TickLODSettings[i].Distance > m_TickLODSettings[j].Distance)
	//			{
	//				tmpTickLODSetting = m_TickLODSettings[i];
	//				m_TickLODSettings[i] = m_TickLODSettings[j];
	//				m_TickLODSettings[j] = tmpTickLODSetting;
	//			}
	//		}
	//	}
	//}
}