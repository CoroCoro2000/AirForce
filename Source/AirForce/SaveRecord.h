// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveRecord.generated.h"

//1�ʂ�Transform�����i�[����\����
USTRUCT(BlueprintType)
struct FSaveBestTimeTransform
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	FSaveBestTimeTransform()
		: BestRecordLocation()
		, BestRecordRotation()
	{}
	FSaveBestTimeTransform(const TArray<FVector> NewRecordLocations, const TArray<FQuat> NewRecordRotations)
		: BestRecordLocation(NewRecordLocations)
		, BestRecordRotation(NewRecordRotations)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FVector> BestRecordLocation;									//1�ʂ̍��W��ۑ�����z��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FQuat> BestRecordRotation;									//1�ʂ̉�]��ۑ�����z��
};

//��ʂ̃^�C�����i�[����\����
USTRUCT(BlueprintType)
struct FSaveTopRankingTime
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	FSaveTopRankingTime()
		: RecordTimes()
	{}

	//�w�菇�ʂ̃��R�[�h���擾
	float GetRankRecord(const int& rank)const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<float> RecordTimes;									//���Ԃ��L�^����z��
	//�����L���O���
	static const int32 RankingMax;
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

	//1�ʂ�Transform��ݒ�
	void SetBestTimeTransform(const FName CourseName, const TArray<FVector>& RecordLocations, const TArray<FQuat>& RecordRotations);
	//�����L���O���肵�����R�[�h��ݒ�
	void SetRecordTime(const FName CourseName, const float& RecordTime);
	//1�ʂ�Transform���擾
	UFUNCTION(BlueprintCallable)
		FSaveBestTimeTransform GetBestTimeTransform(const FName CourseName)const;
	//��ʂ̃����L���O�擾
	UFUNCTION(BlueprintCallable)
		FSaveTopRankingTime GetTopRankingTime(const FName CourseName)const;

private:
	UPROPERTY(VisibleAnywhere)
		TMap<FName, FSaveBestTimeTransform> m_BestTimeTransform;						//1�ʂ�Transform���i�[����\���́A�R�[�X���ŊeTransform�����o����
	UPROPERTY(VisibleAnywhere)
		TMap <FName, FSaveTopRankingTime> m_TopRankingTimes;												//1�ʂ̉�]��ۑ�����z��A�R�[�X���Ń����L���O�����o����
};
