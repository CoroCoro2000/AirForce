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
	: m_pDrone(NULL)
	, m_pGameManager(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n����1�x�����Ă΂�鏈��
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//TArray�Ɋi�[���ꂽ�Ⴂ�v�f�̃����O���珇�ɔԍ��t������
	for (int index = 0; index < (int)m_pChildRings.Num(); ++index)
	{
		if (m_pChildRings[index])
		{
			//m_pChildRings[index]->SetRingNumber(index);
		}
	}

	//�h���[���ƃQ�[���}�l�[�W���[�������A�ێ�����
	TSubclassOf<AActor> findClass = AActor::StaticClass();
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
	UpdateRingColor();

	if (m_pGameManager && m_pChildRings[0])
	{
		m_pGameManager->SetIsGoal(m_pChildRings[0]->GetIsPassed());
	}
	
}

//�����O���̍X�V����
void ARingManager::UpdateRingColor()
{


}
