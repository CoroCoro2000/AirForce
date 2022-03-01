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
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerDrone.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR

//�R���X�g���N�^
ANetworkGameState::ANetworkGameState()
{
	//�v���C���[�����R���g���[����
	Role = ROLE_Authority;
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

//���t���[�����s�����֐�
void ANetworkGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�Q�[���X�e�[�g�擾
ANetworkGameState* ANetworkGameState::Get()
{
	ANetworkGameState* pGameState = nullptr;
	if (GEngine)
	{
		if (UWorld* pWorld = GEngine->GetWorld())
		{
			pGameState = pWorld->GetGameState<ANetworkGameState>();
		}
	}
	return pGameState;
}