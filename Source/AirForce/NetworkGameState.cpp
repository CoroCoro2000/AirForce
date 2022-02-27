//------------------------------------------------------------------------
// �t�@�C����		:ANetworkGameState.h
// �T�v				:�T�[�o�[�@�́@�N���C�A���g�Ԃ̂������Ǘ�����Q�[���X�e�[�g
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#include "NetworkGameState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "PlayerDrone.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR


//�R���X�g���N�^
ANetworkGameState::ANetworkGameState()
	: m_PlayerIndex(0)
{

	//���v���P�[�g��L����
	bReplicates = true;
	//���v���P�[�g���s���Ԋu(�b�P��)��ݒ�
	NetUpdateFrequency = 10;
	MinNetUpdateFrequency = 5;
}

//�Q�[���J�n���Ɏ��s�����֐�
void ANetworkGameState::BeginPlay()
{
	Super::BeginPlay();

}

//���t���[�����s�����֐�
void ANetworkGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//���v���P�[�g����v���p�e�B���X�V����
void ANetworkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkGameState, m_pPlayerDrones);
}

//���[�J���ő��삵�Ă���v���C���[���擾
APlayerDrone* ANetworkGameState::GetLocalPlayerDrone()const
{
	for (APlayerState* pPlayerState : PlayerArray)
	{
		if (pPlayerState)
		{
			if (APawn* pPawn = pPlayerState->GetPawn())
			{
				if (pPawn->IsLocallyControlled())
				{
#if WITH_EDITOR
					UKismetSystemLibrary::PrintString(GetWorld(), pPawn->GetName(), true, false);
#endif // WITH_EDITOR

					return Cast<APlayerDrone>(pPawn);
				}
			}
		}
	}

	return nullptr;
}

void ANetworkGameState::OnRep_CurrentDrone()
{
	if (m_pPlayerDrones.IsValidIndex(m_PlayerIndex))
	{
		m_pPlayerDrones[m_PlayerIndex] = GetLocalPlayerDrone();
	}
}