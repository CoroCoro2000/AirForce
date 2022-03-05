//------------------------------------------------------------------------
// ファイル名		:ANetworkGameState.cpp
// 概要				:サーバー　⇔　クライアント間のやり取りを管理する　サーバーとクライアント全体で1つを共有する。
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#include "NetworkGameState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkPlayerState.h"
#include "PlayerDrone.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR

//コンストラクタ
ANetworkGameState::ANetworkGameState()
{
	//同期対象フラグ
	bReplicates = true;
	//所有権を持つクライアントのみに同期する
	bOnlyRelevantToOwner = false;
}

//ゲーム開始時に実行される関数
void ANetworkGameState::BeginPlay()
{
	Super::BeginPlay();

}

//レプリケートを登録
void ANetworkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkGameState, m_pPlayerDrones);
}

//毎フレーム実行される関数
void ANetworkGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//ゲームステート取得
ANetworkGameState* ANetworkGameState::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		if (UWorld* pWorld = WorldContextObject->GetWorld())
		{
			pWorld->GetGameState<ANetworkGameState>();
		}
	}

	return nullptr;
}

//PlayerStateの配列を取得
TArray<ANetworkPlayerState*> ANetworkGameState::GetPlayerState()const
{
	TArray<ANetworkPlayerState*> pNetworkPlayerArray;

	for (APlayerState* pPlayerState : PlayerArray)
	{
		pNetworkPlayerArray.Add(Cast<ANetworkPlayerState>(pPlayerState));
	}

	return pNetworkPlayerArray;
}

//プレイヤーの登録
void ANetworkGameState::EntryPlayer(APlayerDrone* pEntryDrone)
{
	for (APlayerDrone* pEntryedDrone : m_pPlayerDrones)
	{
		if (pEntryDrone == pEntryedDrone)
		{
#if WITH_EDITOR
			FString str = TEXT("Entryed::") + pEntryDrone->GetName();
			UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR

			break;
		}
	}

#if WITH_EDITOR
	FString str = TEXT("Entry::") + pEntryDrone->GetName();
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR

	m_pPlayerDrones.Add(pEntryDrone);
}