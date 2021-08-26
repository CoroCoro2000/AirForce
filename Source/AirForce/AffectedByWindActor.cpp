//--------------------------------------------------------------------------------------------
// ファイル名		:AAffectedByWindActor.cpp
// 概要				:ドローンの風の影響を受けるアクタークラス
// 作成日			:2021/08/26
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//--------------------------------------------------------------------------------------------

#include "AffectedByWindActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAffectedByWindActor::AAffectedByWindActor()
	: m_pMesh(NULL)
	, m_WindSpeed(1.f)
	, m_TempWindSpeed(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//メッシュ生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//メッシュの生成に成功したら、デフォルトの風の強さを設定
	if (m_pMesh)
	{
		RootComponent = m_pMesh;
		m_pMesh->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), m_WindSpeed);
	}
}

// Called when the game starts or when spawned
void AAffectedByWindActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAffectedByWindActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//受ける風の強さを設定する関数
void AAffectedByWindActor::SetWindSpeed(const float& _windSpeed)
{
	m_WindSpeed = _windSpeed;

	//風の強さの更新処理
	if (m_pMesh)
	{
		m_pMesh->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), m_WindSpeed);
		m_TempWindSpeed = m_WindSpeed;
	}

	UE_LOG(LogTemp, Warning, TEXT("m_WindSpeed    :%f"), m_WindSpeed);
	UE_LOG(LogTemp, Warning, TEXT("m_TempWindSpeed:%f"), m_TempWindSpeed);
}