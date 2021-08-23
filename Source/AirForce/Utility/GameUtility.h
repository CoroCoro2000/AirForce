//----------------------------------------------------------------------------------------------
// ファイル名		:GameUtility.h
// 概要				:どこからでも呼び出せる関数の処理をまとめたユーティリティクラス
// 作成日			:2021/07/26
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//----------------------------------------------------------------------------------------------
//インクルードガード
#pragma once

//インクルード
#include "TimerManager.h"

//前方宣言
template <typename WrappedRetValType, typename... ParamTypes>
class TBaseDelegate;
template<class FuncType>
class TFunction;
class FName;
class AActor;
struct FTimerHandle;

class CGameUtility
{
public:
	//小数第n位未満切り捨て
	float SetDecimalTruncation(float value, int n);

	//シェアードポインタ型でメモリを確保する関数
	template<class T, class U>
	static TSharedPtr<T> CreateSharedPtr(U _initializer) { return (TSharedPtr<T>)new T(_initializer); }

	//*レベル内全てに対して検索を行うため、Tickで使う場合は注意すること。
	//タグからアクターを取得する関数(第1引数：この関数を呼び出すアクター、第2引数：検索するアクターのタグ名)
	static AActor* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag);

	//*テンプレート関数は分割コンパイルが出来ないため、ヘッダーに書く
	//任意時間後に登録した処理を呼び出す関数(第1引数：ワールド、第2引数：遅延する時間(秒で指定)、第3引数：処理のループをするかどうか、第4引数：任意時間後に実行する処理(ラムダ式)、第5引数：実行する処理に必要な引数(可変長))
	template<typename FunctorType, typename... VarTypes>
	static inline void Delay(UWorld* _pWorld, const float& _duration, const bool& _bLoop, const FunctorType&& _func, const VarTypes... _varTypes)
	{
		//NULLチェック
		if (!_pWorld) { return; }

		//デリゲートを行うタイムハンドルを作成
		FTimerHandle timeHandle;
		//実行するラムダ式をデリゲードにバインド
		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda((TFunction<void(VarTypes...)>&&)_func, _varTypes...);

		//指定された秒数後に実行するように設定
		_pWorld->GetTimerManager().SetTimer(timeHandle, timerDelegate, _duration, _bLoop);
	}
};