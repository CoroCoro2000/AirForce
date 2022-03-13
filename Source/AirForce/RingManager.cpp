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

	////�����O��TickLOD�ݒ�
	//m_TickLODSettings.Add(FTickLODSetting());
}

// Called when the game starts or when spawned
void ARingManager::BeginPlay()
{
	Super::BeginPlay();

	////�����O�̏�����
	//InitializeRing();
}

// Called every frame
void ARingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	////�����O�̍X�V
	//UpdateRings();
}

//�����O�̏�����
void ARingManager::InitializeRing()
{
//	//���x����ɑ��݂��郊���O�����ׂĔz��Ɋi�[
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

//�����O�̍X�V
void ARingManager::UpdateRings()
{
	//if (!m_pPlayer) { return; }

	//FVector PlayerLocation = m_pPlayer->GetActorLocation();

	//for (ARing* pRing : m_pRings)
	//{
	//	if (pRing)
	//	{
	//		//�v���C���[�ƃ����O�̋���
	//		const float Distance = FVector::Dist(PlayerLocation, pRing->GetActorLocation());

	//		//LOD�ݒ�̈�ԉ���������艓���ɂ��郊���O�͍Œ�FPS�ɐݒ�
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

	//		//LOD�ݒ�̋������ɂ���ꍇ�͊K�w��ݒ�
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

//�z�u���Ɏ��s�����֐�
void ARingManager::OnConstruction(const FTransform& Transform)
{
	//if (m_TickLODSettings.Num() <= 1) { return; }

	////LOD�ݒ�̔z���Distance�̒l�����������Ƀ\�[�g����
	//int32 i = 0, j = 0;
	//int32 Max = m_TickLODSettings.Num();
	//FTickLODSetting tmpTickLODSetting;
	//for (i = 0; i < Max; ++i)
	//{
	//	for (j = i + 1; j < Max; ++j)
	//	{
	//		if (m_TickLODSettings.IsValidIndex(i) && m_TickLODSettings.IsValidIndex(j))
	//		{
	//			//�������r���A���ёւ���
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