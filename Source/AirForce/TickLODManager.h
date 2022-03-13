//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODManager.h
// 概要				:アクターのTickLODレベルを管理するマネージャークラス
// 作成日			:2022/03/09
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/03/09		:プレイヤーから離れた位置にあるリングのFPSを下げる処理の追加
//------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TickLODManager.generated.h"

class APlayerDrone;
class ATickLODActor;

//アクターのTickLOD設定のパラメーター構造体
USTRUCT(BlueprintType)
struct FTickLODSetting
{
	GENERATED_BODY()

public:
	FTickLODSetting()
		: FrameRate(1.f)
		, Distance(1500.f)
	{}

	FTickLODSetting(const float& NewFrameRate, const float& NewDistance)
		: FrameRate(NewFrameRate)
		, Distance(NewDistance)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float FrameRate;														//設定された距離より近い時に適用するFPSの割合
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;															//FPSの割合を適用する距離
};

UCLASS()
class AIRFORCE_API ATickLODManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATickLODManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//配置時に実行される関数
	virtual void OnConstruction(const FTransform& Transform)override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//LODのアクター配列の初期化
	void InitializeActorArray();
	//LODの更新
	void UpdateLOD(const float& DeltaTime);

private:
	UPROPERTY(EditAnywhere)
		TArray<FTickLODSetting> m_TickLODSettings;								//距離に応じたリングのFPSを可変する設定項目、配列は距離が近い順にソートされる
	UPROPERTY(EditAnywhere)
		APlayerDrone* m_pPlayer;																//プレイヤー
	UPROPERTY(EditAnywhere)
		TArray<ATickLODActor*> m_pTickLODActors;										//LODの設定をするアクターを格納する配列
};
