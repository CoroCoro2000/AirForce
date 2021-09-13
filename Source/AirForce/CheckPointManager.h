//------------------------------------------------------------------------
// ファイル名		:CheckPointManager.h
// 概要				:チェックポイントを管理するマネージャークラス
// 作成日			:2021/09/13
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/09/13
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPointManager.generated.h"

class ACheckPoint;

UCLASS()
class AIRFORCE_API ACheckPointManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		TArray<ACheckPoint*> m_pCheckPoints;		//管理するチェックポイントを格納する配列
	UPROPERTY(VisibleAnywhere)
		int m_PassedCheckpointNum;						//通過されたチェックポイントの数

};
