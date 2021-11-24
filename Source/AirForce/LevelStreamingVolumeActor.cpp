// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamingVolumeActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelStreamingVolumeActor::ALevelStreamingVolumeActor()
	: m_pStreamingVolume(NULL)
	, m_LoadLevelNames()
	, m_UnloadLevelNames()
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

//オーバーラップした瞬間呼び出される関数
void ALevelStreamingVolumeActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//オーバーラップしたアクターがPlayerなら指定された名前のサブレベルをロード、表示
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			int32 UUID = m_LoadLevelNames.Num() + m_UnloadLevelNames.Num();
			for (const FName LoadLevelName : m_LoadLevelNames)
			{
				FLatentActionInfo LatentAction(-1, UUID, TEXT(""), NULL);
				UUID++;
				UGameplayStatics::LoadStreamLevel(this, LoadLevelName, true, false, LatentAction);

#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("Load::UUID[%d]"), UUID);
#endif // WITH_EDITOR
			}
		}
	}
}

//オーバーラップしていたコンポーネントが離れた瞬間呼び出される関数
void ALevelStreamingVolumeActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this)
	{
		//オーバーラップしたアクターがPlayerなら指定された名前のサブレベルをアンロード、非表示
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			int32 UUID = m_LoadLevelNames.Num() - m_UnloadLevelNames.Num();
			for (const FName LoadLevelName : m_UnloadLevelNames)
			{
				FLatentActionInfo LatentAction(-1, UUID, TEXT(""), NULL);
				UUID++;
				UGameplayStatics::UnloadStreamLevel(this, LoadLevelName, LatentAction, false);

#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("Unload::UUID[%d]"), UUID);
#endif // WITH_EDITOR
			}
		}
	}
}