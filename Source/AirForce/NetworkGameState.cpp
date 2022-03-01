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
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerDrone.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR

//コンストラクタ
ANetworkGameState::ANetworkGameState()
{
	//プレイヤーが持つコントロール権
	Role = ROLE_Authority;
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

//毎フレーム実行される関数
void ANetworkGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//ゲームステート取得
ANetworkGameState* ANetworkGameState::Get()
{
	ANetworkGameState* pGameState = nullptr;
	if (GEngine)
	{
		if (UWorld* pWorld = GEngine->GetWorld())
		{
			pGameState = pWorld->GetGameState<ANetworkGameState>();
		}
	}
	return pGameState;
}