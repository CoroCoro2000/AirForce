// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RacingD_Session.generated.h"

UENUM(BlueprintType)
/**
 * @brief 参加セッションの役職
 */
enum class EServerRole : uint8
{
    /**
     * @brief 未設定
     */
    NONE = 0        UMETA(Hidden),

    /**
     * @brief クライアント
     */
    CLIENT = 1      UMETA(DisplayName = "Client"),

    /**
     * @brief サーバー
     */
    SERVER = 2      UMETA(DisplayName = "Server")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class URacingD_Session : public UInterface
{
	GENERATED_BODY()
    
};

/**
 * 
 */
class AIRFORCE_API IRacingD_Session
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Racing-D|Network|Interface")
    /**
     * @brief セッションの役職を取得
     * @return 
     */
    EServerRole GetServerRole() const;
    virtual EServerRole GetServerRole_Implementation() const;
};
