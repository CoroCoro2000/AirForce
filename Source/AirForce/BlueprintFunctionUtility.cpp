// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionUtility.h"
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

//ターゲットに向かう回転量を返す関数
FQuat UBlueprintFunctionUtility::RInterpToQuaternion(const FRotator Current, const FRotator Target, const float DeltaTime, const float InterpSpeed)
{
	//回転角度
	float RotateAngle = DeltaTime * InterpSpeed;
	//現在の回転とターゲットのXベクトル
	FVector CurrentXVector = Current.Vector();
	FVector TargetXVector = Target.Vector();
	//回転軸
	FVector RotationAxis = CurrentXVector * TargetXVector;

	//なす角を求める
	float dot = CurrentXVector | TargetXVector;
	float DegAngle = UKismetMathLibrary::DegAcos(dot);

	//回転角度よりなす角が小さい場合はTargetまでの回転量を返す
	if (DegAngle <= RotateAngle)
	{
		return Target.Quaternion();
	}
	else
	{
		//クォータニオンによる回転
		return UKismetMathLibrary::Conv_VectorToQuaterion(UKismetMathLibrary::RotateAngleAxis(CurrentXVector, RotateAngle, RotationAxis));
	}
}