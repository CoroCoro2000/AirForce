// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterBox.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWaterBox::AWaterBox()
	: m_pSurfaceMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Surface")))
	, m_pBackfaceMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backface")))
	, m_Size(FVector::OneVector)
	, m_BoundaryThickness(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//メッシュ初期化
	InitializeMesh();
}

// Called when the game starts or when spawned
void AWaterBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaterBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaterBox::OnConstruction(const FTransform& Transform)
{
	if (!m_pSurfaceMesh) { return; }
	if (!m_pBackfaceMesh) { return; }

	SetActorScale3D(FVector(m_Size, 1.f));
	m_pBackfaceMesh->SetRelativeLocation(FVector(0.f, 0.f, -m_BoundaryThickness));
}

//メッシュの初期化
void AWaterBox::InitializeMesh()
{
	if (!m_pSurfaceMesh) { return; }
	if (!m_pBackfaceMesh) { return; }

	RootComponent = m_pSurfaceMesh;
	m_pBackfaceMesh->AttachToComponent(m_pSurfaceMesh, FAttachmentTransformRules::KeepRelativeTransform);

	m_pBackfaceMesh->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));
}