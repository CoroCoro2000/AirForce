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
{
	//同期対象フラグ
	bReplicates = true;
	//所有権を持つクライアントのみに同期する
	bOnlyRelevantToOwner = true;
	bAlwaysRelevant = true;
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

}

//サーバー用　Transform適用
void ANetworkPlayerController::Server_ApplyTransform()
{
	//GameState取得
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//参加している全てのPlayerStateを参照
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

//クライアント用　Transformを渡す
void ANetworkPlayerController::Client_TransformTransfer(const FReplicatedPlayerTransform& ReplicatedData)
{
	//GameState取得
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//自身のPlayerStateを取得
		if (ANetworkPlayerState* pPlayerState = GetPlayerState<ANetworkPlayerState>())
		{
			//自身の操作するプレイヤーのTransformをPlayerStateに転送する
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

//サーバー用　最新のドローン情報に更新
void ANetworkPlayerController::Server_UpdateDrone() 
{
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//参加している全てのPlayerStateを参照
		for (ANetworkPlayerState* pPlayerState : pGameState->GetPlayerState())
		{
			if (pPlayerState && GetPlayerState<ANetworkPlayerState>() != pPlayerState)
			{
				//クライアントから受け取ったドローン情報を取得
				if (APlayerDrone* pClientDrone = pPlayerState->GetReplicatedPlayer())
				{
					//サーバー上に存在するドローンの座標を更新する
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

//クライアント用　最新のドローンデータを渡す
void ANetworkPlayerController::Client_UpdateDrone(APlayerDrone* ReplicatedDrone)
{
	//GameState取得
	if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
	{
		//自身のPlayerStateを取得
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

//レプリケートするデータを取得
FReplicatedPlayerTransform ANetworkPlayerController::GetReplicatedData()const
{
	//このコントローラーが操作するドローンを取得
	if (APlayerDrone* pPlayerDrone = GetPawn<APlayerDrone>())
	{
		//プレイヤーステートを取得
		if (ANetworkPlayerState* pPlayerState = GetPlayerState<ANetworkPlayerState>())
		{
			FReplicatedPlayerTransform ReplicatedPlayer(
				FTransform(pPlayerDrone->GetBodyMeshRelativeRotation(), pPlayerDrone->GetActorLocation()),
				pPlayerState->PlayerId);

			return ReplicatedPlayer;
		}
	}

#if WITH_EDITOR
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("ReplicatedData not found."), true, false);
#endif // WITH_EDITOR

	return FReplicatedPlayerTransform();
}