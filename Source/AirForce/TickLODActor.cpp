//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:TickLODActor.cpp
// �T�v				:�A�N�^�[��TickLOD���x����ύX����
// �쐬��			:2022/03/09
// �쐬��			:19CU0105 �r������
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

