//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODManager.cpp
// 概要				:アクターのTickLODレベルを管理するマネージャークラス
// 作成日			:2022/03/09
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/03/09		:プレイヤーから離れた位置のアクターのFPSを下げる処理の追加
//------------------------------------------------------------------------------------------------------------------------------------------------

#include "TickLODManager.h"
#include "TickLODActor.h"
#include "PlayerDrone.h"
#include "GameUtility.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATickLODManager::ATickLODManager()
	: m_TickLODSettings()
	, m_pPlayer(nullptr)
	, m_pTickLODActors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1秒に1回Tick更新を行う
	PrimaryActorTick.TickInterval = 1.f;

	//アクターのTickLOD設定
	m_TickLODSettings.Add(FTickLODSetting());
}

// Called when the game starts or when spawned
void ATickLODManager::BeginPlay()
{
	Super::BeginPlay();
	
	//LODアクターの初期化
	InitializeActorArray();
}

// Called every frame
void ATickLODManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//LODを更新
	UpdateLOD(DeltaTime);
}

//LODのアクター配列の初期化
void ATickLODManager::InitializeActorArray()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), OutActors);

	for (AActor* pActor : OutActors)
	{
		if (pActor)
		{
			//レベル上のATickLODActorを継承しているアクターを全て配列に格納
			if (ATickLODActor* pTickLODActor = Cast<ATickLODActor>(pActor))
			{
				//アクターに番号を設定
				pTickLODActor->SetNumber(m_pTickLODActors.Num());
				m_pTickLODActors.Add(pTickLODActor);
			}

			if (!m_pPlayer)
			{
				if (pActor->ActorHasTag("Player"))
				{
					m_pPlayer = Cast<APlayerDrone>(pActor);
				}
			}
		}
	}
}

//LODの更新
void ATickLODManager::UpdateLOD(const float& DeltaTime)
{
	if (!m_pPlayer) { return; }

	FVector PlayerLocation = m_pPlayer->GetActorLocation();
	const float FPS = 1.f / GetWorld()->GetDeltaSeconds();

	for (ATickLODActor* pTickLODActor : m_pTickLODActors)
	{
		if (pTickLODActor)
		{
			//プレイヤーとアクターの距離
			const float Distance = FVector::Dist(PlayerLocation, pTickLODActor->GetActorLocation());

			//LOD設定の距離が遠い順に確認し、アクターの位置にあったLOD設定を割り当てる
			for (const FTickLODSetting& TickLODSetting : m_TickLODSettings)
			{
				if (TickLODSetting.Distance <= Distance)
				{
					pTickLODActor->SetTickFPS(CGameUtility::SetDecimalTruncation(FPS * TickLODSetting.FrameRate,3));
					
					break;
				}
			}
		}
	}
}

//配置時に実行される関数
void ATickLODManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (m_TickLODSettings.Num() <= 1) { return; }

	//LOD設定の配列を距離が遠い順にソートする
	int32 i = 0, j = 0;
	int32 Max = m_TickLODSettings.Num();
	FTickLODSetting tmpTickLODSetting;
	for (i = 0; i < Max; ++i)
	{
		for (j = i + 1; j < Max; ++j)
		{
			if (m_TickLODSettings.IsValidIndex(i) && m_TickLODSettings.IsValidIndex(j))
			{
				//距離を比較し、並び替える
				if (m_TickLODSettings[i].Distance < m_TickLODSettings[j].Distance)
				{
					tmpTickLODSetting = m_TickLODSettings[i];
					m_TickLODSettings[i] = m_TickLODSettings[j];
					m_TickLODSettings[j] = tmpTickLODSetting;
				}
			}
		}
	}
}

