//------------------------------------------------------------------------
// ファイル名		:ANetworkGameState.h
// 概要				:サーバー　⇔　クライアント間のやり取りを管理する　サーバーとクライアント全体で1つを共有する。
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NetworkGameState.generated.h"

class APlayerDrone;

/**
 * 
 */
UCLASS()
class AIRFORCE_API ANetworkGameState : public AGameState
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ANetworkGameState();

protected:
	//ゲーム開始時に実行される関数
	virtual void BeginPlay()override;
	//毎フレーム実行される関数
	virtual void Tick(float DeltaTime)override;

public:
	//ゲームステート取得
	static ANetworkGameState* Get();


private:
	UPROPERTY(EditAnywhere)
		TArray<APlayerDrone*> m_pPlayerDrones;
};
