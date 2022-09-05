#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FindSessionsCallbackProxy.h"
#include "NetworkPlayerController.generated.h"

class FOnlineSessionSearch;

/**
 * @brief ログイン完了時のデリゲート
 * @param  success ログインに成功したか
*/
DECLARE_DYNAMIC_DELEGATE_OneParam(FLoginCompleted, bool, success);

/**
 * @brief セッション作成完了時のデリゲート
 * @param  sessionName セッション名
 * @param  success セッション作成に成功したか
*/
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCreateSessionCompleted, const FName, sessionName, bool, success);

/**
 * @brief セッション検索完了時のデリゲート
 * @param  results 検索結果のリスト
 * @param  success セッション検索に成功したか
*/
DECLARE_DYNAMIC_DELEGATE_TwoParams(FFindSessionCompleted, const TArray<FBlueprintSessionResult>&, results, bool, success);

UCLASS()
class AIRFORCE_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	ANetworkPlayerController();

public:
	UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
	/**
	 * @brief EOSにログイン
	 * @param _loginCompleted ログイン完了時のデリゲート
	*/
	void LoginEOS(const FLoginCompleted& _loginCompleted);

    UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
    /**
     * @brief セッションを作成
     * @param _connection 接続人数
     * @param _searchKeyword セッションの検索に使用する名前
     * @param _sessionName セッションの名前
     * @param _createSessionCompleted セッション作成完了時のデリゲート
     * @return 
    */
    bool CreateSession(const int32 _connection, const FString _searchKeyword, const FName _sessionName, const FCreateSessionCompleted& _createSessionCompleted);

    UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
    /**
     * @brief セッションの検索
     * @param _searchKeyword 検索する名前
     * @param _maxSearchResults 検索するセッションの最大数
     * @param _findSessionCompleted セッション検索完了時のデリゲート
     * @return 
    */
    bool FindSession(const FString _searchKeyword, const int32 _maxSearchResults, const FFindSessionCompleted& _findSessionCompleted);

private:
	/**
	 * @brief ログイン後の内部処理
	 * @param _localUserNum 
	 * @param _bWasSuccessful 
	 * @param _userId 
	 * @param _error 
	*/
	virtual void OnLoginCompleted_Internal(int32 _localUserNum, bool _bWasSuccessful, const FUniqueNetId& _userId, const FString& _error);

    /**
     * @brief セッション作成完了時のデリゲート
     * @param _sessionName 
     * @param _bWasSuccessful 
    */
    virtual void OnCreateSessionCompleted_Internal(FName _sessionName, bool _bWasSuccessful);

    /**
     * @brief セッション検索完了時のデリゲート
     * @param _bWasSuccessful 
    */
    virtual void OnFindSessionsCompleted_Internal(bool _bWasSuccessful);

private:
    /**
     * @brief セッション検索の結果を保持する用
    */
    TSharedPtr<FOnlineSessionSearch> m_pSearchSettings;

	UPROPERTY()
	/**
	 * @brief ログイン成功時のデリゲート
	*/
	FLoginCompleted OnLoginCompleted;

    UPROPERTY()
    /**
     * @brief セッション作成完了時のデリゲート
    */
    FCreateSessionCompleted OnCreateSessionCompleted;

    UPROPERTY()
    /**
     * @brief セッション検索完了時のデリゲート
    */
    FFindSessionCompleted OnFindSessionCompleted;
};
