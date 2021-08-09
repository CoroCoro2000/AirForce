//--------------------------------------------------------------------------------------------
// ファイル名		:ProceduralMeshOnSpline.h
// 概要				:スプライン上に指定数のメッシュを自動生成するアクタークラス
// 作成日			:2021/08/06
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------
//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshOnSpline.generated.h"

//前方宣言
class USplineComponent;
class UInstancedStaticMeshComponent;

//デバッグ用define
//#define DEBUG_TRANSFORM			//トランスフォームのログ出力
#define DEBUG_INSTANCECOUNT		//保持しているメッシュインスタンスの数を出力

UCLASS()
class AIRFORCE_API AProceduralMeshOnSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshOnSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//エディタ上で配置時、または内部の値が変更された時に呼び出される関数
	virtual void OnConstruction(const FTransform& Transform)override;

	//スプライン上に生成するメッシュ情報の更新
	void UpdateMeshOnSpline();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
		USplineComponent* m_pSpline;							//オブジェクトを配置する位置を示すスプライン
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		UInstancedStaticMeshComponent* m_pMeshes;	//スプライン上に配置されるメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		int m_MeshCount;												//配置するメッシュの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		FRotator m_MeshRelativeRotation;						//配置するメッシュの相対角度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_bLockRotationPitch;								//メッシュのPitch軸回転をロックするかどうか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_bLockRotationYaw;									//メッシュのYaw軸回転をロックするかどうか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_bLockRotationRoll;									//メッシュのRoll軸回転をロックするかどうか
};
