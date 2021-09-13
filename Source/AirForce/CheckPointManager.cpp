//------------------------------------------------------------------------
// ファイル名		:CheckPointManager.cpp
// 概要				:チェックポイントを管理するマネージャークラス
// 作成日			:2021/09/13
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/09/13
//------------------------------------------------------------------------

#include "CheckPointManager.h"
#include "CheckPoint.h"

// Sets default values
ACheckPointManager::ACheckPointManager()
	: m_PassedCheckpointNum(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckPointManager::BeginPlay()
{
	Super::BeginPlay();
	
	//識別番号設定
	int CheckPointNumber = 0;
	for (ACheckPoint* pCheckPoint : m_pCheckPoints)
	{
		if (pCheckPoint)
		{
			pCheckPoint->SetNumber(CheckPointNumber);

			//0番目のチェックポイントだけアクティブな状態にしておく
			if (pCheckPoint->GetNumber() == 0)
			{
				pCheckPoint->SetEveryUpdate(true);
			}

			//次のチェックポイントを同番号にする場合はインクリメントさせない
			if (!pCheckPoint->GetIsSameNumberNext())
			{
				++CheckPointNumber;
			}
		}
	}
}

// Called every frame
void ACheckPointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (ACheckPoint* pCheckPoint : m_pCheckPoints)
	{
		if (pCheckPoint)
		{
			//通過済みのチェックポイントの検索処理を終了し、配列から外す
			if (pCheckPoint->GetIsPassed())
			{
				pCheckPoint->SetEveryUpdate(false);
				m_pCheckPoints.Remove(pCheckPoint);
			}
		}
	}
}

