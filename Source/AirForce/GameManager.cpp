//-------------------------------------------------------------------------------------
// ファイル名	:GameManager.h
// 概要				:ゲームの進行を管理するクラス
// 作成日			:2021/07/07
// 作成者			:19CU0104 池田翔一郎
// 更新内容		:
//-------------------------------------------------------------------------------------

//インクルード
#include "GameManager.h"
#include "GameUtility.h"
#include "Misc/FileHelper.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

#define ScoreTxt "Score.txt"
//コンストラクタ
AGameManager::AGameManager()
	: m_CurrentScene(ECURRENTSCENE::SCENE_TITLE)
	, m_NextScene()
	, m_isPause(NULL)
	, m_isStart(false)
	, m_isGoal(false)
	, m_isSceneTransition(false)
	, m_CountDownTime(4.f)
	, m_RapTime(0.f)
	, m_DefaultTime(0.f)
	, m_RankingDisplayNum(5)
	, m_isScoreWrite(false)
	, m_isNewRecord(false)
	, m_Drone(NULL)
	, m_LatentAction(0, 1, TEXT("Completed"), this)
	, m_bLoadComplete(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ処理
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	FFileHelper::LoadFileToStringArray(m_RapTimeText, *(FPaths::GameDir() + ScoreTxt));
	for (int i = 0; i < m_RapTimeText.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *(m_RapTimeText[i]));
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);
		//入力マッピング
		InputComponent->BindAction(TEXT("InputUp"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneUp);
		InputComponent->BindAction(TEXT("InputDown"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneDown);

		UE_LOG(LogTemp, Warning, TEXT("InputBind"));
	}

	//	ドローンの検索
	AActor* pDrone = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (pDrone)
	{
		m_Drone = Cast<ADroneBase>(pDrone);
	}
}

//毎フレーム処理
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (m_CurrentScene)
	{
	case ECURRENTSCENE::SCENE_TITLE:

		break;
	case ECURRENTSCENE::SCENE_FIRST:
		//レースが始まっていないならカウントダウンをする
		if (!m_isStart)
		{
			m_CountDownTime -= DeltaTime;
		}
			
		//レースが始まっているならラップタイムを計測する
		if (m_isStart & !m_isGoal)
		{
			//m_RapTime = 40.432888;
			m_RapTime += DeltaTime;
		}
			
		if (m_isGoal)
		{
			if (!m_isScoreWrite)
			{
				m_RapTime = CGameUtility::SetDecimalTruncation(m_RapTime, 3);
				m_RapTime *= 1000.f;
				m_RapTimeText.Add(FString::FromInt(int(m_RapTime)));
				//ラップタイム並び替え
				RapTimeSort();

				if (m_RapTimeText.Num() > m_RankingDisplayNum)
				{
					for (int i = m_RankingDisplayNum; i < m_RapTimeText.Num(); i++)
					{
						m_RapTimeText.RemoveAt(i);
					}
				}

				FFileHelper::SaveStringArrayToFile(m_RapTimeText, *(FPaths::GameDir() + ScoreTxt));

				for (int i = 0; i < m_RapTimeText.Num(); i++)
				{
					UE_LOG(LogTemp, Warning, TEXT("%s"), *(m_RapTimeText[i]));
				}
				m_isScoreWrite = true;
				//UE_LOG(LogTemp, Warning, TEXT("%f"), m_RapTime);
			}
			
		}

		if (m_CountDownTime <= 1.f)
		{
			m_isStart = true;
		}
			
		//	レースがスタートして、ゴールしていない間操作可能にする
		m_Drone->SetisControl((m_isStart & !m_isGoal) );
		break;
	case ECURRENTSCENE::SCENE_RESULT:
		break;
	default:
		break;
	}
}

//
void AGameManager::NextSceneUp()
{
	if (!m_isSceneTransition) { return; }
		
	m_NextScene--;
	if ((int)m_NextScene.GetNextScene() > 2)
	{
		m_NextScene = 2;
	}
}

//
void AGameManager::NextSceneDown()
{
	if (!m_isSceneTransition) { return; }

	m_NextScene++;
	if ((int)m_NextScene.GetNextScene() > 2)
	{
		m_NextScene = 0;
	}
}

//ラップタイム並び替え
void AGameManager::RapTimeSort()
{
	TArray<int> SortRapTime;	//ソート用ラップタイム

	for (int i = 0; i < m_RapTimeText.Num(); i++)
	{
		SortRapTime.Add(FCString::Atof(*(m_RapTimeText[i])));
	}

	//ソート用ラップタイムの並び替え
	SortRapTime.Sort();

	for (int i = 0; i < m_RapTimeText.Num(); i++)
	{
		m_RapTimeText[i] = FString::SanitizeFloat(SortRapTime[i]);
	}
}

//レベルのロード処理
void AGameManager::LoadLevel(const FName& _level)
{
	m_bLoadComplete = false;
	UGameplayStatics::LoadStreamLevel(this, _level, false, false, m_LatentAction);
}

//レベルのアンロード処理
void AGameManager::UnloadLevel(const FName& _level)
{
	m_bLoadComplete = false;
	UGameplayStatics::UnloadStreamLevel(this, _level, m_LatentAction, false);
}

//レベルの表示処理
bool AGameManager::ShowLevel(const FName& _level) const
{
	ULevelStreaming* levelstream = UGameplayStatics::GetStreamingLevel(GetWorld(), _level);
	check(levelstream != nullptr);
	levelstream->SetShouldBeVisible(true);
	return levelstream->IsLevelVisible();
}

//レベルの非表示処理
bool AGameManager::HideLevel(const FName& _level) const
{
	ULevelStreaming* levelstream = UGameplayStatics::GetStreamingLevel(GetWorld(), _level);
	check(levelstream != nullptr);
	levelstream->SetShouldBeVisible(false);
	return !levelstream->IsLevelVisible();
}