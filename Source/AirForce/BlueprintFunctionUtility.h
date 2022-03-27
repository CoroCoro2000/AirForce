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

	//�R���X�g���N�^
	UBlueprintFunctionUtility(const FObjectInitializer& ObjectInitializer);

public:
	//�w�肳�ꂽ�^�O�ƈ�v����A�N�^�[���ׂĂ��擾����֐�
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
		static void GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors);

	//�Q�[���f�[�^�̃��[�h
	UFUNCTION(BlueprintCallable)
		static void LoadGameData(const FString& SlotName, const int SlotIndex);
	//�Q�[���f�[�^�̔񓯊����[�h
	UFUNCTION(BlueprintCallable)
		static void AsyncLoadGameData(const FString& SlotName, const int SlotIndex);
	//�Q�[���f�[�^�̃Z�[�u
	UFUNCTION(BlueprintCallable)
		static void SaveGameData(const FString& SlotName, const int SlotIndex);
	//�Q�[���f�[�^�̔񓯊��Z�[�u
	UFUNCTION(BlueprintCallable)
		static void AsyncSaveGameData(const FString& SlotName, const int SlotIndex);
};
