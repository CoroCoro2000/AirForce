//------------------------------------------------------------------------
// �t�@�C����		:ANetworkGameState.cpp
// �T�v				:�T�[�o�[�@�́@�N���C�A���g�Ԃ̂������Ǘ�����@�T�[�o�[�ƃN���C�A���g�S�̂�1�����L����B
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#include "NetworkGameState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkPlayerState.h"
#include "PlayerDrone.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR

//�R���X�g���N�^
ANetworkGameState::ANetworkGameState()
{
	//�����Ώۃt���O
	bReplicates = true;
	//���L�������N���C�A���g�݂̂ɓ�������
	bOnlyRelevantToOwner = false;
}

//�Q�[���J�n���Ɏ��s�����֐�
void ANetworkGameState::BeginPlay()
{
	Super::BeginPlay();

}

//���v���P�[�g��o�^
void ANetworkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkGameState, m_pPlayerDrones);
}

//���t���[�����s�����֐�
void ANetworkGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�Q�[���X�e�[�g�擾
ANetworkGameState* ANetworkGameState::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (UWorld* pWorld = WorldContextObject->GetWorld())
		{
			pWorld->GetGameState<ANetworkGameState>();
		}
	}

	return nullptr;
}

//PlayerState�̔z����擾
TArray<ANetworkPlayerState*> ANetworkGameState::GetPlayerState()const
{
	TArray<ANetworkPlayerState*> pNetworkPlayerArray;

	for (APlayerState* pPlayerState : PlayerArray)
	{
		pNetworkPlayerArray.Add(Cast<ANetworkPlayerState>(pPlayerState));
	}

	return pNetworkPlayerArray;
}

//�v���C���[�̓o�^
void ANetworkGameState::EntryPlayer(APlayerDrone* pEntryDrone)
{
	for (APlayerDrone* pEntryedDrone : m_pPlayerDrones)
	{
		if (pEntryDrone == pEntryedDrone)
		{
#if WITH_EDITOR
			FString str = TEXT("Entryed::") + pEntryDrone->GetName();
			UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR

			break;
		}
	}

#if WITH_EDITOR
	FString str = TEXT("Entry::") + pEntryDrone->GetName();
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR

	m_pPlayerDrones.Add(pEntryDrone);
}