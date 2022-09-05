//--------------------------------------------------------------------------------------------
// ファイル名		:ComponentOnSplineActor.h
// 概要				:スプライン上にコンポーネントを自動生成するアクタークラス
// 作成日			:2021/08/30
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------
//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComponentOnSplineActor.generated.h"

UENUM(BlueprintType)
namespace EComponent
{
	enum Type
	{
		Niagara				UMETA(DisplayName = "Niagara"),
		NUM					UMETA(Hidden)
	};
}

//前方宣言
class USplineComponent;
class UNiagaraSystem;
class UNiagaraComponent;

//デバッグ用define
//#define DEBUG_TRANSFORM				//トランスフォームのログ出力
#define DEBUG_COMPONENTCOUNT			//保持しているコンポーネントの数を出力

UCLASS()
class AIRFORCE_API AComponentOnSplineActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AComponentOnSplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//スプライン上に生成するNiagaraエフェクト情報の更新
	void SpawnNiagaraOnSpline();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
		USplineComponent* m_pSpline;										//オブジェクトを配置する位置を示すスプライン
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
		TEnumAsByte<EComponent::Type> m_ComponentType;						//配置するコンポーネントの種類
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		UNiagaraSystem* m_pNiagaraEffect;									//表示するエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		TArray<UNiagaraComponent*> m_pNiagaraEffectComponents;				//スプライン上に配置するエフェクトコンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		int m_ComponentCount;												//配置するコンポーネントの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		FRotator m_ComponentRelativeRotation;								//配置するコンポーネントの相対角度
};
