//--------------------------------------------------------------------------------------------
// ファイル名		:AAffectedByWindActor.cpp
// 概要				:ドローンの風の影響を受けるアクタークラス
// 作成日			:2021/08/26
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AffectedByWindActor.generated.h"

class UStaticMeshComponent;

#define WINDSPEED_MIN 1.f
#define WINDSPEED_MAX 5.f

UCLASS()
class AIRFORCE_API AAffectedByWindActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAffectedByWindActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//受ける風の強さを設定する関数
	void SetWindSpeed(const float& _windSpeed);

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pMesh;			//メッシュ
	UPROPERTY(EditAnywhere)
		float m_WindSpeed;
	UPROPERTY(EditAnywhere)
		float m_TempWindSpeed;
};
