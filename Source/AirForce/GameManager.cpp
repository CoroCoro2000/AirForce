//-------------------------------------------------------------------------------------
// �t�@�C����	:GameManager.h
// �T�v				:�Q�[���̐i�s���Ǘ�����N���X
// �쐬��			:2021/07/07
// �쐬��			:19CU0104 �r�c�Ĉ�Y
// �X�V���e		:
//-------------------------------------------------------------------------------------

//�C���N���[�h
#include "GameManager.h"
#include "PlayerDrone.h"
#include "Train.h"
#include "GameUtility.h"
#include "Sound/SoundBase.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"

#include "GhostDrone.h"

//�R���X�g���N�^
AGameManager::AGameManager()
	: m_CurrentScene(ECURRENTSCENE::SCENE_TITLE)
	, m_NextScene()
	, m_isCountDownEnable(false)
	, m_isPause(NULL)
	, m_isStart(false)
	, m_isGoal(false)
	, m_isSceneTransition(false)
	, m_bAllSetup(false)
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

//�Q�[���J�n����1�x��������
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	//�t�@�C���ǂݍ���
	FFileHelper::LoadFileToStringArray(m_RapTimeText, *(FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveRapTimeTextPath));

	//	�h���[���̌���
	AActor* pDrone = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (pDrone)
	{
		m_PlayerDrone = Cast<ADroneBase>(pDrone);
		m_PlayerDrone->SetStagePath(m_SaveStageFolderPath);
	}

	if (m_CurrentScene == ECURRENTSCENE::SCENE_FIRST)
	{
		//���Ƀv���C�����l�������Ȃ�
		if (m_RapTimeText.Num() > 0)
		{
			//�S�[�X�g�h���[���𐶐�
			FString ghostPath = TEXT("/Game/BP/GhostDroneBP.GhostDroneBP_C");
			TSubclassOf<AActor> ghostSoftClass = TSoftClassPtr<AActor>(FSoftObjectPath(*ghostPath)).LoadSynchronous(); // ��L�Őݒ肵���p�X�ɊY������N���X���擾
			if (ghostSoftClass)
			{
				AActor* ghostDrone = GetWorld()->SpawnActor<AActor>(ghostSoftClass); // �X�|�[������
				m_GhostDrone = Cast<AGhostDrone>(ghostDrone);
				if (m_GhostDrone)
				{
					m_GhostDrone->SetActorLocation(m_PlayerDrone->GetActorLocation());
					m_GhostDrone->SetActorRotation(m_PlayerDrone->GetActorRotation());
					m_GhostDrone->SetStagePath(m_SaveStageFolderPath);
					//���[�X�̍��W�t�@�C���ǂݍ���
					m_GhostDrone->LoadingRaceVectorFile();
					//���[�X�̃N�I�[�^�j�I���t�@�C���ǂݍ���
					m_GhostDrone->LoadingRaceQuaternionFile();
				}
			}
		}
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

		//�J�E���g�_�E������
		CountDown(DeltaTime);
	
		
		//���[�X���n�܂��Ă���Ȃ烉�b�v�^�C�����v������
		if (m_isStart && !m_isGoal)
		{
			m_RapTime += DeltaTime;
			if (m_RapTime > 599.999f)
			{
				m_RapTime = 599.999f;
			}
		}
		
		//���U���g����
		Result(DeltaTime);

		if (m_PlayerDrone)
		{
			//���[�X���X�^�[�g���āA�S�[�����Ă��Ȃ��ԑ���\�ɂ���
			m_PlayerDrone->SetisControl((m_isStart && !m_isGoal));
		}

		if (m_GhostDrone)
		{
			//���[�X���X�^�[�g���Ă���ԑ���\�ɂ���
			m_GhostDrone->SetisControl((m_isStart));
		}

		break;
	case ECURRENTSCENE::SCENE_RESULT:
		break;
	default:
		break;
	}
}

//���͉\�ɂ���
void AGameManager::EnableResultInput()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		EnableInput(PlayerController);
		//���̓}�b�s���O
		InputComponent->BindAction(TEXT("InputUp"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneUp);
		InputComponent->BindAction(TEXT("InputDown"), EInputEvent::IE_Pressed, this, &AGameManager::NextSceneDown);
	}
}

//�J�E���g�_�E������
void AGameManager::CountDown(float DeltaTime)
{
	//�X�^�[�g�����Ȃ珈�����Ȃ�
	if (m_isStart) { return; }
	if (!m_isCountDownEnable) { return; }

	FString m_prevCountDownText = m_CountDownText;	//1�t���[���O�̃J�E���g�_�E���e�L�X�g

	ATrain* pTrain = CGameUtility::GetActorFromTag<ATrain>(this, TEXT("Train"));

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
		if (pTrain)
		{
			pTrain->SetIsMove(true);
		}
	}
}

//���U���g����
void AGameManager::Result(float DeltaTime)
{
	//�S�[�����Ă��Ȃ��Ȃ珈�����Ȃ�
	if (!m_isGoal) { return; }

	//�X�R�A���������݂������m�F
	if (!m_isScoreWrite)
	{
		//���b�v�^�C�����e�L�X�g�t�@�C���ɋL��
		m_RapTime = CGameUtility::SetDecimalTruncation(m_RapTime, 3);
		FString rapText = GetRapMinuteText().ToString() + ":" + GetRapSecondText().ToString() + "." + GetRapMiliSecondText().ToString();
		m_RapTimeText.Add(rapText);

		//���b�v�^�C�����ёւ�
		RapTimeSort();

		//����̃^�C���̃����L���O�m�F
		for (int i = m_RapTimeText.Num() - 1; i >= 0; i--)
		{
			if (rapText == m_RapTimeText[i])
			{
				m_PlayerRank = i;
				break;
			}
		}

		APlayerDrone* player = Cast<APlayerDrone>(m_PlayerDrone);
		
		//����̃^�C����1�ʂ�������
		if (m_PlayerRank == 0)
		{
			m_isNewRecord = true;
		}

		//�����L���O�O�̃X�R�A���폜
		if (m_RapTimeText.Num() > m_RankingDisplayNum)
		{
			for (int i = m_RankingDisplayNum; i < m_RapTimeText.Num(); i++)
			{
				m_RapTimeText.RemoveAt(i);
			}
		}

		//�e�L�X�g�t�@�C����������
		FFileHelper::SaveStringArrayToFile(m_RapTimeText, *(FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveRapTimeTextPath));

		//�������݃t���O��ON�ɂ���
		m_isScoreWrite = true;
	}
}

//
void AGameManager::NextSceneUp()
{
	if (!m_isSceneTransition) { return; }
		
	if ((int)m_NextScene.GetNextScene() == 0)
	{
		m_NextScene = 1;
	}
	else
	{
		m_NextScene--;
	}

#if WITH_EDITOR
	UKismetSystemLibrary::PrintString(this, FString::FromInt((int32)m_NextScene.GetNextScene()), true, false);
#endif // WITH_EDITOR
}

//
void AGameManager::NextSceneDown()
{
	if (!m_isSceneTransition) { return; }

	m_NextScene++;
	if ((int)m_NextScene.GetNextScene() > 1)
	{
		m_NextScene = 0;
	}
#if WITH_EDITOR
	UKismetSystemLibrary::PrintString(this, FString::FromInt((int32)m_NextScene.GetNextScene()), true, false);
#endif // WITH_EDITOR
}

//���b�v�^�C�����ёւ�
void AGameManager::RapTimeSort()
{
	FString SortRapTime;	//�\�[�g�p���b�v�^�C��

	//�o�u���\�[�g�ŕ��ёւ�
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