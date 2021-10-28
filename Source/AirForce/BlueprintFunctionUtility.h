// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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

	//指定されたタグと一致するアクターすべてを取得する関数
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
		static void GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors);
public:
	//ターゲットに向かった回転を返す関数
	UFUNCTION(BlueprintCallable)
		static FQuat RInterpToQuaternion(const FRotator Current, const FRotator Target, const float DeltaTime, const float InterpSpeed);
};
