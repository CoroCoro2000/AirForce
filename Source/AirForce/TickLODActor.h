//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODActor.h
// 概要				:アクターのTickLODレベルを変更する
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
	//Tickを更新するフレームを設定
	void SetTickFPS(const uint8& NewTickFPS) { m_TickFPS = NewTickFPS; }
	//現在の更新されているFPSを取得
	UFUNCTION(BlueprintCallable)
		uint8 GetTickFPS()const { return m_TickFPS; }

protected:
	UPROPERTY(EditAnywhere, Category = "TickLOD")
		uint8 m_TickFPS;																						//1秒間にTickを更新する回数
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		float m_LastTickTime;																				//最後にTickが実行された時刻
};
