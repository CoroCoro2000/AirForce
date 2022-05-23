// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveRecord.generated.h"

USTRUCT(BlueprintType)
/**
 * @brief ���R�[�h�L�^�p�\����
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
	 * @param recordTime ���R�[�h
	*/
	FRecordTime(const FRecordTime& recordTime);

	/**
	 * @brief 
	 * @param ticks �o�ߎ��ԁ@�b�P��
	*/
	FRecordTime(const float& ticks);

	/**
	 * @brief ���R�[�h�̎��ԍX�V
	 * @param ticks 
	*/
	void UpdateTime(const float& deltaTime);

	/**
	 * @brief Text �ϊ�
	 * @return 
	*/
	FText ToText()const;

	/**
	 * @brief String �ϊ�
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
	 * @brief ������
	*/
	void Initialize();

public:
	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief ��
	*/
	int32 Minutes;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief �b
	*/
	int32 Second;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief �~���b
	*/
	int32 Millisecond;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	/**
	 * @brief 0���琔�������݂̎��ԁ@�~���b�P��
	*/
	float Ticks;

	/**
	 * @brief �L�^�ł���Ticks�̏��
	*/
	static const float RECORDTICKS_MAX;

	/**
	 * @brief �L�^�ł��鎞�Ԃ̏��
	*/
	static const FRecordTime RECORDTIME_MAX;

private:
	/**
	 * @brief ���̃e�L�X�g�t�H�[�}�b�g
	*/
	FNumberFormattingOptions MinutesTextFormat;

	/**
	 * @brief �b�̃e�L�X�g�t�H�[�}�b�g
	*/
	FNumberFormattingOptions SecondTextFormat;

	/**
	 * @brief �~���b�̃e�L�X�g�t�H�[�}�b�g
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
	//�R���X�g���N�^
	USaveRecord(const FObjectInitializer& ObjectInitializer);


private:
};
