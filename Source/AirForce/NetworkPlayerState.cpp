//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:ANetworkPlayerState.cpp
// �T�v				:�N���C�A���g��PlayerController����Transform���󂯎��A�ێ�����v���C���[�X�e�[�g�N���X�@�T�[�o�[�ƃN���C�A���g�����ꂼ��1�����L���A�����Ɋ֌W�Ȃ��ǂݎ�邱�Ƃ��ł���B
// �쐬��			:2022/03/01
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "NetworkPlayerState.h"
#include "NetworkPlayerController.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"

//�R���X�g���N�^
ANetworkPlayerState::ANetworkPlayerState()
	: m_ReplicatedPlayer(FTransform::Identity, -1)
	, m_pReplicatedDrone(nullptr)
	, m_bCanUpdate(false)
{
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

	DOREPLIFETIME(ANetworkPlayerState, m_ReplicatedPlayer);
	DOREPLIFETIME(ANetworkPlayerState, m_pReplicatedDrone);
	DOREPLIFETIME(ANetworkPlayerState, m_bCanUpdate);
}

void ANetworkPlayerState::OnRep_m_ReplicatedPlayer()
{
	//�v���C���[�̍X�V�����ꂽ��X�V�\�t���O�𗧂Ă�
	m_bCanUpdate = true;

#if WITH_EDITOR
	FString str = GetName() + TEXT("::Replicated");
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
}

void ANetworkPlayerState::OnRep_m_pReplicatedDrone()
{
	//�v���C���[�̍X�V�����ꂽ��X�V�\�t���O�𗧂Ă�
	m_bCanUpdate = true;

#if WITH_EDITOR
	FString str = GetName() + TEXT("::Replicated");
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
}