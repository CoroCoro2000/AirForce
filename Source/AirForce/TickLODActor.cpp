//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:TickLODActor.cpp
// �T�v				:1�t���[���������Tick�X�V�񐔂�ύX�ł���A�N�^�[�̃x�[�X�N���X
// �쐬��			:2022/03/09
// �쐬��			:19CU0105 �r������
//------------------------------------------------------------------------------------------------------------------------------------------------

#include "TickLODActor.h"
#include "ConfigParameter.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR


// Sets default values
ATickLODActor::ATickLODActor()
	: m_TickFPS(60.f)
	, m_LastTickTime(0.f)
	, m_ActorNumber(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATickLODActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATickLODActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�����\�ȃt���[��������
bool ATickLODActor::IsProcessableFrame(const float& currentTime)const
{
	//���̃A�N�^�[�̔ԍ�����Ȃ��t���[���̎��̂ݏ����ł���
	//�����Ȃ�����t���[���̎����������ł���
	//�o�߂������Ԃ��猻�݉��t���[���ڂ���������
	bool isEvenFrame = FMath::RoundToInt(currentTime * CConfigParameter::FixedFrameRate) & 1;

	return (m_ActorNumber & 1) ? isEvenFrame : !isEvenFrame;
}