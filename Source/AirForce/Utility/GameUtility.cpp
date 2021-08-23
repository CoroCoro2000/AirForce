//----------------------------------------------------------------------------------------------
// ファイル名		:GameUtility.cpp
// 概要				:どこからでも呼び出せる関数の処理をまとめたユーティリティクラス
// 作成日			:2021/07/26
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//----------------------------------------------------------------------------------------------

//インクルード
#include "GameUtility.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

//#define DEBUG_LOG

//小数第n位未満切り捨て
float CGameUtility::SetDecimalTruncation(float value, int n)
{
    value = value * FMath::Pow(10, n);
    value = floor(value);
    value /= FMath::Pow(10, n);
    return value;
}

//タグからアクターを取得する関数(第1引数：この関数を呼び出すアクター、第2引数：検索するアクターのタグ名)
AActor* CGameUtility::GetActorFromTag(AActor* _pOwnerActor, const FName& _tag)
{
    //NULLチェック
    if (!_pOwnerActor) { return NULL; }

    //検索対象は全てのActor
    TSubclassOf<AActor> findClass;
    findClass = AActor::StaticClass();
    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsOfClass(_pOwnerActor->GetWorld(), findClass, actors);

    //検索結果、Actorがあれば
    if (actors.Num() > 0)
    {
        //そのActorの中を順番に検索
        for (AActor* pActor : actors)
        {
            //タグ名で判別する
            if (pActor->ActorHasTag(_tag))
            {
#ifdef DEBUG_LOG
                //確認用メッセージ出力
                FString message = FString("Founded Actor：") + pActor->GetName();
                UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
#endif // DEBUG_LOG
                return pActor;
            }
        }
    }
    return NULL;
}