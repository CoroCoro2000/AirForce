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

public:
	//指定されたタグと一致するアクターすべてを取得する関数
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
		static void GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors);

	//ゲームデータのロード
	UFUNCTION(BlueprintCallable)
		static void LoadGameData(const FString& SlotName, const int SlotIndex);
	//ゲームデータの非同期ロード
	UFUNCTION(BlueprintCallable)
		static void AsyncLoadGameData(const FString& SlotName, const int SlotIndex);
	//ゲームデータのセーブ
	UFUNCTION(BlueprintCallable)
		static void SaveGameData(const FString& SlotName, const int SlotIndex);
	//ゲームデータの非同期セーブ
	UFUNCTION(BlueprintCallable)
		static void AsyncSaveGameData(const FString& SlotName, const int SlotIndex);
};
