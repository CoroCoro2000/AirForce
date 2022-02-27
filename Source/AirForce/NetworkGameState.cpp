//------------------------------------------------------------------------
// ファイル名		:ANetworkGameState.h
// 概要				:サーバー　⇔　クライアント間のやり取りを管理するゲームステート
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#include "NetworkGameState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "PlayerDrone.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR


//コンストラクタ
ANetworkGameState::ANetworkGameState()
	: m_PlayerIndex(0)
{

	//レプリケートを有効化
	bReplicates = true;
	//レプリケートを行う間隔(秒単位)を設定
	NetUpdateFrequency = 10;
	MinNetUpdateFrequency = 5;
}

//ゲーム開始時に実行される関数
void ANetworkGameState::BeginPlay()
{
	Super::BeginPlay();

}

//毎フレーム実行される関数
void ANetworkGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//レプリケートするプロパティを更新する
void ANetworkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkGameState, m_pPlayerDrones);
}

//ローカルで操作しているプレイヤーを取得
APlayerDrone* ANetworkGameState::GetLocalPlayerDrone()const
{
	for (APlayerState* pPlayerState : PlayerArray)
	{
		if (pPlayerState)
		{
			if (APawn* pPawn = pPlayerState->GetPawn())
			{
				if (pPawn->IsLocallyControlled())
				{
#if WITH_EDITOR
					UKismetSystemLibrary::PrintString(GetWorld(), pPawn->GetName(), true, false);
#endif // WITH_EDITOR

					return Cast<APlayerDrone>(pPawn);
				}
			}
		}
	}

	return nullptr;
}

void ANetworkGameState::OnRep_CurrentDrone()
{
	if (m_pPlayerDrones.IsValidIndex(m_PlayerIndex))
	{
		m_pPlayerDrones[m_PlayerIndex] = GetLocalPlayerDrone();
	}
}