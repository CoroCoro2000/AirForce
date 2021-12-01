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

//�i�񂾋�������X�v���C���̍��W���擾
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

		//�X�v���C���̒����̈ʒu���擾
		Location = m_pSpline->GetLocationAtDistanceAlongSpline(LocalLength, ESplineCoordinateSpace::World);
	}
	return Location;
}

//�i�񂾋�������X�v���C���̉�]���擾
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

		//�X�v���C���̒����̉�]���擾
		Rotation = m_pSpline->GetRotationAtDistanceAlongSpline(LocalLength, ESplineCoordinateSpace::World);
	}
	return Rotation;
}

