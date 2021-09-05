//--------------------------------------------------------------------------------------------
// ファイル名		:AlignedProceduralMesh.cpp
// 概要				:規則的に並ぶメッシュを自動生成するアクタークラス
// 作成日			:2021/08/07
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------

#include "AlignedProceduralMesh.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameUtility.h"

// Sets default values
AAlignedProceduralMesh::AAlignedProceduralMesh()
	: m_pMeshes(NULL)
	, m_ArrangementType(EARRANGEMENT::LINEAR)
	, m_MeshCount(1)
	, m_Distance(10.f)
	, m_MeshRelativeRotation(FRotator::ZeroRotator)
	, m_RandomizeStatus()
	, m_GridStatus()
	, m_bFix(false)
{
	//処理はエディタ上でしか実行しない為、Tickは無効にする
	PrimaryActorTick.bCanEverTick = false;

	//メッシュインスタンス生成
	m_pMeshes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Meshes"));
	if (m_pMeshes)
	{
		RootComponent = m_pMeshes;
		m_pMeshes->SetMobility(EComponentMobility::Static);
	}
}

// Called when the game starts or when spawned
void AAlignedProceduralMesh::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAlignedProceduralMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//直線状に生成する処理
void AAlignedProceduralMesh::CreateLinear()
{
	//固定されていなければ指定した数だけ生成する
	if (!m_bFix)
	{
		//トランスフォームのリセット
		m_TempInstanceTransform.Empty();

		for (int index = 0; index < m_MeshCount; ++index)
		{
			FRotator initRotation = m_MeshRelativeRotation;
			FVector initLocation = FVector::ZeroVector;
			initLocation.Y = (float)index * m_Distance;
			float initScale = 1.f;

			//メッシュ間の距離をランダム化するかどうか
			if (m_RandomizeStatus.bRandomizeDistance)
			{
				float RandomDistance = FMath::FRandRange(m_RandomizeStatus.RandomDistanceMin, m_RandomizeStatus.RandomDistanceMax);
				initLocation.Y *= CGameUtility::SetDecimalTruncation(RandomDistance, 3);
			}
			//メッシュのスケールをランダム化するかどうか
			if (m_RandomizeStatus.bRandomizeScale)
			{
				float RandomScale = FMath::FRandRange(m_RandomizeStatus.RandomScaleMin, m_RandomizeStatus.RandomScaleMax);
				initScale *= CGameUtility::SetDecimalTruncation(RandomScale, 3);
			}
			//メッシュの回転をランダム化するかどうか
			if (m_RandomizeStatus.bRandomizeRotaion)
			{
				float RandomRotation = FMath::FRandRange(-m_RandomizeStatus.RandomRotaionYawAngle, m_RandomizeStatus.RandomRotaionYawAngle);
				initRotation.Yaw += CGameUtility::SetDecimalTruncation(RandomRotation, 3);
			}

			//メッシュのトランスフォームを設定
			FTransform initTransform = FTransform(initRotation, initLocation, FVector(initScale));

			//メッシュの追加
			m_pMeshes->AddInstance(initTransform);
			//メッシュのトランスフォーム情報を保存
			m_TempInstanceTransform.Add(initTransform);
		}
	}
	//固定されているときは変更前の状態で生成する
	else
	{
		for (FTransform transform : m_TempInstanceTransform)
		{
			m_pMeshes->AddInstance(transform);
		}
	}
}

//格子状に生成する処理
void AAlignedProceduralMesh::CreateGrid()
{
	FVector initLocation = FVector::ZeroVector;
	for (int x = 0; x < m_GridStatus.MeshCountX; ++x)
	{
		initLocation.X = (float)x * m_GridStatus.Distance.X;
		for (int y = 0; y < m_GridStatus.MeshCountY; ++y)
		{
			initLocation.Y = (float)y * m_GridStatus.Distance.Y;
			for (int z = 0; z < m_GridStatus.MeshCountZ; ++z)
			{
				initLocation.Z = (float)z * m_GridStatus.Distance.Z;

				FTransform initTransform = FTransform(m_MeshRelativeRotation, initLocation);
				m_pMeshes->AddInstance(initTransform);
			}
		}
	}
}

//メッシュ情報の更新
void AAlignedProceduralMesh::UpdateMesh()
{
	if (!m_pMeshes) { return; }
	if (!m_pMeshes->GetStaticMesh()) { return; }
	//更新される前のメッシュ情報をリセット
	if ((int)m_pMeshes->GetInstanceCount() > 0) { m_pMeshes->ClearInstances(); }

	switch (m_ArrangementType)
	{
	case EARRANGEMENT::LINEAR:
		CreateLinear();
		break;
	case EARRANGEMENT::GRID:
		CreateGrid();
		break;
	default:
		break;
	}

#ifdef DEBUG_INSTANCECOUNT
	UE_LOG(LogTemp, Warning, TEXT("MeshInctanceCount:%i"), m_pMeshes->GetInstanceCount());
#endif // DEBUG_INCTANCECOUNT
}

//エディタ上で配置時、または内部の値が変更された時に呼び出される関数
void AAlignedProceduralMesh::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//メッシュを生成する
	UpdateMesh();
}