//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:ANetworkPlayerState.h
// 概要				:クライアントのPlayerControllerからTransformを受け取り、保持するプレイヤーステートクラス　サーバーとクライアントがそれぞれ1つずつ所有し、権限に関係なく読み取ることができる。
// 作成日			:2022/03/01
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetworkPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API ANetworkPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ANetworkPlayerState();

protected:
	//ゲーム開始時に実行
	virtual void BeginPlay()override;
	//毎フレーム実行
	virtual void Tick(float DeltaTime)override;
	//レプリケートを登録
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

public:
	//　クライアント用　トランスフォームを設定
	void SetPlayerTransform(const FTransform& NewTransform) { m_PlayerTransform = NewTransform; }
	//　サーバー用　トランスフォームを取得
	FTransform GetPlayerTransform()const { return m_PlayerTransform; }

private:
	UPROPERTY(EditAnywhere, Replicated)
		FTransform m_PlayerTransform;								//プレイヤーのTransformを格納
};
