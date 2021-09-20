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

//カラーステート情報
UENUM(BlueprintType)
namespace ECOLOR_STATE
{
	enum Type
	{
		RED							UMETA(DisplayName = "RED"),
		ORANGE					UMETA(DisplayName = "ORANGE"),
		YELLOW					UMETA(DisplayName = "YELLOW"),
		GREEN						UMETA(DisplayName = "GREEN"),
		BLUE							UMETA(DisplayName = "BLUE"),
		INDIGO						UMETA(DisplayName = "INDIGO"),
		PURPLE						UMETA(DisplayName = "PURPLE"),
		NUM							UMETA(Hidden),
	};
}

//デバッグdefine
#define DEBUG_RING_COUNT
//カラー情報
#define LINEARCOLOR_ORANGE FLinearColor(0.94921875f, 0.609375f, 0.0703125f)		//オレンジ
#define LINEARCOLOR_PURPLE FLinearColor(0.66015625f, 0.02734375f, 0.890625f)		//紫
#define LINEARCOLOR_INDIGO FLinearColor(0, 0.296875f, 0.44140625f)						//藍色

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
	
private:
	//ターゲットカラーを取得
	void UpdateColorState();
	//リングの色更新
	void UpdateColor(const float& DeltaTime);
	//ステートからカラーターゲットを取得
	FLinearColor GetTargetColor(const int32& _colorIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ARing*> m_pChildRings;			//リングを格納する配列
	UPROPERTY(VisibleAnywhere)
		ADroneBase* m_pDrone;					//ドローンの情報
	UPROPERTY(VisibleAnywhere)
		AGameManager* m_pGameManager;	//ゲームマネージャーの情報
	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<ECOLOR_STATE::Type> m_ColorState;	//カラーステート
	UPROPERTY(EditAnywhere)
		FLinearColor m_RingColor;													//現在の色情報
	UPROPERTY(EditAnywhere)
		FLinearColor m_FresnelColor;
	UPROPERTY(VisibleAnywhere)
		FLinearColor m_TargetColor;												//現在の色情報
	UPROPERTY(EditAnywhere)
		float m_ColorTransitionSpeed;												//色の遷移する速度
	UPROPERTY(EditAnywhere)
		int32 m_DelayTempo;															//色を何テンポ遅らせるか
};
