//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:ANetworkPlayerState.cpp
// 概要				:クライアントのPlayerControllerからTransformを受け取り、保持するプレイヤーステートクラス　サーバーとクライアントがそれぞれ1つずつ所有し、権限に関係なく読み取ることができる。
// 作成日			:2022/03/01
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "NetworkPlayerState.h"
#include "NetworkPlayerController.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"

//コンストラクタ
ANetworkPlayerState::ANetworkPlayerState()
	: m_ReplicatedPlayer(FTransform::Identity, -1)
	, m_pReplicatedDrone(nullptr)
	, m_bCanUpdate(false)
{
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

	DOREPLIFETIME(ANetworkPlayerState, m_ReplicatedPlayer);
	DOREPLIFETIME(ANetworkPlayerState, m_pReplicatedDrone);
	DOREPLIFETIME(ANetworkPlayerState, m_bCanUpdate);
}

void ANetworkPlayerState::OnRep_m_ReplicatedPlayer()
{
	//プレイヤーの更新がされたら更新可能フラグを立てる
	m_bCanUpdate = true;

#if WITH_EDITOR
	FString str = GetName() + TEXT("::Replicated");
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
}

void ANetworkPlayerState::OnRep_m_pReplicatedDrone()
{
	//プレイヤーの更新がされたら更新可能フラグを立てる
	m_bCanUpdate = true;

#if WITH_EDITOR
	FString str = GetName() + TEXT("::Replicated");
	UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR
}