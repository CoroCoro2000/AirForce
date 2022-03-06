//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:RingManager.h
// 概要				:レベル上にあるRingを管理するマネージャークラス
// 作成日			:2022/03/06
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/03/06		:プレイヤーから離れた位置にあるリングのFPSを下げる処理の追加
//------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingManager.generated.h"

class ARing;
class APlayerDrone;

//リングのTickLOD設定のパラメーター構造体
USTRUCT(BlueprintType)
struct FTickLODSetting
{
	GENERATED_BODY()

public:
	FTickLODSetting()
		: FPS(60)
		, Distance(500.f)
	{}

	FTickLODSetting(const float& NewFPS, const float& NewDistance)
		: FPS(NewFPS)
		, Distance(NewDistance)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FPS;																	//設定された範囲内に存在するリングのFPS上限値
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;															//この距離の範囲内に存在するリングのFPSを設定する
};

UCLASS()
class AIRFORCE_API ARingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARingManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//配置時に実行される関数
	virtual void OnConstruction(const FTransform& Transform)override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//リングの初期化
	void InitializeRing();
	//リングの更新
	void UpdateRings();

private:
	UPROPERTY(EditAnywhere)
		APlayerDrone* m_pPlayer;										//プレイヤー
	UPROPERTY(EditAnywhere)
		TArray<ARing*> m_pRings;									//このマネージャーが管理するリング
	UPROPERTY(EditAnywhere)
		TArray<FTickLODSetting> m_TickLODSettings;		//距離に応じたリングのFPSを可変する設定項目、配列は距離が近い順にソートされる
};
