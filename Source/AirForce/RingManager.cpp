//-----------------------------------------------------------------------------------------------------------------
// ファイル名		:RingManager.cpp
// 概要				:リングを管理するアクタークラス
// 作成日			:2021/08/20
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/08/20	池村凌太		:	ゲーム開始時にすべてのリングに番号を割り当てる処理を作成
//						:2021/08/20	池村凌太		:	リングがくぐられたら配列から削除する処理を作成
//-----------------------------------------------------------------------------------------------------------------

//インクルード
#include "RingManager.h"
#include "GameManager.h"
#include "Ring.h"
#include "DroneBase.h"
#include "GameUtility.h"
#include "Kismet/GameplayStatics.h"

//コンストラクタ
ARingManager::ARingManager()
	: m_pDrone(NULL)
	, m_pGameManager(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ呼ばれる処理
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//TArrayに格納された若い要素のリングから順に番号付けする
	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			//m_pChildRings[index]->SetRingNumber(index);
		}
	}

	//ドローンとゲームマネージャーを検索、保持する
	TSubclassOf<AActor> findClass = AActor::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), findClass, actors);

	if ((int)actors.Num() > 0)
	{
		for (AActor* pActor : actors)
		{
			if (pActor->ActorHasTag(TEXT("GameManager")))
			{
				m_pGameManager = Cast<AGameManager>(pActor);
			}
			if (pActor->ActorHasTag(TEXT("Drone")))
			{
				m_pDrone = Cast<ADroneBase>(pActor);
			}
		}
	}
}

//毎フレーム呼ばれる処理
void ARingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//リング情報を更新
	UpdateRingColor();

	if (m_pGameManager && m_pChildRings[0])
	{
		m_pGameManager->SetIsGoal(m_pChildRings[0]->GetIsPassed());
	}
	
}

//リング情報の更新処理
void ARingManager::UpdateRingColor()
{


}
