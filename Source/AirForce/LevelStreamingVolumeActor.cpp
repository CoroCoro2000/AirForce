// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamingVolumeActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RacingD_GameInstance.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif //WITH_EDITOR

// Sets default values
ALevelStreamingVolumeActor::ALevelStreamingVolumeActor()
	: m_pStreamingVolume(NULL)
	, m_LevelName(TEXT("None"))
#if WITH_EDITOR
	, m_LoadStartTime(0.f)
	, m_UnloadStartTime(0.f)
	, m_bLoad(false)
#endif // WITH_EDITOR
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

//ロードをする関数
void ALevelStreamingVolumeActor::LoadStream()
{
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		const int32 UUID = pGameInstance->GenerateUUID();
		if (TSharedPtr<FLatentActionInfo> pLatentAction = MakeShareable(new FLatentActionInfo(1, UUID, TEXT("LoadCompleted"), this)))
		{
			UGameplayStatics::LoadStreamLevel(this, m_LevelName, true, false, *pLatentAction);

#if WITH_EDITOR
			if (UWorld* pWorld = GetWorld())
			{
				m_bLoad = true;
				m_LoadStartTime = GetWorld()->GetTimeSeconds();
				const FString DebugText = TEXT("Load::UUID[") + FString::FromInt(UUID) + TEXT("]");
				UKismetSystemLibrary::PrintString(this, DebugText);
			}
#endif // WITH_EDITOR
		}
	}
}

//アンロードをする関数
void ALevelStreamingVolumeActor::UnloadStream()
{
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		const int32 UUID = pGameInstance->GenerateUUID();
		if (TSharedPtr<FLatentActionInfo> pLatentAction = MakeShareable(new FLatentActionInfo(1, UUID, TEXT("LoadCompleted"), this)))
		{
			UGameplayStatics::UnloadStreamLevel(this, m_LevelName, *pLatentAction, false);

#if WITH_EDITOR
			if (UWorld* pWorld = GetWorld())
			{
				m_UnloadStartTime = GetWorld()->GetTimeSeconds();
				const FString DebugText = TEXT("Unload::UUID[") + FString::FromInt(UUID) + TEXT("]");
				UKismetSystemLibrary::PrintString(this, DebugText);
			}
#endif // WITH_EDITOR
		}
	}
}

//ロード完了時に実行する関数
void ALevelStreamingVolumeActor::LoadCompleted()
{
	//UUIDを削除
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		pGameInstance->DeleteUUID();

#if WITH_EDITOR
		if (UWorld* pWorld = GetWorld())
		{
			//ロード完了までにかかった時間を表示
			const float LoadTime = pWorld->GetTimeSeconds() - (m_bLoad ? m_LoadStartTime : m_UnloadStartTime);
			const FString LoadTimeText = TEXT("LoadTime::[") + FString::SanitizeFloat(LoadTime) + TEXT(" second]");

			FLinearColor TextColor = (LoadTime < 1.f ?
				FLinearColor(0.f, 0.6600000262f, 1.f, 1.f) :
				FLinearColor::Red);

			UKismetSystemLibrary::PrintString(this, LoadTimeText, true, false, TextColor);
			m_bLoad = false;
		}
#endif // WITH_EDITOR
	}
}

//オーバーラップした瞬間呼び出される関数
void ALevelStreamingVolumeActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//オーバーラップしたアクターがPlayerなら指定された名前のサブレベルをロード、表示
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			LoadStream();
		}
	}
}

//オーバーラップしていたコンポーネントが離れた瞬間呼び出される関数
void ALevelStreamingVolumeActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this)
	{
		//オーバーラップしたアクターがPlayerなら指定された名前のサブレベルをロード、表示
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			UnloadStream();
		}
	}
}