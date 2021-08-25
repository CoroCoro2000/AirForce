//-------------------------------------------------------------------------------------
// �t�@�C����	:GameManager.h
// �T�v				:�Q�[���̐i�s���Ǘ�����N���X
// �쐬��			:2021/07/07
// �쐬��			:19CU0104 �r�c�Ĉ�Y
// �X�V���e		:
//-------------------------------------------------------------------------------------

//�C���N���[�h
#include "GameManager.h"
#include "GameUtility.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"

#define ScoreTxt "Score.txt"
//�R���X�g���N�^
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
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n����1�x��������
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	FFileHelper::LoadFileToStringArray(m_RapTimeText, *(FPaths::GameDir() + ScoreTxt));

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);
		//���̓}�b�s���O
		InputComponent->BindAction(TEXT("InputUp"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneUp);
		InputComponent->BindAction(TEXT("InputDown"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneDown);

		UE_LOG(LogTemp, Warning, TEXT("InputBind"));
	}

	//	�h���[���̌���
	AActor* pDrone = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (pDrone)
	{
		m_Drone = Cast<ADroneBase>(pDrone);
	}
}

//���t���[������
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (m_CurrentScene)
	{
	case ECURRENTSCENE::SCENE_TITLE:

		break;
	case ECURRENTSCENE::SCENE_FIRST:
		//���[�X���n�܂��Ă��Ȃ��Ȃ�J�E���g�_�E��������
		if (!m_isStart)
		{
			m_CountDownTime -= DeltaTime;
		}
			
		//���[�X���n�܂��Ă���Ȃ烉�b�v�^�C�����v������
		if (m_isStart & !m_isGoal)
		{
			m_RapTime += DeltaTime;
		}
			
		if (m_isGoal)
		{
			if (!m_isScoreWrite)
			{
				m_RapTime = CGameUtility::SetDecimalTruncation(m_RapTime, 3);
				m_RapTimeText.Add(FString::SanitizeFloat(m_RapTime));

				//���b�v�^�C�����ёւ�
				m_RapTimeText.Sort();

				if (m_RapTimeText.Num() > m_RankingDisplayNum)
				{
					for (int i = m_RankingDisplayNum; i < m_RapTimeText.Num(); i++)
					{
						m_RapTimeText.RemoveAt(i);
					}
				}
				FFileHelper::SaveStringArrayToFile(m_RapTimeText, *(FPaths::GameDir() + ScoreTxt));
				m_isScoreWrite = true;
				//UE_LOG(LogTemp, Warning, TEXT("%f"), m_RapTime);
			}
			
		}

		if (m_CountDownTime <= 1.f)
		{
			m_isStart = true;
		}
			
		//	���[�X���X�^�[�g���āA�S�[�����Ă��Ȃ��ԑ���\�ɂ���
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

//���b�v�^�C�����ёւ�
void AGameManager::RapTimeSort()
{
	FString _Time;
	for (int i = 0; i < m_RapTimeText.Num(); i++)
	{
	}
}

