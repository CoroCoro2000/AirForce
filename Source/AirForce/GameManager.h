//-------------------------------------------------------------------------------------
// ファイル名	:GameManager.h
// 概要			:ゲームの進行を管理するクラス
// 作成日		:2021/07/07
// 作成者		:19CU0104 池田翔一郎
// 更新内容		:
//-------------------------------------------------------------------------------------

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneBase.h"
#include "Ring.h"
#include "GameManager.generated.h"

//シーン管理列挙
UENUM(BlueprintType)
namespace ECURRENTSCENE
{
	enum Type
	{
		SCENE_TITLE = 0		UMETA(DisPlayName = "TITLE"),
		SCENE_FIRST 			UMETA(DisPlayName = "FIRST"),
		SCENE_RESULT			UMETA(DisPlayName = "RESULT"),
	};
}

//シーン管理列挙
UENUM(BlueprintType)
namespace ENEXTSCENE
{
	enum Type
	{
		SCENE_ONCEMORE = 0		UMETA(DisPlayName = "ONCEMORE"),
		SCENE_RASECHANGE			UMETA(DisPlayName = "RASECHANGE"),
		SCENE_EXIT						UMETA(DisPlayName = "EXIT"),
	};
}

USTRUCT(BlueprintType)
struct FNEXTSCENE
{
	GENERATED_USTRUCT_BODY()

public:
	FNEXTSCENE()
	:_NextScene(ENEXTSCENE::SCENE_ONCEMORE)
	{}

	FNEXTSCENE operator++(int) 
	{
		_NextScene= TEnumAsByte<ENEXTSCENE::Type>(_NextScene + 1);
		return *this;
	}

	FNEXTSCENE operator--(int) 
	{ 
		_NextScene = TEnumAsByte<ENEXTSCENE::Type>(_NextScene - 1);
	return *this; 
	}

	FNEXTSCENE operator=(int n)
	{
		_NextScene = TEnumAsByte<ENEXTSCENE::Type>(n);
		return *this;
	}

	bool operator>(int n) { return _NextScene > n ? true : false; }
	bool operator<(int n) { return _NextScene < n ? true : false; }
	bool operator==(int n) { return _NextScene == n ? true : false; }

	TEnumAsByte<ENEXTSCENE::Type> GetNextScene()const { return _NextScene; }
public:
	UPROPERTY(EditAnywhere, Category = "FNEXTSCENE", DisplayName = "NEXTSCENE")
		TEnumAsByte<ENEXTSCENE::Type> _NextScene;
};

UCLASS()
class AIRFORCE_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	//コンストラクタ
	AGameManager();

protected:
	//ゲーム開始時に1度だけ処理
	virtual void BeginPlay() override;

public:	
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;


	void NextSceneUp();
	void NextSceneDown();

	//ポーズの設定
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetIsPause() { m_isPause = !m_isPause; }

	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetisSceneTransition(const bool b) { m_isSceneTransition = b; }

	void SetLeftStickInputValue(const float n) { m_LeftStickInputValue = n; }
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsPause() const {return m_isPause; }

	//レース開始フラグの取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsStart()const { return m_isStart; }

	//ゴールフラグの取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetIsGoal(const bool& _isGoal) { m_isGoal = _isGoal; }

	//ゴールフラグの取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsGoal()const { return m_isGoal; }

	//シーン遷移フラグの取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetisSceneTransition()const { return m_isSceneTransition; }

	//レース前のカウントダウン時間の取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		float GetCountDownTime()const { return m_CountDownTime; }

	//ラップタイムの分取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapMinute()const { return (int)m_RapTime / 60; }
	//ラップタイムの秒取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapSecond()const { return (int)m_RapTime % 60; }
	//ラップタイムのミリ秒取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapMiliSecond()const { return (m_RapTime - (int)m_RapTime) * 1000; }

	//レース後のシーン遷移取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		TEnumAsByte<ENEXTSCENE::Type> GetNextScene()const { return m_NextScene.GetNextScene(); }

private:
	UPROPERTY(EditAnywhere, Category = "Flag")
		TEnumAsByte<ECURRENTSCENE::Type> m_CurrentScene;				//現在のシーンステート
	UPROPERTY(EditAnywhere, Category = "Flag")
		FNEXTSCENE m_NextScene;						//次のシーンステート

	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isPause;								//ポーズフラグ
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isStart;								//レース開始フラグ
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isGoal;								//ゴールフラグ
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isSceneTransition;					//レース後のシーン遷移フラグ

	UPROPERTY(EditAnywhere, Category = "Stage")
		float m_CountDownTime;						//レース前のカウントダウン
	UPROPERTY(VisibleAnywhere, Category = "Stage")
		float m_RapTime;							//ゴールするまでの時間

	UPROPERTY(VisibleAnywhere, Category = "Drone")
		ADroneBase* m_Drone;						//ドローン

	UPROPERTY(VisibleAnywhere, Category = "Drone")
		int m_GoalRingNumber;						//ゴールのリングの通し番号

	float m_LeftStickInputValue;
};
