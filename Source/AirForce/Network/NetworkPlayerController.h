/*-------------------------------------------------------------------------------------------------------------------------------------------------------
* セッション接続周りの通信を行うPlayerControllerクラス
-------------------------------------------------------------------------------------------------------------------------------------------------------*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FindSessionsCallbackProxy.h"
#include "Interface/RacingD_Session.h"
#include "NetworkPlayerController.generated.h"

class FOnlineSessionSearch;

UENUM(BlueprintType)
/**
 * @brief セッションの参加ステート
*/
enum class EJoinSessionState :uint8
{
    /**
     * @brief 参加成功
    */
    Success = 0,

    /**
     * @brief セッションに空きがない
    */
    SessionIsFull = 1,

    /**
     * @brief セッションが見つからない
    */
    SessionDoesNotExist = 2,

    /**
     * @brief セッションサーバーのアドレス取得に失敗
    */
    CouldNotRetrieveAddress = 3,

    /**
     * @brief すでに参加しているセッション
    */
    AlreadyInSession = 4,

    /**
     * @brief 不明なエラー
    */
    UnknownError = 5,
};

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

/**
 * @brief セッション参加完了時のデリゲート
 * @param  
 * @param  
*/
DECLARE_DYNAMIC_DELEGATE_TwoParams(FJoinSessionCompleted, const FName, sessionName, EJoinSessionState, _joinState);

/**
 * @brief セッション破棄完了時のデリゲート
 * @param  sessionName 破棄したセッション名
 * @param  success セッション破棄に成功したか
*/
DECLARE_DYNAMIC_DELEGATE_TwoParams(FKillSessionCompleted, const FName, sessionName, bool, success);

UCLASS()
class AIRFORCE_API ANetworkPlayerController : public APlayerController, public IRacingD_Session
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	ANetworkPlayerController();

protected:
    /**
     * @brief 終了時の処理
     * @param EndPlayReason 
    */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

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
     * @param _createSessionCompleted セッション作成完了時のデリゲート
     * @return 
    */
    bool CreateSession(const int32 _connection, const FString _searchKeyword, const FCreateSessionCompleted& _createSessionCompleted);

    UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
    /**
     * @brief セッションの検索
     * @param _searchKeyword 検索する名前
     * @param _maxSearchResults 検索するセッションの最大数
     * @param _findSessionCompleted セッション検索完了時のデリゲート
     * @return 
    */
    bool FindSession(const FString _searchKeyword, const int32 _maxSearchResults, const FFindSessionCompleted& _findSessionCompleted);

    UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
    /**
     * @brief セッションに参加
     * @param _searchResult 参加するセッション
     * @param _joinSessionCompleted セッション参加完了時のデリゲート
    */
    bool JoinSession(const FBlueprintSessionResult& _searchResult, const FJoinSessionCompleted& _joinSessionCompleted);

    UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
    /**
     * @brief セッションの破棄
     * @param _killSessionCompleted セッション破棄完了時のデリゲート
     * @return 
    */
    bool KillSession(const FKillSessionCompleted& _killSessionCompleted);

    /**
     * @brief セッションのロール取得
     * @return 
     */
    EServerRole GetServerRole_Implementation() const override;

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
     * @brief セッション作成完了時の内部処理
     * @param _sessionName 
     * @param _bWasSuccessful 
    */
    virtual void OnCreateSessionCompleted_Internal(FName _sessionName, bool _bWasSuccessful);

    /**
     * @brief セッション検索完了時の内部処理
     * @param _bWasSuccessful 
    */
    virtual void OnFindSessionsCompleted_Internal(bool _bWasSuccessful);

    virtual void OnJoinSessionCompleted_Internal(FName _sessionName, EOnJoinSessionCompleteResult::Type _joinState);
    /**
     * @brief セッション破棄完了時の内部処理
     * @param _sessionName 
     * @param _bWasSuccessful 
    */
    virtual void OnKillSessionCompleted_Internal(FName _sessionName, bool _bWasSuccessful);

private:
    /**
     * @brief セッション検索の結果を保持する用
    */
    TSharedPtr<FOnlineSessionSearch> m_pSearchSettings;

    /**
     * @brief セッション名
    */
    static const FName SESSION_NAME;

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

    UPROPERTY()
    /**
     * @brief セッション参加完了時のデリゲート
    */
    FJoinSessionCompleted OnJoinSessionCompleted;

    UPROPERTY()
    /**
     * @brief セッション破棄完了時のデリゲート
    */
    FKillSessionCompleted OnKillSessionCompleted;
};
