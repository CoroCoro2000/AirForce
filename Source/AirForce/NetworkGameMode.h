//------------------------------------------------------------------------
// ファイル名		:ANetworkGameMode.h
// 概要				:ログイン、ログアウト時の処理を行うクラス　サーバーのみに存在する。
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NetworkGameMode.generated.h"

class ANetworkPlayerController;

/**
 * 
 */
UCLASS()
class AIRFORCE_API ANetworkGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ANetworkGameMode();

protected:
	//ログイン時に呼ばれる関数
	virtual void PostLogin(APlayerController* NewPlayer)override;
	//ログアウト時に呼ばれる関数
	virtual void Logout(AController* Exiting)override;

public:
	//ゲームモード取得
	static ANetworkGameMode* Get();

private:
	UPROPERTY(EditAnywhere)
		TArray<ANetworkPlayerController*> m_pPlayerControllers;				//セッションに参加中のプレイヤーのコントローラー
};
