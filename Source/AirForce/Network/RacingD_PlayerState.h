// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/RacingD_Session.h"
#include "RacingD_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API ARacingD_PlayerState : public APlayerState, public IRacingD_Session
{
	GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ
     */
    ARacingD_PlayerState();

    /**
     * @brief レプリケーションする変数を登録
     * @param OutLifetimeProps 
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
     * @brief セッションのロール設定
     * @param _serverRole 
     */
    void SetServerRole(const EServerRole& _serverRole);

    /**
     * @brief セッションのロール取得
     * @return 
     */
    virtual EServerRole GetServerRole_Implementation() const override;
    
private:
    UPROPERTY(BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = true, DisplayName = "ServerRole"))
    /**
     * @brief セッション中の役職
     */
    EServerRole m_ServerRole;
};
