// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionUtility.h"

#include "RacingD_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//コンストラクタ
UBlueprintFunctionUtility::UBlueprintFunctionUtility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//指定されたタグと一致するアクターすべてを取得する関数
void UBlueprintFunctionUtility::GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors)
{
	//検索するタグがあれば検索開始
	if ((int)FindTags.Num() > 0)
	{
		TArray<AActor*> FindActors;
		//レベル内のアクターすべてを取得
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ActorClass, FindActors);

		//検索結果にアクターがあれば
		if ((int)FindActors.Num() > 0)
		{
			//見つかったアクターを順に調べる
			for (AActor* pActor : FindActors)
			{
				if (pActor)
				{
					//タグと一致するアクターが見つかったら配列に追加
					for (FName& findTag : FindTags)
					{
						if (pActor->ActorHasTag(findTag))
						{
							OutActors.Add(pActor);
						}
					}
				}
			}
		}
	}
}

//ゲームデータのロード
void UBlueprintFunctionUtility::LoadGameData(const FString& SlotName, const int SlotIndex)
{
	if(URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		pGameInstance->LoadGameData(SlotName,SlotIndex);
	}
}

//ゲームデータの非同期ロード
void UBlueprintFunctionUtility::AsyncLoadGameData(const FString& SlotName, const int SlotIndex)
{
	if(URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		pGameInstance->AsyncLoadGameData(SlotName,SlotIndex);
	}
}

//ゲームデータのセーブ
void UBlueprintFunctionUtility::SaveGameData(const FString& SlotName, const int SlotIndex)
{
	if(const URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		pGameInstance->SaveGameData(SlotName,SlotIndex);
	}
}

//ゲームデータの非同期セーブ
void UBlueprintFunctionUtility::AsyncSaveGameData(const FString& SlotName, const int SlotIndex)
{
	if(URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		pGameInstance->AsyncSaveGameData(SlotName,SlotIndex);
	}
}