//------------------------------------------------------------------------
// ファイル名		:ANetworkGameMode.cpp
// 概要				:ログイン、ログアウト時の処理を行うクラス　サーバーのみに存在する。
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------


#include "NetworkGameMode.h"
#include "Engine/Engine.h"
#include "NetworkPlayerController.h"

//コンストラクタ
ANetworkGameMode::ANetworkGameMode()
{
	//プレイヤーが持つコントロール権
	Role = ROLE_Authority;
	//同期対象フラグ
	bReplicates = false;
	//所有権を持つクライアントのみに同期する
	bOnlyRelevantToOwner = false;
}

//ログイン時に呼ばれる関数
void ANetworkGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(NewPlayer))
		{
			//ログインしたプレイヤーのコントローラーを保持
			m_pPlayerControllers.Add(pPlayerController);
		}
	}
}

//ログアウト時に呼ばれる関数
void ANetworkGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

}

	//ゲームモード取得
ANetworkGameMode* ANetworkGameMode::Get()
{
	ANetworkGameMode* pGameMode = nullptr;
	if (GEngine)
	{
		if (UWorld* pWorld = GEngine->GetWorld())
		{
			pGameMode = pWorld->GetAuthGameMode<ANetworkGameMode>();
		}
	}
	return pGameMode;
}