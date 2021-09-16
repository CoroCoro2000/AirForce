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
#include "Kismet/KismetTextLibrary.h"
#include "GameManager.generated.h"

class USoundBase;

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

	//カウントダウン処理
	void CountDown(float DeltaTime);

	void NextSceneUp();
	void NextSceneDown();

	//ポーズの設定
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetIsPause() { m_isPause = !m_isPause; }
	//シーン遷移フラグの設定
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
	//スコア書き込みフラグの取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsScoreWrite()const { return m_isScoreWrite; }
	//シーン遷移フラグの取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetisSceneTransition()const { return m_isSceneTransition; }

	//レース前のカウントダウン時間の取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		float GetCountDownTime()const { return m_CountDownTime; }
	//レース前のカウントダウンテキストの取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		FString GetCountDownText()const { return m_CountDownText; }

	//ラップタイムの取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapTime()const { return m_RapTime; }
	//ラップタイムの分取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		FText GetRapMinuteText()const { return UKismetTextLibrary::Conv_IntToText((int)m_RapTime / 60, false, true, 1); }
	//ラップタイムの秒取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		FText GetRapSecondText()const { return UKismetTextLibrary::Conv_IntToText((int)m_RapTime % 60, false, true, 2); }
	//ラップタイムのミリ秒取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		FText GetRapMiliSecondText()const { return UKismetTextLibrary::Conv_IntToText((m_RapTime - (int)m_RapTime) * 1000, false, true, 3); }

	//ラップタイムの分取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapMinute()const { return (int)m_RapTime / 60; }
	//ラップタイムの秒取得m_isGoal
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapSecond()const { return (int)m_RapTime % 60; }
	//ラップタイムのミリ秒取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapMiliSecond()const { return (m_RapTime - (int)m_RapTime) * 1000; }

	//レース後のシーン遷移取得
	UFUNCTION(BlueprintCallable, Category = "Stage")
		TEnumAsByte<ENEXTSCENE::Type> GetNextScene()const { return m_NextScene.GetNextScene(); }

	//ラップタイムテキスト取得
	UFUNCTION(BlueprintCallable, Category = "Result")
		FText GetRapTimeRanking(int n) { return n < m_RapTimeText.Num() ? FText::FromString(m_RapTimeText[n]) : FText::FromString(m_RapDefaultText); }

	//今回のタイムのプレイヤーの順位取得
	UFUNCTION(BlueprintCallable, Category = "Result")
		int GetPlayerRank() { return m_PlayerRank; }
	//ラップタイム並び替え
	UFUNCTION(BlueprintCallable, Category = "Result")
		void RapTimeSort();

private:
	UPROPERTY(EditAnywhere, Category = "Flag")
		TEnumAsByte<ECURRENTSCENE::Type> m_CurrentScene;				//現在のシーンステート
	UPROPERTY(EditAnywhere, Category = "Flag")
		FNEXTSCENE m_NextScene;																//次のシーンステート

	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isPause;							//ポーズフラグ
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isStart;								//レース開始フラグ
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isGoal;								//ゴールフラグ
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isSceneTransition;					//レース後のシーン遷移フラグ

	UPROPERTY(EditAnywhere, Category = "Stage")
		float m_CountDownTime;						//レース前のカウントダウン
	UPROPERTY(VisibleAnywhere, Category = "Stage")
		FString m_CountDownText;					//レース前のカウントダウン表示テキスト
	UPROPERTY(VisibleAnywhere, Category = "Stage")
		float m_RapTime;							//ゴールするまでの時間

	UPROPERTY(VisibleAnywhere, Category = "Result")
		TArray<FString> m_RapTimeText;			//ラップテキスト
	UPROPERTY(EditAnywhere, Category = "Result")
		FString m_RapDefaultText;				//ラップデフォルトテキスト
	UPROPERTY(EditAnywhere, Category = "Result")
		FString m_RapTimeTextPath;				//ラップテキストパス
	UPROPERTY(EditAnywhere, Category = "Result")
		int m_PlayerRank;						//プレイヤーの順位
	UPROPERTY(EditAnywhere, Category = "Result")
		int m_RankingDisplayNum;					//ランキングに残す数
	UPROPERTY(VisibleAnywhere, Category = "Result")
		bool m_isScoreWrite;						//スコア書き込みフラグ
	UPROPERTY(VisibleAnywhere, Category = "Result")
		bool m_isNewRecord;							//最速タイム更新フラグ
	UPROPERTY(VisibleAnywhere, Category = "Drone")
		ADroneBase* m_PlayerDrone;						//プレイヤードローン
	UPROPERTY(VisibleAnywhere, Category = "Drone")
		ADroneBase* m_GhostDrone;						//ゴーストドローン

	UPROPERTY(VisibleAnywhere, Category = "Drone")
		int m_GoalRingNumber;						//ゴールのリングの通し番号

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_CountDownSE;						//カウントダウンSE
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_StartSE;						//スタートSE

	float m_LeftStickInputValue;

};
