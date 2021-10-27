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

	//�w�肳�ꂽ�^�O�ƈ�v����A�N�^�[���ׂĂ��擾����֐�
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "ActorClass", DynamicOutputParam = "OutActors"))
		static void GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors);
public:
	//�^�[�Q�b�g�Ɍ���������]��Ԃ��֐�
	UFUNCTION(BlueprintCallable)
		static FQuat RInterpToQuaternion(const FRotator Current, const FRotator Target, const float DeltaTime, const float InterpSpeed);
};
