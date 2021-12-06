// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pTrain(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Train")))
	, m_pTrainMesh(CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("TrainMesh")))
	, m_pSplineActor(NULL)
	, m_MaxSpeed(500.f)
	, m_CurrentSpeed(0.f)
	, m_Acceleration(3.f)
	, m_Deceleration(5.f)
	, m_MoveDistance(0.f)
	, m_bLoop(true)
	, m_FrontBoneRotation(FRotator::ZeroRotator)
	, m_Joint1BoneRotation(FRotator::ZeroRotator)
	, m_Joint2BoneRotation(FRotator::ZeroRotator)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//メッシュのアタッチ
	if (m_pTrain)
	{
		RootComponent = m_pTrain;
	}
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pTrainMesh)
	{
		m_pTrainMesh->CopyPoseFromSkeletalComponent(m_pTrain);
		m_FrontBoneRotation = m_pTrainMesh->GetBoneRotationByName(TEXT("Front"), EBoneSpaces::WorldSpace);
		m_FrontBoneRotation.Yaw -= 90.096451f;
		m_Joint1BoneRotation = m_pTrainMesh->GetBoneRotationByName(TEXT("joint1"), EBoneSpaces::WorldSpace);
		m_Joint1BoneRotation.Yaw -= 90.096451f;
		m_Joint2BoneRotation = m_pTrainMesh->GetBoneRotationByName(TEXT("joint2"), EBoneSpaces::WorldSpace);
		m_Joint2BoneRotation.Yaw -= 90.096451f;
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

	//回転更新処理
	UpdateRotation(DeltaTime);
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
	if (!m_pTrainMesh) { return; }

	//進んだ距離を更新
	float Speed = m_CurrentSpeed * DeltaTime;
	m_MoveDistance += Speed;

	if (m_bLoop)
	{
		if (m_pSplineActor->GetSpline()->GetSplineLength() <= m_MoveDistance)
		{
			m_MoveDistance = 0.f;
			//始点座標に移動
			FVector StartLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, false);
			SetActorLocation(StartLocation, true);
		}
	}

	//スプラインの座標を取得
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, false);
	//座標に移動
	SetActorLocation(NewLocation, true);
}

//回転更新処理
void ATrain::UpdateRotation(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (!m_pTrainMesh) { return; }
	
	//各車両のボーン座標を取得
	FVector FrontBoneLocation = m_pTrainMesh->GetBoneLocationByName(TEXT("Front"), EBoneSpaces::WorldSpace);
	FVector Joint1BoneLocation = m_pTrainMesh->GetBoneLocationByName(TEXT("joint1"), EBoneSpaces::WorldSpace);
	FVector Joint2BoneLocation = m_pTrainMesh->GetBoneLocationByName(TEXT("joint2"), EBoneSpaces::WorldSpace);

	//ボーンの距離を求める
	float Dist1 = FMath::Abs(FVector::Dist(FrontBoneLocation, Joint1BoneLocation));
	float Dist2 = FMath::Abs(FVector::Dist(Joint1BoneLocation, Joint2BoneLocation));

	//各ボーンの回転量を求める
	FRotator FrontRotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance, false);
	FRotator Joint1Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - Dist1, false);
	FRotator Joint2Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - (Dist1 + Dist2), false);

	//各ボーンを回転させる
	m_pTrainMesh->SetBoneRotationByName(TEXT("Front"), FrontRotation + m_FrontBoneRotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint1"), Joint1Rotation + m_Joint1BoneRotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint2"), Joint2Rotation + m_Joint2BoneRotation, EBoneSpaces::WorldSpace);
}