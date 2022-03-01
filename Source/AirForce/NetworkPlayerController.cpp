//------------------------------------------------------------------------
// �t�@�C����		:ANetworkPlayerController.cpp
// �T�v				:�N���C�A���g��Pawn��HUD�̑�����s���R���g���[���[
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------


#include "NetworkPlayerController.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerDrone.h"
#include "NetworkGameState.h"
#include "NetworkPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"

//�R���X�g���N�^
ANetworkPlayerController::ANetworkPlayerController()
	: m_SynchronousInterval(0.25f)
	, m_bCanUpdate(true)
{
	//�v���C���[�����R���g���[����
	Role = ROLE_Authority;
	//�����Ώۃt���O
	bReplicates = true;
	//���L�������N���C�A���g�݂̂ɓ�������
	bOnlyRelevantToOwner = true;
}

//�Q�[���J�n���Ɏ��s�����֐�
void ANetworkPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

//���t���[�����s�����֐�
void ANetworkPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//���v���P�[�g��o�^
void ANetworkPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkPlayerController, m_bCanUpdate);
}

//�T�[�o�[�p�@Transform�K�p
void ANetworkPlayerController::Server_ApplyTransform_Implementation()
{
	//GameState����ePlayerState�ɃA�N�Z�X���A�N���C�A���g���X�V����Transform���T�[�o�[��ɔ��f����
	if (ANetworkGameState* pGameState = ANetworkGameState::Get())
	{
		for (APlayerState* pPlayerState : pGameState->PlayerArray)
		{
			if (ANetworkPlayerState* pNetworkPlayerState = Cast<ANetworkPlayerState>(pPlayerState))
			{
				//���g��Transform�͕ύX���Ă͂Ȃ�Ȃ����߁A�����ȊO�̏��L����PlayerState�ɃA�N�Z�X����
				if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(pNetworkPlayerState->GetOwner()))
				{
					if (pPlayerController != this)
					{
						//Transform��K�p
						if (APlayerDrone* pClientDrone = pPlayerController->GetPawn<APlayerDrone>())
						{
							FTransform PlayerTransform = pNetworkPlayerState->GetPlayerTransform();

							pClientDrone->SetActorLocation(PlayerTransform.GetLocation());
							pClientDrone->SetBodyMeshRotation(PlayerTransform.GetRotation());

#if WITH_EDITOR
							FString str = TEXT("Server::") + GetName() + TEXT("::SynchronizePlayerTransform()");
							UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
						}
					}
				}
			}
		}
	}
}

//�N���C�A���g�p�@Transform��n��
void ANetworkPlayerController::Client_SetTransform_Implementation()
{
	//GameState����ePlayerState�ɃA�N�Z�X���A�N���C�A���g���X�V����Transform���T�[�o�[��ɔ��f����
	if (ANetworkGameState* pGameState = ANetworkGameState::Get())
	{
		for (APlayerState* pPlayerState : pGameState->PlayerArray)
		{
			if (ANetworkPlayerState* pNetworkPlayerState = Cast<ANetworkPlayerState>(pPlayerState))
			{
				if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(pNetworkPlayerState->GetOwner()))
				{
					if (pPlayerController == this)
					{
						//�����̃v���C���[�X�e�[�g��Transform��n��
						if (APlayerDrone* pClientDrone = pPlayerController->GetPawn<APlayerDrone>())
						{
							FTransform LocalTransform(pClientDrone->GetBodyMeshRotation(), pClientDrone->GetActorLocation());
							pNetworkPlayerState->SetPlayerTransform(LocalTransform);

#if WITH_EDITOR
							FString str = TEXT("Client::") + GetName() + TEXT("::SynchronizePlayerTransform()");
							UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
						}
					}
				}
			}
		}
	}
}

//�v���C���[��Transform����
void ANetworkPlayerController::SynchronizePlayerTransform()
{
	if (!m_bCanUpdate) { return; }

	(GetRemoteRole() == ENetRole::ROLE_Authority) ? Server_ApplyTransform() : Client_SetTransform();

	m_bCanUpdate = false;

	//�w��b��ɍX�V�\�t���O�𗧂Ă�֐����Ăяo���悤�ɓo�^
	FLatentActionInfo LatentAction(0, 0, TEXT("ActivateUpdate"), this);
	UKismetSystemLibrary::Delay(this, m_SynchronousInterval, LatentAction);
}

//�v���C���[���̍X�V
void ANetworkPlayerController::ActivateUpdate()
{
	m_bCanUpdate = true;

#if WITH_EDITOR
	FString str = GetName() + TEXT("::ActivateUpdate()");
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
}

//�Q�[���R���g���[���[�擾
ANetworkPlayerController* ANetworkPlayerController::Get()
{
	ANetworkPlayerController* pPlayerController = nullptr;
	if (GEngine)
	{
		if (UWorld* pWorld = GEngine->GetWorld())
		{
			pPlayerController = pWorld->GetFirstPlayerController<ANetworkPlayerController>();
		}
	}
	return pPlayerController;
}