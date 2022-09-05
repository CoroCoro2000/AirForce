//----------------------------------------------------------------------------------------------
// ファイル名		:GameUtility.h
// 概要				:どこからでも呼び出せる関数の処理をまとめたユーティリティクラス
// 作成日			:2021/07/26
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//----------------------------------------------------------------------------------------------
//インクルードガード
#pragma once

//前方宣言
class AActor;

class CGameUtility
{
public:
	//小数第n位未満切り捨て
	static float SetDecimalTruncation(float value, int n);
	//小数第n位未満切り捨て
	static FVector SetDecimalTruncation(FVector value, int n);
	//FPSが変わっても移動量を一定にする倍率を返す関数
	static float MoveCorrection(const float& DeltaTime) { return  60.f / (1.f / DeltaTime); }
	//*レベル内全てに対して検索を行うため、Tickで使う場合は注意すること。
	//タグからアクターを取得する関数(第1引数：この関数を呼び出すアクター、第2引数：検索するアクターのタグ名)
	static AActor* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag);
	template<class T>
	static T* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag) { return Cast<T>(GetActorFromTag(_pOwnerActor, _tag)); }

	//バイナリソート
};
