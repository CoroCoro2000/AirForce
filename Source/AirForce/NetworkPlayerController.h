#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

/**
 * @brief ログイン完了時のデリゲート
 * @param  Success ログインに成功したか
*/
DECLARE_DYNAMIC_DELEGATE_OneParam(FLoginCompleted, bool, Success);

UCLASS()
class AIRFORCE_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	ANetworkPlayerController();

public:
	UFUNCTION(BlueprintPure, Category = "Racing-D|Network", meta = (WorldContext = WorldContextObject))
	/**
	 * @brief プレイヤーコントローラー取得
	 * @return 
	*/
	static ANetworkPlayerController* GetNetworkPlayerController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
	/**
	 * @brief EOSにログイン
	 * @param _loginCompleted ログイン後のデリゲート
	*/
	void LoginEOS(const FLoginCompleted& _loginCompleted);

private:
	/**
	 * @brief ログイン後の内部処理
	 * @param _localUserNum 
	 * @param _bWasSuccessful 
	 * @param _userId 
	 * @param _error 
	*/
	void OnLoginCompleted_Internal(int32 _localUserNum, bool _bWasSuccessful, const FUniqueNetId& _userId, const FString& _error);

private:
	UPROPERTY()
	/**
	 * @brief ログイン成功時のデリゲート
	*/
	FLoginCompleted OnLoginCompleted;
};
