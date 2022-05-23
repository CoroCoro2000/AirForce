// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveRecord.h"
#include "BlueprintFunctionUtility.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API UBlueprintFunctionUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//コンストラクタ
	UBlueprintFunctionUtility(const FObjectInitializer& ObjectInitializer);

public:
	//指定されたタグと一致するアクターすべてを取得する関数
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
		static void GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors);

	/** Converts a RecordTime to localized formatted text, in the form '00:00.000' */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToText (RecordTime)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|Text")
		static FText Conv_RecordTimeToText(const FRecordTime& InRecordTime);
};
