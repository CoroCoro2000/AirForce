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
	bAlwaysRelevant = true;
	bReplicateMovement = true;
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
void ANetworkPlayerController::Server_ApplyTransform(const FReplicatedPlayerTransform& ReplicatedPlayer)
{
	if (!IsLocalPlayerController())
	{
		if (APlayerDrone* pPlayerDrone = Cast<APlayerDrone>(UGameplayStatics::GetPlayerPawn(GetWorld(), ReplicatedPlayer.ControllerID)))
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

//�N���C�A���g�p�@Transform��n��
FReplicatedPlayerTransform ANetworkPlayerController::Client_TransformTransfer()
{
	if (IsLocalPlayerController())
	{
		if (APlayerDrone* pPlayerDrone = GetPawn<APlayerDrone>())
		{
			FReplicatedPlayerTransform ReplicatedPlayer(
				FTransform(pPlayerDrone->GetBodyMeshRelativeRotation(), pPlayerDrone->GetActorLocation()),
				UGameplayStatics::GetPlayerControllerID(this));

#if WITH_EDITOR
			FString str = TEXT("Client_TransformTransfer_Implementation::") + pPlayerDrone->GetName();
			UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR

			return ReplicatedPlayer;
		}
	}

	return FReplicatedPlayerTransform(FTransform::Identity, -1);
}

//�v���C���[��Transform����
void ANetworkPlayerController::SynchronizePlayerTransform()
{
	if (!m_bCanUpdate) { return; }

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