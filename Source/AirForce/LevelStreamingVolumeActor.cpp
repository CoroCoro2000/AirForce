// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamingVolumeActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"

// Sets default values
ALevelStreamingVolumeActor::ALevelStreamingVolumeActor()
	: m_pStreamingVolume(NULL)
	, m_LoadLevelNames()
	, m_UnloadLevelName(TEXT("None"))
	, m_LoadIndex(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//トリガーコンポーネント生成
	m_pStreamingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("StreamingVolume"));
	if (m_pStreamingVolume)
	{
		RootComponent = m_pStreamingVolume;
	}
}

// Called when the game starts or when spawned
void ALevelStreamingVolumeActor::BeginPlay()
{
	Super::BeginPlay();
	
	//トリガー接触時のイベント関数を登録
	if (m_pStreamingVolume)
	{
		m_pStreamingVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamingVolumeActor::OnComponentBeginOverlap);
		m_pStreamingVolume->OnComponentEndOverlap.AddDynamic(this, &ALevelStreamingVolumeActor::OnComponentEndOverlap);
	}
}

// Called every frame
void ALevelStreamingVolumeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//ストリーミングロード中のレベル数を取得
int32 ALevelStreamingVolumeActor::GetStreamingLevelNum()const
{
	int32 StreamingLevelNum = -1;
	if (UWorld* pWorld = GetWorld())
	{
		StreamingLevelNum = pWorld->GetStreamingLevels().Num();
	}
	return StreamingLevelNum;
}

//ロードが終わったとき呼び出す関数
void ALevelStreamingVolumeActor::ShowLevel()
{
	if (m_LoadLevelNames.IsValidIndex(m_LoadIndex))
	{
		if (ULevelStreaming* pLevelStreaming = UGameplayStatics::GetStreamingLevel(this, m_LoadLevelNames[m_LoadIndex]))
		{
			if (pLevelStreaming->IsLevelVisible() == false)
			{
				pLevelStreaming->SetShouldBeVisible(true);
				++m_LoadIndex;

#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("ShowLevel()"));
#endif // WITH_EDITOR
			}
		}
	}
}

//サブレベルを非表示にする関数
void ALevelStreamingVolumeActor::HideLevel()
{
	if (ULevelStreaming* pLevelStreaming = UGameplayStatics::GetStreamingLevel(this, m_UnloadLevelName))
	{
		if (pLevelStreaming->IsLevelVisible())
		{
			pLevelStreaming->SetShouldBeVisible(false);

#if WITH_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("HideLevel()"));
#endif // WITH_EDITOR
		}
	}
}

//オーバーラップした瞬間呼び出される関数
void ALevelStreamingVolumeActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//オーバーラップしたアクターがドローンなら隣のサブレベルをロード、表示
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			for (const FName LoadLevelName : m_LoadLevelNames)
			{
				int32 UUID = GetStreamingLevelNum();
				FLatentActionInfo LatentAction(-1, UUID, TEXT(""), this);
				UGameplayStatics::LoadStreamLevel(this, LoadLevelName, true, false, LatentAction);

#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("UUID[%d]"), UUID);
#endif // WITH_EDITOR
			}
		}
	}
}

//オーバーラップしていたコンポーネントが離れた瞬間呼び出される関数
void ALevelStreamingVolumeActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (m_UnloadLevelName != TEXT("None"))
	{
		if (OtherActor && OtherActor != this)
		{
			//オーバーラップしたアクターがドローンなら隣のサブレベルをアンロード、非表示
			if (OtherActor->ActorHasTag(TEXT("Drone")))
			{
				int32 UUID = GetStreamingLevelNum();
				FLatentActionInfo LatentAction(-1, UUID, TEXT(""), this);
				UGameplayStatics::UnloadStreamLevel(this, m_UnloadLevelName, LatentAction, false);

#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("UUID[%d]"), UUID);
#endif // WITH_EDITOR
			}
		}
	}
}