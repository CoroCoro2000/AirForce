//--------------------------------------------------------------------------------------------
// �t�@�C����		:CheckPointActor.h
// �T�v				:���[�X���[�h���̃`�F�b�N�|�C���g�Ƃ��Ĉ����A�N�^�[
// �쐬��			:2021/08/27
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#include "CheckPointActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckPointActor::ACheckPointActor()
	: m_CheckPointCollision(NULL)
	, m_CheckPointNumber(0)
	, m_pNextCheckPointActor(NULL)
{
 	//���t���[���X�V����K�v���Ȃ����߁ATick������؂�
	PrimaryActorTick.bCanEverTick = false;

	//�{�b�N�X�R���|�[�l���g����
	m_CheckPointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointCollision"));
	if (m_CheckPointCollision)
	{
		RootComponent = m_CheckPointCollision;
	}

	//�`�F�b�N�|�C���g�̃^�O��ǉ�
	Tags.Add(TEXT("CheckPoint"));
}

// Called when the game starts or when spawned
void ACheckPointActor::BeginPlay()
{
	Super::BeginPlay();
	

	//���̃`�F�b�N�|�C���g�̔ԍ���ݒ�
	if (m_pNextCheckPointActor)
	{
		m_pNextCheckPointActor->SetNumber(m_CheckPointNumber + 1);
	}
}

// Called every frame
void ACheckPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

