//--------------------------------------------------------------------------------------------
// �t�@�C����		:CheckPointActor.h
// �T�v				:���[�X���[�h���̃`�F�b�N�|�C���g�Ƃ��Ĉ����A�N�^�[
// �쐬��			:2021/08/27
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#include "CheckPointActor.h"
#include "Components/BoxComponent.h"
#include "PlayerDrone.h"

// Sets default values
ACheckPointActor::ACheckPointActor()
	: m_pCheckPointCollision(NULL)
	, m_pNextCheckPointActor(NULL)
{
 	//���t���[���X�V����K�v���Ȃ����߁ATick������؂�
	PrimaryActorTick.bCanEverTick = false;

	//�{�b�N�X�R���|�[�l���g����
	m_pCheckPointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckPointCollision"));
	if (m_pCheckPointCollision)
	{
		RootComponent = m_pCheckPointCollision;
	}

	//�`�F�b�N�|�C���g�̃^�O��ǉ�
	Tags.Add(TEXT("CheckPoint"));
}

// Called when the game starts or when spawned
void ACheckPointActor::BeginPlay()
{
	Super::BeginPlay();
	
	//�R���W�������I�[�o�[���b�v�������Ăяo���C�x���g�֐���o�^
	if (m_pCheckPointCollision)
	{
		m_pCheckPointCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckPointActor::OnComponentOverlapBegin);
	}
}

// Called every frame
void ACheckPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�I�u�W�F�N�g���I�[�o�[���b�v�������Ă΂��C�x���g�֐�
void ACheckPointActor::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�I�[�o�[���b�v�����A�N�^�[�̃^�O��Drone�Ȃ�
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			APlayerDrone* pPlayerDrone = Cast<APlayerDrone>(OtherActor);

			if (pPlayerDrone)
			{
				//�v���C���[���w���Ă���`�F�b�N�|�C���g�̃A�h���X�����g�̃A�h���X�Ɠ����Ȃ玟�̃`�F�b�N�|�C���g����n���A�폜����
				if (pPlayerDrone->GetCheckPoint() == this)
				{
					pPlayerDrone->SetNextCheckPoint(m_pNextCheckPointActor);
					Destroy();
				}
			}
		}
	}
}