//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:ANetworkPlayerState.cpp
// 概要				:クライアントのPlayerControllerからTransformを受け取り、保持するプレイヤーステートクラス　サーバーとクライアントがそれぞれ1つずつ所有し、権限に関係なく読み取ることができる。
// 作成日			:2022/03/01
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "NetworkPlayerState.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

//コンストラクタ
ANetworkPlayerState::ANetworkPlayerState()
	: m_PlayerTransform(FTransform::Identity)
{
	//プレイヤーが持つコントロール権
	Role = ROLE_Authority;
	//同期対象フラグ
	bReplicates = true;
	//所有権を持つクライアントのみに同期する
	bOnlyRelevantToOwner = false;
}

	//ゲーム開始時に実行
void ANetworkPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

//毎フレーム実行
void ANetworkPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//レプリケートを登録
void ANetworkPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkPlayerState, m_PlayerTransform);
}