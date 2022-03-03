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
	if (IsLocalPlayerController())
	{
		if (ANetworkGameState* pGameState = GetWorld()->GetGameState<ANetworkGameState>())
		{
			for (ANetworkPlayerState* pPlayerState : pGameState->GetPlayerState())
			{
				if (pPlayerState)
				{
					//クライアントから送られてきたTransformでサーバー上にいるドローンを更新する
					FReplicatedPlayerTransform ReplicatedPlayer = pPlayerState->GetPlayerTransform();

					if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(pPlayerState->GetOwner()))
					{
						if (APlayerDrone* pPlayerDrone = pPlayerController->GetPawn<APlayerDrone>())
						{
							//ローカルでコントロールされていない(クライアントの)ドローンのみ更新
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

//クライアント用　Transformを渡す
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

//プレイヤーのTransform同期
void ANetworkPlayerController::SynchronizePlayerTransform()
{

}

//プレイヤー情報の更新
void ANetworkPlayerController::ActivateUpdate()
{

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