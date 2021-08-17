//--------------------------------------------------------------------------------------------
// ファイル名		:RegularlyAlignedProceduralMesh.h
// 概要				:規則的に並ぶメッシュを自動生成するアクタークラス
// 作成日			:2021/08/07
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------
//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlignedProceduralMesh.generated.h"

//前方宣言
class UHierarchicalInstancedStaticMeshComponent;

//メッシュの並べ方を指定する列挙
UENUM(BlueprintType)
namespace EARRANGEMENT
{
	enum Type
	{
		LINEAR				UMETA(DisplayName = "Linear"),
		CURVE				UMETA(DisplayName = "Curve"),
		CIRCLE				UMETA(DisplayName = "Circle"),
		SPIRAL				UMETA(DisplayName = "Spiral"),
		GRID					UMETA(DisplayName = "Grid"),
		NUM					UMETA(Hidden),
	};
}

USTRUCT(BlueprintType)
struct FLinearStatus
{
	GENERATED_USTRUCT_BODY()

		FLinearStatus()
		: Distance(10.f)
	{}

	float Distance;
};

USTRUCT(BlueprintType)
struct FCurveStatus
{
	GENERATED_USTRUCT_BODY()

};

USTRUCT(BlueprintType)
struct FCircleStatus
{
	GENERATED_USTRUCT_BODY()

};

USTRUCT(BlueprintType)
struct FSpiralStatus
{
	GENERATED_USTRUCT_BODY()

	FSpiralStatus()
		: Direction(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector Direction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FRotator Rotation;
};

USTRUCT(BlueprintType)
struct FGridStatus
{
	GENERATED_USTRUCT_BODY()

	FGridStatus()
		: MeshCountX(1)
		, MeshCountY(1)
		, MeshCountZ(1)
		, Distance(FVector::ZeroVector)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MeshCountX;					//X軸のメッシュの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MeshCountY;					//Y軸のメッシュの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MeshCountZ;					//Z軸のメッシュの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector Distance;				//隣接するメッシュとの距離
};

//デバッグ用define
//#define DEBUG_TRANSFORM			//トランスフォームのログ出力
#define DEBUG_INSTANCECOUNT		//保持しているメッシュインスタンスの数を出力

UCLASS()
class AIRFORCE_API AAlignedProceduralMesh : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAlignedProceduralMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	//エディタ上で配置時、または内部の値が変更された時に呼び出される関数
	virtual void OnConstruction(const FTransform& Transform)override;

	//直線状に生成する処理
	void CreateLinear();
	//曲線状に生成する処理
	void CreateCurved();
	//円状に生成する処理
	void CreateCircular();
	//螺旋状に生成する処理
	void CreateSpiral();
	//格子状に生成する処理
	void CreateGrid();
	//メッシュ情報の更新
	void UpdateMesh();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		UHierarchicalInstancedStaticMeshComponent* m_pMeshes;							//メッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		int m_MeshCount;																		//配置するメッシュの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		float m_Distance;																		//メッシュ同士の間隔
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		TEnumAsByte<EARRANGEMENT::Type> m_ArrangementType;		//メッシュをどのように並べるか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		FSpiralStatus m_SpiralStatus;														//螺旋状に配置する際に設定するパラメーター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		FGridStatus m_GridStatus;															//格子上に配置する際に設定するパラメーター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
		FRotator m_MeshRelativeRotation;												//配置するメッシュの相対角度
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
	//	bool m_bLockRotationPitch;														//メッシュのPitch軸回転をロックするかどうか
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
	//	bool m_bLockRotationYaw;															//メッシュのYaw軸回転をロックするかどうか
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
	//	bool m_bLockRotationRoll;															//メッシュのRoll軸回転をロックするかどうか
};
