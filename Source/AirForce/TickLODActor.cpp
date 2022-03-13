//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODActor.cpp
// 概要				:アクターのTickLODレベルを変更する
// 作成日			:2022/03/09
// 作成者			:19CU0105 池村凌太
//------------------------------------------------------------------------------------------------------------------------------------------------

#include "TickLODActor.h"

// Sets default values
ATickLODActor::ATickLODActor()
	: m_TickFPS(60.f)
	, m_LastTickTime(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATickLODActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATickLODActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

