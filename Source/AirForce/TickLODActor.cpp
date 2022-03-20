//------------------------------------------------------------------------------------------------------------------------------------------------
// ファイル名		:TickLODActor.cpp
// 概要				:1フレーム当たりのTick更新回数を変更できるアクターのベースクラス
// 作成日			:2022/03/09
// 作成者			:19CU0105 池村凌太
//------------------------------------------------------------------------------------------------------------------------------------------------

#include "TickLODActor.h"
#include "ConfigParameter.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR


// Sets default values
ATickLODActor::ATickLODActor()
	: m_TickFPS(60.f)
	, m_LastTickTime(0.f)
	, m_ActorNumber(0)
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

//処理可能なフレームか判定
bool ATickLODActor::IsProcessableFrame(const float& currentTime)const
{
	//このアクターの番号が奇数なら奇数フレームの時のみ処理できる
	//偶数なら偶数フレームの時だけ処理できる
	//経過した時間から現在何フレーム目か推測する
	bool isEvenFrame = FMath::RoundToInt(currentTime * CConfigParameter::FixedFrameRate) & 1;

	return (m_ActorNumber & 1) ? isEvenFrame : !isEvenFrame;
}