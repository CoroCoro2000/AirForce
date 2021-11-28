// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugLevelStreamingVolume.h"

#if WITH_EDITOR
#include "Components/TextRenderComponent.h"
#include "Engine/LevelStreaming.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"
#define LOCTEXT_NAMESPACE "ErrorChecking"
#endif // WITH_EDITOR

//コンストラクタ
ADebugLevelStreamingVolume::ADebugLevelStreamingVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#if WITH_EDITOR
	, m_Color(FColor::Blue)
	, m_Text(FText::FromString(TEXT("StreamingVolume")))
	, m_pTextComponent(CreateDefaultSubobject<UTextRenderComponent>(TEXT("StreamingLevelNumber")))
#endif//WITH_EDITOR
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITOR
	if (m_pTextComponent)
	{
		m_pTextComponent->SetupAttachment(RootComponent);
		m_pTextComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
		m_pTextComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
		m_pTextComponent->bHiddenInGame = true;
		m_pTextComponent->bIsEditorOnly = true;
	}
#endif // WITH_EDITOR
}

//ゲーム開始時に1度だけ実行される関数
void ADebugLevelStreamingVolume::BeginPlay()
{
	Super::BeginPlay();

}

//毎フレーム実行される関数
void ADebugLevelStreamingVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADebugLevelStreamingVolume::PostLoad()
{
	AVolume::PostLoad();

#if WITH_EDITOR
	if (GIsEditor)
	{
		UpdateStreamingLevelsRefs();
	}
#endif//WITH_EDITOR
}

#if WITH_EDITOR
void ADebugLevelStreamingVolume::CheckForErrors()
{
	AVolume::CheckForErrors();

	// Streaming level volumes are not permitted outside the persistent level.
	if (GetLevel() != GetWorld()->PersistentLevel)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::Format(LOCTEXT("MapCheck_Message_LevelStreamingVolume", "{ActorName} : LevelStreamingVolume is not in the persistent level - please delete"), Arguments)))
			->AddToken(FMapErrorToken::Create(FMapErrors::LevelStreamingVolume));
	}

	// Warn if the volume has no streaming levels associated with it
	bool bHasAssociatedLevels = false;
	for (ULevelStreaming* LevelStreaming : GetWorld()->GetStreamingLevels())
	{
		if (LevelStreaming && LevelStreaming->EditorStreamingVolumes.Contains(this))
		{
			bHasAssociatedLevels = true;
			break;
		}
	}

	if (!bHasAssociatedLevels)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::Format(LOCTEXT("MapCheck_Message_NoLevelsAssociatedWithStreamingVolume", "{ActorName} : No levels are associated with streaming volume."), Arguments)))
			->AddToken(FMapErrorToken::Create(FMapErrors::NoLevelsAssociated));
	}
}

void ADebugLevelStreamingVolume::UpdateStreamingLevelsRefs()
{
	StreamingLevelNames.Reset();

	UWorld* OwningWorld = GetWorld();
	if (OwningWorld)
	{
		for (ULevelStreaming* LevelStreaming : OwningWorld->GetStreamingLevels())
		{
			if (LevelStreaming && LevelStreaming->EditorStreamingVolumes.Find(this) != INDEX_NONE)
			{
				StreamingLevelNames.Add(LevelStreaming->GetWorldAssetPackageFName());
			}
		}
	}
}

void ADebugLevelStreamingVolume::OnConstruction(const FTransform& Transform)
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