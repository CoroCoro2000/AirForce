//-------------------------------------------------------------------------------------
// ファイル名	:GameManager.h
// 概要				:ゲームの進行を管理するクラス
// 作成日			:2021/07/07
// 作成者			:19CU0104 池田翔一郎
// 更新内容		:
//-------------------------------------------------------------------------------------

//インクルード
#include "GameManager.h"
#include "PlayerDrone.h"
#include "GameUtility.h"
#include "Sound/SoundBase.h"
#include "Misc/FileHelper.h"
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
	, m_CountDownText(TEXT(""))
	, m_RapTime(0.f)
	, m_RapDefaultText(TEXT(""))
	, m_PlayerRank(0)
	, m_RankingDisplayNum(5)
	, m_isScoreWrite(false)
	, m_isNewRecord(false)
	, m_PlayerDrone(NULL)
	, m_GhostDrone(NULL)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ処理
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	//ファイル読み込み
	FFileHelper::LoadFileToStringArray(m_RapTimeText, *(FPaths::ProjectDir() + m_RapTimeTextPath));

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);
		//入力マッピング
		InputComponent->BindAction(TEXT("InputUp"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneUp);
		InputComponent->BindAction(TEXT("InputDown"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneDown);
	}

	//	ドローンの検索
	AActor* pDrone = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (pDrone)
	{
		m_PlayerDrone = Cast<ADroneBase>(pDrone);
	}

	//既にプレイした人がいたならゴーストドローンを生成
	if (m_RapTimeText.Num() > 0)
	{
		FString ghostPath = TEXT("/Game/BP/GhostDroneBP.GhostDroneBP_C");
		TSubclassOf<AActor> ghostSoftClass = TSoftClassPtr<AActor>(FSoftObjectPath(*ghostPath)).LoadSynchronous(); // 上記で設定したパスに該当するクラスを取得
		if (ghostSoftClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("GhostSpawn"));
			AActor* ghostDrone = GetWorld()->SpawnActor<AActor>(ghostSoftClass); // スポーン処理
			m_GhostDrone = Cast<ADroneBase>(ghostDrone);
			m_GhostDrone->SetActorLocation(m_PlayerDrone->GetActorLocation());
			m_GhostDrone->SetActorRotation(m_PlayerDrone->GetActorRotation());
		}
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
			CountDown(DeltaTime);
		}
			
		//レースが始まっているならラップタイムを計測する
		if (m_isStart & !m_isGoal)
		{
			m_RapTime += DeltaTime;
		}
		
		//ゴールしたなら
		if (m_isGoal)
		{
			//スコアを書き込みしたか確認
			if (!m_isScoreWrite)
			{
				//ラップタイムをテキストファイルに記入
				m_RapTime = CGameUtility::SetDecimalTruncation(m_RapTime, 3);
				FString rapText = GetRapMinuteText().ToString() + ":" + GetRapSecondText().ToString() + "." + GetRapMiliSecondText().ToString();
				m_RapTimeText.Add(rapText);

				//ラップタイム並び替え
				RapTimeSort();

				//今回のタイムのランキング確認
				for (int i = m_RapTimeText.Num() - 1; i >= 0; i--)
				{
					if (rapText == m_RapTimeText[i])
					{
						m_PlayerRank = i;
						break;
					}
				}

				//今回のタイムが1位だったら
				if (m_PlayerRank == 0)
				{
					m_isNewRecord = true;
					APlayerDrone* player = Cast<APlayerDrone>(m_PlayerDrone);
					if (player)
					{
						player->WritingRaceVector();
						player->WritingRaceQuaternion();
					}

				}

				//ランキング外のスコアを削除
				if (m_RapTimeText.Num() > m_RankingDisplayNum)
				{
					for (int i = m_RankingDisplayNum; i < m_RapTimeText.Num(); i++)
					{
						m_RapTimeText.RemoveAt(i);
					}
				}

				//テキストファイル書き込み
				FFileHelper::SaveStringArrayToFile(m_RapTimeText, *(FPaths::ProjectDir() + m_RapTimeTextPath));

				//書き込みフラグをONにする
				m_isScoreWrite = true;
			}
			
		}
		if (m_PlayerDrone)
		{
			//	レースがスタートして、ゴールしていない間操作可能にする

			m_PlayerDrone->SetisControl((m_isStart && !m_isGoal));
			if (m_isGoal)
			{
				UE_LOG(LogTemp, Warning, TEXT("Gooooooooooooooooooooooal"));

			}
		}

		if (m_GhostDrone)
		{
			//	レースがスタートして、ゴールしていない間操作可能にする
			m_GhostDrone->SetisControl((m_isStart));
			//m_GhostDrone->SetHidden(!m_isStart);
		}

		break;
	case ECURRENTSCENE::SCENE_RESULT:
		break;
	default:
		break;
	}
}

//カウントダウン処理
void AGameManager::CountDown(float DeltaTime)
{
	FString m_prevCountDownText = m_CountDownText;	//1フレーム前のカウントダウンテキスト

	m_CountDownTime -= DeltaTime;
	m_CountDownText = FString::FromInt(int(m_CountDownTime) + 1);

	if (m_CountDownText != m_prevCountDownText)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), m_CountDownSE);
	}

	if (m_CountDownTime <= 0.f)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), m_StartSE);
		m_CountDownText = "";
		m_isStart = true;
	}

}//
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
	FString SortRapTime;	//ソート用ラップタイム

	//バブルソートで並び替え
	for (int i = 0; i < m_RapTimeText.Num()-1; i++)
	{
		for (int j = i+1; j < m_RapTimeText.Num(); j++)
		{
			if (m_RapTimeText[i] > m_RapTimeText[j])
			{
				SortRapTime = m_RapTimeText[i];
				m_RapTimeText[i] = m_RapTimeText[j];
				m_RapTimeText[j] = SortRapTime;
			}
		}
	}
}