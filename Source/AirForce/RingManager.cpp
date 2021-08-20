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
#include "Ring.h"
#include "DroneBase.h"
#include "Utility/GameUtility.h"

//�R���X�g���N�^
ARingManager::ARingManager()
	: m_MaxRingCount(0)
	, m_RingDrawUpNumber(5)
	, m_pDrone(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n����1�x�����Ă΂�鏈��
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//�h���[�����������A�����擾
	AActor* FindActor = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (FindActor)
	{
		m_pDrone = Cast<ADroneBase>(FindActor);
	}

	//�z�u���������O�̐����擾
	m_MaxRingCount = (int)m_pChildRings.Num();

	//TArray�Ɋi�[���ꂽ�Ⴂ�v�f�̃����O���珇�ɔԍ��t������
	int index = 1;
	for (ARing* pRing : m_pChildRings)
	{
		if (pRing)
		{
			pRing->SetRingNumber(index);
		}
		++index;
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

	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			//�����O�̏�ԍX�V
			m_pChildRings[index]->SetActivate(IsDraw(index));

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
