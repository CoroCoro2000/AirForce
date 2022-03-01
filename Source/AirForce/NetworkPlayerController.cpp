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
void ANetworkPlayerController::Server_ApplyTransform_Implementation()
{
	//GameStateから各PlayerStateにアクセスし、クライアントが更新したTransformをサーバー上に反映する
	if (ANetworkGameState* pGameState = ANetworkGameState::Get())
	{
		for (APlayerState* pPlayerState : pGameState->PlayerArray)
		{
			if (ANetworkPlayerState* pNetworkPlayerState = Cast<ANetworkPlayerState>(pPlayerState))
			{
				//自身のTransformは変更してはならないため、自分以外の所有するPlayerStateにアクセスする
				if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(pNetworkPlayerState->GetOwner()))
				{
					if (pPlayerController != this)
					{
						//Transformを適用
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

//クライアント用　Transformを渡す
void ANetworkPlayerController::Client_SetTransform_Implementation()
{
	//GameStateから各PlayerStateにアクセスし、クライアントが更新したTransformをサーバー上に反映する
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
						//自分のプレイヤーステートにTransformを渡す
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

//プレイヤーのTransform同期
void ANetworkPlayerController::SynchronizePlayerTransform()
{
	if (!m_bCanUpdate) { return; }

	(GetRemoteRole() == ENetRole::ROLE_Authority) ? Server_ApplyTransform() : Client_SetTransform();

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