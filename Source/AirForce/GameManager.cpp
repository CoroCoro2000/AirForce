//-------------------------------------------------------------------------------------
// ファイル名	:GameManager.h
// 概要				:ゲームの進行を管理するクラス
// 作成日			:2021/07/07
// 作成者			:19CU0104 池田翔一郎
// 更新内容		:
//-------------------------------------------------------------------------------------

//インクルード
#include "GameManager.h"
#include "Utility/GameUtility.h"
#include "Kismet/GameplayStatics.h"

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
	, m_Drone(NULL)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//ゲーム開始時に1度だけ処理
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);
		//入力マッピング
		InputComponent->BindAction(TEXT("InputUp"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneUp);
		InputComponent->BindAction(TEXT("InputDown"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneDown);

		UE_LOG(LogTemp, Warning, TEXT("InputBind"));
	}

	if (m_GoalRing)
	{
		//何個リングを通ったらゴールになるか確認する
		m_GoalRingNumber = m_GoalRing->GetRingNumber();
	}
	else
	{
		m_GoalRingNumber = 0;
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
			m_CountDownTime -= DeltaTime;

		//レースが始まっているならラップタイムを計測する
		if(m_isStart & !m_isGoal)
			m_RapTime += DeltaTime;

		if (m_isGoal)
		{
			SetRapTimeDecimalTruncation(3.f);
			//UE_LOG(LogTemp, Warning, TEXT("%f"), m_RapTime);
		}

		if (m_CountDownTime <= 1.f)
			m_isStart = true;

		m_isGoal = GetConfirmationGoal(m_Drone->GetRingAcquisition(), m_GoalRingNumber);
		//	レースがスタートして、ゴールしていない間操作可能にする
		m_Drone->SetisControl((m_isStart & !m_isGoal) );
		break;
	case ECURRENTSCENE::SCENE_RESULT:
		break;
	default:
		break;
	}
}

//小数第n位未満切り捨て
void AGameManager::SetRapTimeDecimalTruncation(float n)
{
	m_RapTime = m_RapTime * FMath::Pow(10, n);
	m_RapTime = floor(m_RapTime);
	m_RapTime /= FMath::Pow(10, n);
}

//
void AGameManager::NextSceneUp()
{
	if (!m_isSceneTransition)
		return;
	m_NextScene--;
	if ((int)m_NextScene.GetNextScene() > 2)
			m_NextScene._NextScene = TEnumAsByte<ENEXTSCENE::Type>(2);
	UE_LOG(LogTemp, Warning, TEXT("--%d"), (int)m_NextScene.GetNextScene());
}

//
void AGameManager::NextSceneDown()
{
	if (!m_isSceneTransition)
		return;
	m_NextScene++;
	if ((int)m_NextScene.GetNextScene() > 2)
		m_NextScene._NextScene = TEnumAsByte<ENEXTSCENE::Type>(0);
	UE_LOG(LogTemp, Warning, TEXT("++%d"), (int)m_NextScene.GetNextScene());
}

