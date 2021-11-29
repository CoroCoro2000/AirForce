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

//�Q�[���J�n����1�x�������s�����֐�
void ADebugCullDistanceVolume::BeginPlay()
{
	Super::BeginPlay();

}

//���t���[�����s�����֐�
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

	//�{�����[���̃o�E���X�T�C�Y���擾
	const FBoxSphereBounds& Bounds = GetBounds();
	//�{�����[���̑傫���ɍ��킹��3D�e�L�X�g�̑傫���𒲐�
	m_pTextComponent->SetWorldSize(Bounds.BoxExtent.Size() * 0.2f);
	//3D�e�L�X�g���{�����[���̏�ɕ\�������悤�ɔz�u
	const float Height = Bounds.BoxExtent.Z * 1.5f;
	m_pTextComponent->SetRelativeLocation(FVector(0.f, 0.f, Height));
	//BoxShape�̐F���e�L�X�g�J���[�ɕύX
	m_pTextComponent->TextRenderColor = m_Color;
	BrushColor = m_Color;
	//�\������e�L�X�g��ݒ�
	m_pTextComponent->SetText(m_Text);
}
#endif // WITH_EDITOR