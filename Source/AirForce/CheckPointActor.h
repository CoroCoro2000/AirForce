//--------------------------------------------------------------------------------------------
// ファイル名		:CheckPointActor.h
// 概要				:レースモード時のチェックポイントとして扱うアクター
// 作成日			:2021/08/27
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPointActor.generated.h"


class UBoxComponent;

UCLASS()
class AIRFORCE_API ACheckPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//チェックポイントの番号設定
	void SetNumber(const int& _number) { m_CheckPointNumber = _number; }
	//チェックポイントの番号取得
	int GetNumber()const { return m_CheckPointNumber; }
	//次のチェックポイント情報取得
	ACheckPointActor* GetNextCheckPoint()const { return m_pNextCheckPointActor; }

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_CheckPointCollision;
	UPROPERTY(EditAnywhere)
		int m_CheckPointNumber;							//チェックポイントの番号
	UPROPERTY(EditAnywhere)
		ACheckPointActor* m_pNextCheckPointActor;		//次のチェックポイント
};
