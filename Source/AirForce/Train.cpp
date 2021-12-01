// Fill out your copyright notice in the Description page of Project Settings.


#include "Train.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.h"

// Sets default values
ATrain::ATrain()
	: m_pTrainMesh(CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("TrainMesh")))
	, m_pSplineActor(NULL)
	, m_MaxSpeed(500.f)
	, m_CurrentSpeed(0.f)
	, m_Acceleration(3.f)
	, m_Deceleration(5.f)
	, m_MoveDistance(0.f)
	, m_bLoop(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//メッシュのアタッチ
	if (m_pTrainMesh)
	{
		RootComponent = m_pTrainMesh;
	}
}

// Called when the game starts or when spawned
void ATrain::BeginPlay()
{
	Super::BeginPlay();
	
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
	UE_LOG(LogTemp, Warning, TEXT("Speed[%f]"), m_CurrentSpeed);
}

//移動の更新
void ATrain::UpdateMove(const float& DeltaTime)
{
	if (!m_pSplineActor) { return; }
	if (!m_pTrainMesh) { return; }
	
	//進んだ距離を更新
	float Speed = m_CurrentSpeed * DeltaTime;
	m_MoveDistance += Speed;

	//スプラインの座標を取得
	FVector NewLocation = m_pSplineActor->GetCurrentLocation(m_MoveDistance, m_bLoop);

	//座標に移動
	m_pTrainMesh->SetBoneLocationByName(TEXT("Front"), NewLocation, EBoneSpaces::WorldSpace);
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

	//1両目から2両目のボーンの距離を求める
	float Dist1 = FVector::Dist(FrontBoneLocation, Joint1BoneLocation);
	float Dist2 = Dist1 + FVector::Dist(Joint1BoneLocation, Joint2BoneLocation);

	//各ボーンの回転量を求める
	FRotator FrontRotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance, m_bLoop);
	FRotator Joint1Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - Dist1, m_bLoop);
	FRotator Joint2Rotation = m_pSplineActor->GetCurrentRotation(m_MoveDistance - Dist2, m_bLoop);

	//各ボーンを回転させる
	m_pTrainMesh->SetBoneRotationByName(TEXT("Front"), FrontRotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint1"), Joint1Rotation, EBoneSpaces::WorldSpace);
	m_pTrainMesh->SetBoneRotationByName(TEXT("joint2"), Joint2Rotation, EBoneSpaces::WorldSpace);
}