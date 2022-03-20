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
{
	//�����Ώۃt���O
	bReplicates = true;
	//���L�������N���C�A���g�݂̂ɓ�������
	bOnlyRelevantToOwner = true;
	bAlwaysRelevant = true;
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

}

//�T�[�o�[�p�@Transform�K�p
void ANetworkPlayerController::Server_ApplyTransform()
{
	//GameState�擾
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//�Q�����Ă���S�Ă�PlayerState���Q��
		for (ANetworkPlayerState* pPlayerState : pGameState->GetPlayerState())
		{
			if (pPlayerState && GetPlayerState<ANetworkPlayerState>() != pPlayerState)
			{
				//if (pPlayerState->IsCanUpdate())
				{
					if (APlayerDrone* pPlayerDrone = pPlayerState->GetPawn<APlayerDrone>())
					{
						FReplicatedPlayerTransform ReplicatedPlayer = pPlayerState->GetPlayerTransform();

						if (ReplicatedPlayer.ControllerID > 0)
						{
							//pPlayerDrone->SetBodyMeshRotation(ReplicatedPlayer.Transform.GetRotation());
							pPlayerDrone->SetActorLocation(ReplicatedPlayer.Transform.GetLocation());

#if WITH_EDITOR
							FString str = TEXT("ClientID::") + FString::FromInt(ReplicatedPlayer.ControllerID);
							UKismetSystemLibrary::PrintString(this, str, true, false);
							//UKismetSystemLibrary::PrintString(this, ReplicatedPlayer.Transform.ToString(), true, false);
#endif // WITH_EDITOR
						}
					}
				}
#if WITH_EDITOR
				//else
				//{
				//	FString DebugStr = pPlayerState->GetName() + TEXT("::Can't Update");
				//	UKismetSystemLibrary::PrintString(this, DebugStr, true, false);
				//}
#endif // WITH_EDITOR
			}
#if WITH_EDITOR
			else
			{
				UKismetSystemLibrary::PrintString(this, TEXT("Server_ApplyTransform_Implementation::Is Server"), true, false);
			}
#endif // WITH_EDITOR
		}
	}
#if WITH_EDITOR
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("GameState not found."), true, false);
	}
#endif // WITH_EDITOR
}

//�N���C�A���g�p�@Transform��n��
void ANetworkPlayerController::Client_TransformTransfer(const FReplicatedPlayerTransform& ReplicatedData)
{
	//GameState�擾
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//���g��PlayerState���擾
		if (ANetworkPlayerState* pPlayerState = GetPlayerState<ANetworkPlayerState>())
		{
			//���g�̑��삷��v���C���[��Transform��PlayerState�ɓ]������
			if (APlayerDrone* pPlayerDrone = GetPawn<APlayerDrone>())
			{
				pPlayerState->SetPlayerTransform(ReplicatedData);

#if WITH_EDITOR
				FString str = TEXT("Client_TransformTransfer_Implementation::") + GetName();
				UKismetSystemLibrary::PrintString(this, str, true, false);
				UKismetSystemLibrary::PrintString(this, ReplicatedData.Transform.ToString(), true, false);
#endif // WITH_EDITOR
			}
		}
#if WITH_EDITOR
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("PlayerState not found."), true, false);
		}
#endif // WITH_EDITOR
	}
#if WITH_EDITOR
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("GameState not found."), true, false);
	}
#endif // WITH_EDITOR
}

//�T�[�o�[�p�@�ŐV�̃h���[�����ɍX�V
void ANetworkPlayerController::Server_UpdateDrone() 
{
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//�Q�����Ă���S�Ă�PlayerState���Q��
		for (ANetworkPlayerState* pPlayerState : pGameState->GetPlayerState())
		{
			if (pPlayerState && GetPlayerState<ANetworkPlayerState>() != pPlayerState)
			{
				//�N���C�A���g����󂯎�����h���[�������擾
				if (APlayerDrone* pClientDrone = pPlayerState->GetReplicatedPlayer())
				{
					//�T�[�o�[��ɑ��݂���h���[���̍��W���X�V����
					if (APlayerDrone* pServerDrone = pPlayerState->GetPawn<APlayerDrone>())
					{
						pServerDrone->SetActorLocation(pClientDrone->GetActorLocation());
						pServerDrone->SetBodyMeshRotation(pClientDrone->GetBodyMeshRelativeRotation());
					}
				}
			}
		}
	}
}

//�N���C�A���g�p�@�ŐV�̃h���[���f�[�^��n��
void ANetworkPlayerController::Client_UpdateDrone(APlayerDrone* ReplicatedDrone)
{
	//GameState�擾
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//���g��PlayerState���擾
		if (ANetworkPlayerState* pPlayerState = GetPlayerState<ANetworkPlayerState>())
		{
			pPlayerState->SetReplicatedPlayer(ReplicatedDrone);

#if WITH_EDITOR
			FString str = TEXT("Client_TransformTransfer_Implementation::") + GetName();
			UKismetSystemLibrary::PrintString(this, str, true, false);
			UKismetSystemLibrary::PrintString(this, ReplicatedDrone->GetName(), true, false);
#endif // WITH_EDITOR
		}
	}
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

//���v���P�[�g����f�[�^���擾
FReplicatedPlayerTransform ANetworkPlayerController::GetReplicatedData()const
{
	//���̃R���g���[���[�����삷��h���[�����擾
	if (APlayerDrone* pPlayerDrone = GetPawn<APlayerDrone>())
	{
		//�v���C���[�X�e�[�g���擾
		if (ANetworkPlayerState* pPlayerState = GetPlayerState<ANetworkPlayerState>())
		{
			FReplicatedPlayerTransform ReplicatedPlayer(
				FTransform(pPlayerDrone->GetBodyMeshRelativeRotation(), pPlayerDrone->GetActorLocation()),
				pPlayerState->GetPlayerId());

			return ReplicatedPlayer;
		}
	}

#if WITH_EDITOR
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ReplicatedData not found."), true, false);
#endif // WITH_EDITOR

	return FReplicatedPlayerTransform();
}