// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionUtility.h"

#include "RacingD_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//�R���X�g���N�^
UBlueprintFunctionUtility::UBlueprintFunctionUtility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//�w�肳�ꂽ�^�O�ƈ�v����A�N�^�[���ׂĂ��擾����֐�
void UBlueprintFunctionUtility::GetAllActorHasTags(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<FName> FindTags, TArray<AActor*>& OutActors)
{
	//��������^�O������Ό����J�n
	if ((int)FindTags.Num() > 0)
	{
		TArray<AActor*> FindActors;
		//���x�����̃A�N�^�[���ׂĂ��擾
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ActorClass, FindActors);

		//�������ʂɃA�N�^�[�������
		if ((int)FindActors.Num() > 0)
		{
			//���������A�N�^�[�����ɒ��ׂ�
			for (AActor* pActor : FindActors)
			{
				if (pActor)
				{
					//�^�O�ƈ�v����A�N�^�[������������z��ɒǉ�
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

FText UBlueprintFunctionUtility::Conv_RecordTimeToText(const FRecordTime& InRecordTime)
{
	return InRecordTime.ToText();
}