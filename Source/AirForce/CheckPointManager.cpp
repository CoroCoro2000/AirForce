//------------------------------------------------------------------------
// �t�@�C����		:CheckPointManager.cpp
// �T�v				:�`�F�b�N�|�C���g���Ǘ�����}�l�[�W���[�N���X
// �쐬��			:2021/09/13
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/09/13
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
	
	//���ʔԍ��ݒ�
	int CheckPointNumber = 0;
	for (ACheckPoint* pCheckPoint : m_pCheckPoints)
	{
		if (pCheckPoint)
		{
			pCheckPoint->SetNumber(CheckPointNumber);

			//0�Ԗڂ̃`�F�b�N�|�C���g�����A�N�e�B�u�ȏ�Ԃɂ��Ă���
			if (pCheckPoint->GetNumber() == 0)
			{
				pCheckPoint->SetEveryUpdate(true);
			}

			//���̃`�F�b�N�|�C���g�𓯔ԍ��ɂ���ꍇ�̓C���N�������g�����Ȃ�
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
			//�ʉߍς݂̃`�F�b�N�|�C���g�̌����������I�����A�z�񂩂�O��
			if (pCheckPoint->GetIsPassed())
			{
				pCheckPoint->SetEveryUpdate(false);
				m_pCheckPoints.Remove(pCheckPoint);
			}
		}
	}
}

