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

	//サーバー用　Transform適用
	UFUNCTION(BlueprintCallable)
		void Server_ApplyTransform(const FReplicatedPlayerTransform& ReplicatedPlayer);
	//クライアント用　Transformを渡す
	UFUNCTION(BlueprintCallable)
		FReplicatedPlayerTransform Client_TransformTransfer();

	//プレイヤーのTransform同期
	void SynchronizePlayerTransform();
	//プレイヤー情報の更新
	UFUNCTION()
		void ActivateUpdate();

public:
	//ゲームコントローラー取得
	static ANetworkPlayerController* Get();

private:
	UPROPERTY(EditAnywhere)
		float m_SynchronousInterval;			//同期する間隔(秒単位)
	UPROPERTY(EditAnywhere, Replicated)
		bool m_bCanUpdate;						//更新可能フラグ
};
