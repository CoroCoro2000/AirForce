//-----------------------------------------------------------------------------------------------------------------
// �t�@�C����		:RingManager.cpp
// �T�v				:�����O���Ǘ�����A�N�^�[�N���X
// �쐬��			:2021/08/20
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/08/20	�r������		:	�Q�[���J�n���ɂ��ׂẴ����O�ɔԍ������蓖�Ă鏈�����쐬
//						:2021/08/20	�r������		:	�����O��������ꂽ��z�񂩂�폜���鏈�����쐬
//-----------------------------------------------------------------------------------------------------------------

//�C���N���[�h
#include "RingManager.h"
#include "GameManager.h"
#include "Ring.h"
#include "DroneBase.h"
#include "GameUtility.h"
#include "Kismet/GameplayStatics.h"

//�R���X�g���N�^
ARingManager::ARingManager()
	: m_MaxRingCount(0)
	, m_RingCount(0)
	, m_RingDrawUpNumber(5)
	, m_pDrone(NULL)
	, m_pGameManager(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n����1�x�����Ă΂�鏈��
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//�z�u���������O�̐����擾
	m_MaxRingCount = (int)m_pChildRings.Num();
	m_RingCount = m_MaxRingCount;

	//TArray�Ɋi�[���ꂽ�Ⴂ�v�f�̃����O���珇�ɔԍ��t������
	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			m_pChildRings[index]->SetRingNumber(index);
		}
	}

	//�h���[���ƃQ�[���}�l�[�W���[���������A�����擾
	TSubclassOf<AActor> findClass;
	findClass = AActor::StaticClass();
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

//���t���[���Ă΂�鏈��
void ARingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�����O�����X�V
	UpdateRingInfo();
}

//�����O��`�悷�邩�ǂ����̔���
bool ARingManager::IsDraw(const int& _ringIndex) const
{
	return (m_pChildRings[_ringIndex]->GetRingNumber() - m_pDrone->GetRingAcquisition() <= m_RingDrawUpNumber) ? true : false;
}

//�����O���̍X�V����
void ARingManager::UpdateRingInfo()
{
	if (!m_pDrone) { return; }

	//�����O�̐���0�ȉ��Ȃ�
	if (m_RingCount <= 0)
	{
		if (m_pGameManager)
		{
			m_pGameManager->SetIsGoal(true);
		}
	}

	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			//�����O�̏�ԍX�V
			m_pChildRings[index]->SetActivate(IsDraw(index));

			//�ʉ߂����u��
			if (m_pChildRings[index]->IsPassBegin())
			{
				//�����O�̐������炷
				--m_RingCount;
				m_pChildRings[index]->SetPassBegin(false);
				UE_LOG(LogTemp, Warning, TEXT("m_RingCount%i"), m_RingCount);
			}

			//�ʉ߂��ꂽ�����O�Ƀh���[���̍��W��n��
			if (m_pChildRings[index]->IsPassed())
			{
				m_pChildRings[index]->SetDroneLocation(m_pDrone->GetActorLocation());
			}

			//�����O���j�󂳂ꂽ��z�񂩂�폜����
			if (m_pChildRings[index]->IsDestroy())
			{
				m_pChildRings.RemoveAt(index);
			}
		}
	}
}
