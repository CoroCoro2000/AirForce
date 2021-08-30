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
		GRID					UMETA(DisplayName = "Grid"),
		NUM					UMETA(Hidden),
	};
}

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

USTRUCT(BlueprintType)
struct FRandomizeStatus
{
	GENERATED_USTRUCT_BODY()

		//コンストラクタ
		FRandomizeStatus()
		: bRandomizeScale(false)
		, RandomScaleMax(1.05f)
		, RandomScaleMin(0.95f)
		, bRandomizeDistance(false)
		, RandomDistanceMax(1.05f)
		, RandomDistanceMin(0.95f)
		, bRandomizeRotaion(false)
		, RandomRotaionYawAngle(1.f)
	{}

	UPROPERTY(EditAnywhere)
		bool bRandomizeScale;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeScale"))
		float RandomScaleMax;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeScale"))
		float RandomScaleMin;
	UPROPERTY(EditAnywhere)
		bool bRandomizeDistance;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeDistance"))
		float RandomDistanceMax;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeDistance"))
		float RandomDistanceMin;
	UPROPERTY(EditAnywhere)
		bool bRandomizeRotaion;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeRotaion"))
		float RandomRotaionYawAngle;
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
	//格子状に生成する処理
	void CreateGrid();
	//メッシュ情報の更新
	void UpdateMesh();
	//トランスフォームを格納する配列のリセット
	void ClearTempTransform();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		UHierarchicalInstancedStaticMeshComponent* m_pMeshes;							//メッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		TEnumAsByte<EARRANGEMENT::Type> m_ArrangementType;						//メッシュをどのように並べるか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix && m_ArrangementType != EARRANGEMENT::GRID"))
		int m_MeshCount;																						//配置するメッシュの数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix && m_ArrangementType != EARRANGEMENT::GRID"))
		float m_Distance;																						//メッシュ同士の間隔
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		FRotator m_MeshRelativeRotation;																//配置するメッシュの相対角度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		FRandomizeStatus m_RandomizeStatus;														//ランダム化する際のパラメーター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix && m_ArrangementType == EARRANGEMENT::GRID"))
		FGridStatus m_GridStatus;																			//格子上に配置する際に設定するパラメーター
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_IsFix;																								//現在の配置で固定するかどうか
		TArray<FTransform> m_pTempInstanceTransform;										//配置されているメッシュのトランスフォームを保存しておく配列
};
