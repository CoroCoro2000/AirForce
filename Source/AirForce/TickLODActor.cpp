//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODActor.cpp
// 概要				:1フレーム当たりのTick更新回数を変更できるアクターのベースクラス
// 作成日			:2022/03/09
// 作成者			:19CU0105 池村凌太
//------------------------------------------------------------------------------------------------------------------------------------------------

#include "TickLODActor.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR


// Sets default values
ATickLODActor::ATickLODActor()
	: m_TickFPS(60.f)
	, m_LastTickTime(0.f)
	, m_ActorNumber(0)
	, m_FrameCount(0)
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

	//フレーム数を加算
	if (m_FrameCount < 10000)
	{
		++m_FrameCount;
	}
	else
	{
		m_FrameCount = 0.f;
	}
}

//処理可能なフレームか判定
bool ATickLODActor::IsProcessableFrame()const
{
	//このアクターの番号が奇数なら奇数フレームの時のみ処理できる
	//偶数なら偶数フレームの時だけ処理できる
	bool isEvenFrame = m_FrameCount & 1;

	return (m_ActorNumber & 1) ? isEvenFrame : !isEvenFrame;
}