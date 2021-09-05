//--------------------------------------------------------------------------------------------
// ファイル名		:ProceduralMeshOnSpline.cpp
// 概要				:スプライン上に指定数のメッシュを自動生成するアクタークラス
// 作成日			:2021/08/06
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------
//インクルード
#include "ProceduralMeshOnSpline.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AProceduralMeshOnSpline::AProceduralMeshOnSpline()
	: m_pSpline(NULL)
	, m_pMeshes(NULL)
	, m_MeshCount(1)
	, m_Rotation(FRotator::ZeroRotator)
{
	//処理はエディタ上でしか実行しない為、Tickは無効にする
	PrimaryActorTick.bCanEverTick = false;

	//スプライン生成
	m_pSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	if (m_pSpline)
	{
		RootComponent = m_pSpline;
	}

	//メッシュコンポーネント生成
	m_pMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Meshes");
	if (m_pMeshes && m_pSpline)
	{
		m_pMeshes->SetupAttachment(m_pSpline);
	}
}

// Called when the game starts or when spawned
void AProceduralMeshOnSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralMeshOnSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//スプライン上にメッシュを生成する処理
void AProceduralMeshOnSpline::UpdateMeshOnSpline()
{
	//NULLチェック
	if (m_MeshCount == 0) { return; }
	if (!m_pSpline) { return; }
	if (!m_pMeshes) { return; }
	if (!m_pMeshes->GetStaticMesh()) { return; }

	//変更前のメッシュ情報を削除
	if ((int)m_pMeshes->GetInstanceCount() > 0) { m_pMeshes->ClearInstances(); }

	//スプラインの長さを取得
	const float splineLength = m_pSpline->GetSplineLength();

	//配置するメッシュの数だけ生成処理を行う
	for (int index = 0; index < m_MeshCount; ++index)
	{
		//スプライン上のどの位置ににメッシュを生成するか決める
		const float rate = (float)index / (float)m_MeshCount;
		const float distance = rate * splineLength;
		const FVector initLocation = m_pSpline->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
		FRotator initRotation = m_pSpline->GetRotationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);

		//配置するメッシュのトランスフォームを設定
		const FTransform initTransform = FTransform(initRotation + m_Rotation, initLocation);

		//メッシュインスタンスを追加
		m_pMeshes->AddInstance(initTransform);

#ifdef DEBUG_TRANSFORM
		UE_LOG(LogTemp, Warning, TEXT("MeshTransform:%s"), *MeshTransform.ToString());
#endif // DEBUG_TRANSFORM
	}
#ifdef DEBUG_INSTANCECOUNT
	UE_LOG(LogTemp, Warning, TEXT("MeshInctanceCount:%i"), m_pMeshes->GetInstanceCount());
#endif // DEBUG_INCTANCECOUNT
}

//エディタ上で配置時、または内部の値が変更された時に呼び出される関数
void AProceduralMeshOnSpline::OnConstruction(const FTransform& Transform)
{
	//スプライン上にメッシュを生成
	UpdateMeshOnSpline();
}