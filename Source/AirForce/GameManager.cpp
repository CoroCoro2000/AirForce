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
	, m_Drone(NULL)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n����1�x��������
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

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
			//m_RapScore = CGameUtility::SetDecimalTruncation(m_RapTime, 3);
			m_RapScore.Add(m_RapTime);
			FString txt = "Score.txt";
			FFileHelper::SaveStringToFile(FString::SanitizeFloat(m_RapTime), *(FPaths::GameDir() + txt));
			//UE_LOG(LogTemp, Warning, TEXT("%f"), m_RapTime);
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

