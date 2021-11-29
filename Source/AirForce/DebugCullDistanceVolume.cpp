// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugCullDistanceVolume.h"
#include "Components/TextRenderComponent.h"

ADebugCullDistanceVolume::ADebugCullDistanceVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	m_Color = FColor::Yellow;
	m_Text = FText::FromString(TEXT("CullDistanceVolume"));
	m_pTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CullDistanceVolume"));
	if (m_pTextComponent)
	{
		m_pTextComponent->SetupAttachment(RootComponent);
		m_pTextComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
		m_pTextComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
		m_pTextComponent->bHiddenInGame = true;
		m_pTextComponent->bIsEditorOnly = true;
	}
}

//ゲーム開始時に1度だけ実行される関数
void ADebugCullDistanceVolume::BeginPlay()
{
	Super::BeginPlay();

}

//毎フレーム実行される関数
void ADebugCullDistanceVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR
void ADebugCullDistanceVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	AVolume::PostEditChangeProperty(PropertyChangedEvent);
	GetWorld()->bDoDelayedUpdateCullDistanceVolumes = true;
}

void ADebugCullDistanceVolume::PostEditMove(bool bFinished)
{
	AVolume::PostEditMove(bFinished);
	if (bFinished)
	{
		GetWorld()->bDoDelayedUpdateCullDistanceVolumes = true;
	}
}

void ADebugCullDistanceVolume::Destroyed()
{
	AVolume::Destroyed();
	if (GIsEditor)
	{
		UWorld* World = GetWorld();
		if (World && !World->IsGameWorld())
		{
			World->bDoDelayedUpdateCullDistanceVolumes = true;
		}
	}
}

void ADebugCullDistanceVolume::OnConstruction(const FTransform& Transform)
{
	if (!m_pTextComponent) { return; }

	//ボリュームのバウンスサイズを取得
	const FBoxSphereBounds& Bounds = GetBounds();
	//ボリュームの大きさに合わせて3Dテキストの大きさを調整
	m_pTextComponent->SetWorldSize(Bounds.BoxExtent.Size() * 0.2f);
	//3Dテキストがボリュームの上に表示されるように配置
	const float Height = Bounds.BoxExtent.Z * 1.5f;
	m_pTextComponent->SetRelativeLocation(FVector(0.f, 0.f, Height));
	//BoxShapeの色をテキストカラーに変更
	m_pTextComponent->TextRenderColor = m_Color;
	BrushColor = m_Color;
	//表示するテキストを設定
	m_pTextComponent->SetText(m_Text);
}
#endif // WITH_EDITOR