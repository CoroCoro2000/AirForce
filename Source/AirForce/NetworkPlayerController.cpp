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
	if (IsLocalPlayerController())
	{
		if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
		{
			for (ANetworkPlayerState* pPlayerState : pGameState->GetPlayerState())
			{
				if (pPlayerState)
				{
					//�N���C�A���g���瑗���Ă���Transform�ŃT�[�o�[��ɂ���h���[�����X�V����
					FReplicatedPlayerTransform ReplicatedPlayer = pPlayerState->GetPlayerTransform();

					if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(pPlayerState->GetOwner()))
					{
						if (APlayerDrone* pPlayerDrone = pPlayerController->GetPawn<APlayerDrone>())
						{
							//���[�J���ŃR���g���[������Ă��Ȃ�(�N���C�A���g��)�h���[���̂ݍX�V
							if (!pPlayerDrone->IsLocallyControlled())
							{
								pPlayerDrone->SetActorLocation(ReplicatedPlayer.Transform.GetLocation());
								pPlayerDrone->SetBodyMeshRotation(ReplicatedPlayer.Transform.GetRotation());

#if WITH_EDITOR
								FString str = TEXT("Server_ApplyTransform_Implementation::") + pPlayerDrone->GetName();
								UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
							}
						}
					}
				}
			}
		}
	}
}

//�N���C�A���g�p�@Transform��n��
void ANetworkPlayerController::Client_TransformTransfer()
{
	if (IsLocalPlayerController())
	{
		if (APlayerDrone* pPlayerDrone = GetPawn<APlayerDrone>())
		{
			if (ANetworkPlayerState* pPlayerState = GetPlayerState<ANetworkPlayerState>())
			{
				pPlayerState->SetPlayerTransform(FReplicatedPlayerTransform(
					FTransform(pPlayerDrone->GetBodyMeshRelativeRotation(), pPlayerDrone->GetActorLocation()),
					pPlayerState->PlayerId));

#if WITH_EDITOR
				FString str = TEXT("Client_TransformTransfer_Implementation::") + pPlayerDrone->GetName();
				UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
			}
		}
	}
}

//�v���C���[��Transform����
void ANetworkPlayerController::SynchronizePlayerTransform()
{

}

//�v���C���[���̍X�V
void ANetworkPlayerController::ActivateUpdate()
{

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