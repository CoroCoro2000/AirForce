/*
* マルチ対戦時のサーバーで制御する処理をまとめたゲームモードクラス
*
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RacingD_GameMode.generated.h"

class ANetworkPlayerController;

/**
 * 
 */
UCLASS()
class AIRFORCE_API ARacingD_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    /**
     * @brief コンストラクタ
    */
    ARacingD_GameMode();
    
    UFUNCTION(BlueprintCallable, Category="Racing-D|Network")
    /**
     * @brief 参加中のプレイヤーをキック
     * @param _kickedPlayer キックするプレイヤー
     * @param _kickReason キックされたプレイヤーに表示するキックの理由
     * @return 
     */
    bool KickPlayer(APlayerController* _kickedPlayer,const FText& _kickReason);

    UFUNCTION(BlueprintCallable, Category="Racing-D|Network")
    /**
     * @brief レベル遷移(サーバー権限のみ)
     * @param _levelName 遷移先レベル名
     * @param _bAbsolute レベル遷移に絶対移動を使用するか?
     * @param _bShouldSkipGameNotify クライアントにレベル遷移を通知するか?
     * @param _option レベル移動時のオプション
     * @return 
     */
    bool ServerTravel(const FString& _levelName, const bool _bAbsolute, const bool _bShouldSkipGameNotify, const FString _option);

protected:
    /**
     * @brief AGameModeからオーバーライド セッションにログインした時の処理
     * @param NewPlayer 
     */
    virtual void PostLogin(APlayerController* NewPlayer) override;

    /**
     * @brief SeamlessTravelでレベル遷移した時の処理
     * @param OldPC 
     * @param NewPC 
     */
    virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;
    
    /**
     * @brief AGameModeからオーバーライド セッションを離脱した時の処理
     * @param Exiting 
     */
    virtual void Logout(AController* Exiting) override;
    
private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess=true, DisplayName="PlayerControllers"))
    /**
     * @brief セッションに参加しているプレイヤーのコントローラー
     */
    TArray<ANetworkPlayerController*> m_pPlayerControllers;
};
