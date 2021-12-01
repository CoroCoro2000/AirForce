// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pTrainMesh(NULL)
	, m_pSplineActor(NULL)
	, m_MaxSpeed(50.f)
	, m_CurrentSpeed(0.f)
	, m_Acceleration(3.f)
	, m_Deceleration(5.f)
	, m_MoveDistance(0.f)
	, m_bLoop(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//メッシュ生成
	m_pTrainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TrainMesh"));
	if (m_pTrainMesh)
	{
		RootComponent = m_pTrainMesh;
	}
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pSplineActor)
	{
		if (USplineComponent* pSpline = m_pSplineActor->GetSpline())
		{
			
		}
	}
}

// Called every frame
void ATrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//速度更新処理
	UpdateSpeed(DeltaTime);

	//移動更新処理
	UpdateMove(DeltaTime);
}

//速度の更新
void ATrain::UpdateSpeed(const float& DeltaTime)
{
	m_CurrentSpeed = FMath::Lerp(m_CurrentSpeed, m_MaxSpeed, DeltaTime * m_Acceleration);
}

//移動の更新
void ATrain::UpdateMove(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	
	//進んだ距離を更新
	float Speed = m_CurrentSpeed * DeltaTime;
	m_MoveDistance += Speed;

	//スプラインの座標を取得
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, m_bLoop);

	//座標に移動
	SetActorLocation(NewLocation, true);
}

//回転更新処理
void ATrain::UpdateRotation(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	
	//スプラインの座標を取得
	FRotator NewRotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance, m_bLoop);

	//座標に移動
	SetActorRotation(NewRotation);
}