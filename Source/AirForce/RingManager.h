//-----------------------------------------------------------------------------------------------------------------
// ファイル名		:RingManager.h
// 概要				:リングを管理するアクタークラス
// 作成日			:2021/08/20
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/08/20	池村凌太		:	ゲーム開始時にすべてのリングに番号を割り当てる処理を作成
//						:2021/08/20	池村凌太		:	リングがくぐられたら配列から削除する処理を作成
//-----------------------------------------------------------------------------------------------------------------

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingManager.generated.h"

//前方宣言
class ARing;
class ADroneBase;
class AGameManager;

//デバッグdefine
#define DEBUG_RING_COUNT

UCLASS()
class AIRFORCE_API ARingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	//コンストラクタ
	ARingManager();

protected:
	//ゲーム開始時に1度だけ呼ばれる処理
	virtual void BeginPlay() override;

public:	
	//毎フレーム呼ばれる処理
	virtual void Tick(float DeltaTime) override;

public:
	//リングの最大値を取得
	UFUNCTION(BlueprintCallable, Category = "RingManager")
		int GetMaxCount()const { return m_MaxRingCount; }
	//リングの残り数取得
	UFUNCTION(BlueprintCallable, Category = "RingManager")
		int GetCount()const { return m_RingCount; }
	
private:
	//リングを描画するかどうかの判定
	bool IsDraw(const int& _ringIndex) const;
	//リング情報の更新
	void UpdateRingInfo();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ARing*> m_pChildRings;			//リングを格納するコンテナ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int m_MaxRingCount;							//配置されているリングの合計数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int m_RingCount;								//リングの残り数
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int m_RingDrawUpNumber;					//取得したリングの何個先までリングを表示するか決める値
	UPROPERTY(EditAnywhere)
		ADroneBase* m_pDrone;					//ドローンの情報
	UPROPERTY(EditAnywhere)
		AGameManager* m_pGameManager;	//ゲームマネージャーの情報
};
