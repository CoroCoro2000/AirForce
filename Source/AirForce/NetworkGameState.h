//------------------------------------------------------------------------
// ファイル名		:ANetworkGameState.h
// 概要				:サーバー　⇔　クライアント間のやり取りを管理するゲームステート
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
	//レプリケートするプロパティを更新する
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//ローカルで操作しているプレイヤーを取得
	UFUNCTION(BlueprintCallable)
		APlayerDrone* GetLocalPlayerDrone()const;

	UFUNCTION()
		void OnRep_CurrentDrone();

protected:
	UPROPERTY(EditAnywhere)
		uint8 m_PlayerIndex;													//ローカルプレイヤーの識別番号
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = OnRep_CurrentDrone)
		TArray<APlayerDrone*> m_pPlayerDrones;					//セッションに参加しているプレイヤーの座標
};
