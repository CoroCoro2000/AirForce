// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveRecord.generated.h"

USTRUCT(BlueprintType)
/**
 * @brief レコード記録用構造体
*/
struct FRecordTime
{
	GENERATED_BODY()

public:
	/**
	 * @brief Constructor
	*/
	FRecordTime();

	/**
	 * @brief 
	 * @param recordTime レコード
	*/
	FRecordTime(const FRecordTime& recordTime);

	/**
	 * @brief 
	 * @param ticks 経過時間　秒単位
	*/
	FRecordTime(const float& ticks);

	/**
	 * @brief レコードの時間更新
	 * @param ticks 
	*/
	void UpdateTime(const float& deltaTime);

	/**
	 * @brief Text 変換
	 * @return 
	*/
	FText ToText()const;

	/**
	 * @brief String 変換
	 * @return Record
	*/
	FString ToString()const;

public:
	/**
	 * @brief operator> override
	*/
	bool operator>(const FRecordTime& Time)const;

	/**
	* @brief operator>= override
	*/
	bool operator>=(const FRecordTime& Time)const;

	/**
	 * @brief operator< override
	*/
	bool operator<(const FRecordTime& Time)const;

	/**
	 * @brief operator<= override
	*/
	bool operator<=(const FRecordTime& Time)const;

	/**
	 * @brief operator== override
	*/
	bool operator==(const FRecordTime& Time)const;

	/**
	 * @brief operator!= override
	*/
	bool operator!=(const FRecordTime& Time)const;

private:
	/**
	 * @brief 初期化
	*/
	void Initialize();

public:
	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief 分
	*/
	int32 Minutes;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief 秒
	*/
	int32 Second;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief ミリ秒
	*/
	int32 Millisecond;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief 0から数えた現在の時間　ミリ秒単位
	*/
	float Ticks;

	/**
	 * @brief 記録できるTicksの上限
	*/
	static const float RECORDTICKS_MAX;

	/**
	 * @brief 記録できる時間の上限
	*/
	static const FRecordTime RECORDTIME_MAX;

private:
	/**
	 * @brief 分のテキストフォーマット
	*/
	FNumberFormattingOptions MinutesTextFormat;

	/**
	 * @brief 秒のテキストフォーマット
	*/
	FNumberFormattingOptions SecondTextFormat;

	/**
	 * @brief ミリ秒のテキストフォーマット
	*/
	FNumberFormattingOptions MillisecondTextFormat;
};


/**
 * 
 */
UCLASS()
class AIRFORCE_API USaveRecord : public USaveGame
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	USaveRecord(const FObjectInitializer& ObjectInitializer);


private:
};
