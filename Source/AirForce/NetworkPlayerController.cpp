//------------------------------------------------------------------------
// ファイル名		:ANetworkPlayerController.cpp
// 概要				:クライアントのPawnとHUDの操作を行うコントローラー
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------


#include "NetworkPlayerController.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerDrone.h"
#include "NetworkGameState.h"
#include "NetworkPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"

//コンストラクタ
ANetworkPlayerController::ANetworkPlayerController()
	: m_SynchronousInterval(0.25f)
	, m_bCanUpdate(true)
{
	//プレイヤーが持つコントロール権
	Role = ROLE_Authority;
	//同期対象フラグ
	bReplicates = true;
	//所有権を持つクライアントのみに同期する
	bOnlyRelevantToOwner = true;
	bAlwaysRelevant = true;
	bReplicateMovement = true;
}

//ゲーム開始時に実行される関数
void ANetworkPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

//毎フレーム実行される関数
void ANetworkPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//レプリケートを登録
void ANetworkPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkPlayerController, m_bCanUpdate);
}

//サーバー用　Transform適用
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

//クライアント用　Transformを渡す
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

//プレイヤーのTransform同期
void ANetworkPlayerController::SynchronizePlayerTransform()
{
	if (!m_bCanUpdate) { return; }

	m_bCanUpdate = false;

	//指定秒後に更新可能フラグを立てる関数を呼び出すように登録
	FLatentActionInfo LatentAction(0, 0, TEXT("ActivateUpdate"), this);
	UKismetSystemLibrary::Delay(this, m_SynchronousInterval, LatentAction);
}

//プレイヤー情報の更新
void ANetworkPlayerController::ActivateUpdate()
{
	m_bCanUpdate = true;

#if WITH_EDITOR
	FString str = GetName() + TEXT("::ActivateUpdate()");
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
}

//ゲームコントローラー取得
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