//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODActor.h
// 概要				:1フレーム当たりのTick更新回数を変更できるアクターのベースクラス
// 作成日			:2022/03/09
// 作成者			:19CU0105 池村凌太
//------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TickLODActor.generated.h"

UCLASS()
class AIRFORCE_API ATickLODActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATickLODActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//FPSを設定
	void SetTickFPS(const float& NewTickFPS) { m_TickFPS = NewTickFPS; }
	//FPSを取得
	UFUNCTION(BlueprintCallable)
		float GetTickFPS()const { return m_TickFPS; }
	//LODアクターの番号を設定
	void SetNumber(const uint32& Number) { m_ActorNumber = Number; }

protected:
	//処理可能なフレームか判定
	bool IsProcessableFrame()const;

protected:
	UPROPERTY(EditAnywhere, Category = "TickLOD")
		float m_TickFPS;																						//FPS
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		float m_LastTickTime;																				//最後にTickが実行された時刻
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		uint32 m_ActorNumber;																				//番号
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		uint32 m_FrameCount;																				//処理フレーム数
};
