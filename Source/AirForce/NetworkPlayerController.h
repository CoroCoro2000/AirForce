//------------------------------------------------------------------------
// ファイル名		:ANetworkPlayerController.h
// 概要				:クライアントのPawnとHUDの操作を行うコントローラー　サーバー、各クライアントに存在する
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

class APlayerDrone;
class ANetworkGameState;

//サーバーに送信するプレイヤーのTransformを格納する構造体
USTRUCT(BlueprintType)
struct FReplicatedPlayerTransform
{
	GENERATED_BODY()
public:
	FReplicatedPlayerTransform()
		: Transform(FTransform::Identity)
		, ControllerID(-1)
	{}
	FReplicatedPlayerTransform(const FTransform& InTransform, const int32& InPlayerControllerID)
		: Transform(InTransform)
		, ControllerID(InPlayerControllerID)
	{}

	UPROPERTY(EditAnywhere)
		FTransform Transform;
	UPROPERTY(EditAnywhere)
		int32 ControllerID;
};

UCLASS()
class AIRFORCE_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	ANetworkPlayerController();

protected:
	//ゲーム開始時に実行される関数
	virtual void BeginPlay()override;
	//毎フレーム実行される関数
	virtual void Tick(float DeltaTime)override;
	//レプリケートを登録
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

public:
	//ゲームコントローラー取得
	static ANetworkPlayerController* Get();

	//サーバー用　Transform適用
	UFUNCTION(BlueprintCallable)
		void Server_ApplyTransform();
	//クライアント用　Transformを渡す
	UFUNCTION(BlueprintCallable)
		void Client_TransformTransfer(const FReplicatedPlayerTransform& ReplicatedData);

	//サーバー用　最新のドローン情報に更新
	UFUNCTION(BlueprintCallable)
		void Server_UpdateDrone();
	//クライアント用　最新のドローンデータを渡す
	UFUNCTION(BlueprintCallable)
		void Client_UpdateDrone(APlayerDrone* ReplicatedDrone);

	//レプリケートするデータを取得
	UFUNCTION(BlueprintCallable)
		FReplicatedPlayerTransform GetReplicatedData()const;

	UFUNCTION(BlueprintCallable)
		float GetSynchronousInterval()const { return m_SynchronousInterval; }

private:
	UPROPERTY(EditAnywhere)
		float m_SynchronousInterval;			//同期する間隔(秒単位)
};
