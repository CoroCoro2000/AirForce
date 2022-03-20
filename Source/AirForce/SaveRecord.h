// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveRecord.generated.h"

//1位のTransform情報を格納する構造体
USTRUCT(BlueprintType)
struct FSaveBestTimeTransform
{
	GENERATED_BODY()

public:
	//コンストラクタ
	FSaveBestTimeTransform()
		: BestRecordLocation()
		, BestRecordRotation()
	{}
	FSaveBestTimeTransform(const TArray<FVector> NewRecordLocations, const TArray<FQuat> NewRecordRotations)
		: BestRecordLocation(NewRecordLocations)
		, BestRecordRotation(NewRecordRotations)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FVector> BestRecordLocation;									//1位の座標を保存する配列
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FQuat> BestRecordRotation;									//1位の回転を保存する配列
};

//上位のタイムを格納する構造体
USTRUCT(BlueprintType)
struct FSaveTopRankingTime
{
	GENERATED_BODY()

public:
	//コンストラクタ
	FSaveTopRankingTime()
		: RecordTimes()
	{}

	//指定順位のレコードを取得
	float GetRankRecord(const int& rank)const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<float> RecordTimes;									//時間を記録する配列
	//ランキング上限
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
	//コンストラクタ
	USaveRecord(const FObjectInitializer& ObjectInitializer);

	//1位のTransformを設定
	void SetBestTimeTransform(const FName CourseName, const TArray<FVector>& RecordLocations, const TArray<FQuat>& RecordRotations);
	//ランキング入りしたレコードを設定
	void SetRecordTime(const FName CourseName, const float& RecordTime);
	//1位のTransformを取得
	UFUNCTION(BlueprintCallable)
		FSaveBestTimeTransform GetBestTimeTransform(const FName CourseName)const;
	//上位のランキング取得
	UFUNCTION(BlueprintCallable)
		FSaveTopRankingTime GetTopRankingTime(const FName CourseName)const;

private:
	UPROPERTY(VisibleAnywhere)
		TMap<FName, FSaveBestTimeTransform> m_BestTimeTransform;						//1位のTransformを格納する構造体、コース名で各Transformを取り出せる
	UPROPERTY(VisibleAnywhere)
		TMap <FName, FSaveTopRankingTime> m_TopRankingTimes;												//1位の回転を保存する配列、コース名でランキングを取り出せる
};
