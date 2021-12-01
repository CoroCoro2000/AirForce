// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActor.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ASplineActor::ASplineActor()
	: m_pSpline(CreateDefaultSubobject<USplineComponent>(TEXT("Spline")))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (m_pSpline)
	{
		RootComponent = m_pSpline;
	}
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//進んだ距離からスプラインの座標を取得
FVector ASplineActor::GetCurrentLocation(const float& length, const bool& bLoop)const
{
	FVector Location = FVector::ZeroVector;
	if (m_pSpline)
	{
		float LocalLength = length;
		if (bLoop)
		{
			LocalLength = length / m_pSpline->GetSplineLength();
		}

		//スプラインの長さの位置を取得
		Location = m_pSpline->GetLocationAtDistanceAlongSpline(LocalLength, ESplineCoordinateSpace::World);
	}
	return Location;
}

//進んだ距離からスプラインの回転を取得
FRotator ASplineActor::GetCurrentRotation(const float& length, const bool& bLoop)const
{
	FRotator Rotation = FRotator::ZeroRotator;
	if (m_pSpline)
	{
		float LocalLength = length;
		if (bLoop)
		{
			LocalLength = length / m_pSpline->GetSplineLength();
		}

		//スプラインの長さの回転を取得
		Rotation = m_pSpline->GetRotationAtDistanceAlongSpline(LocalLength, ESplineCoordinateSpace::World);
	}
	return Rotation;
}

