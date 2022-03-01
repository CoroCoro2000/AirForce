//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:ANetworkPlayerState.cpp
// �T�v				:�N���C�A���g��PlayerController����Transform���󂯎��A�ێ�����v���C���[�X�e�[�g�N���X�@�T�[�o�[�ƃN���C�A���g�����ꂼ��1�����L���A�����Ɋ֌W�Ȃ��ǂݎ�邱�Ƃ��ł���B
// �쐬��			:2022/03/01
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "NetworkPlayerState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

//�R���X�g���N�^
ANetworkPlayerState::ANetworkPlayerState()
	: m_PlayerTransform(FTransform::Identity)
{
	//�v���C���[�����R���g���[����
	Role = ROLE_Authority;
	//�����Ώۃt���O
	bReplicates = true;
	//���L�������N���C�A���g�݂̂ɓ�������
	bOnlyRelevantToOwner = false;
}

	//�Q�[���J�n���Ɏ��s
void ANetworkPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

//���t���[�����s
void ANetworkPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//���v���P�[�g��o�^
void ANetworkPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkPlayerState, m_PlayerTransform);
}