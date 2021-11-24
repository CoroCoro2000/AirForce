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

	//�g���K�[�R���|�[�l���g����
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
	
	//�g���K�[�ڐG���̃C�x���g�֐���o�^
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

//�I�[�o�[���b�v�����u�ԌĂяo�����֐�
void ALevelStreamingVolumeActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�I�[�o�[���b�v�����A�N�^�[��Player�Ȃ�w�肳�ꂽ���O�̃T�u���x�������[�h�A�\��
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

//�I�[�o�[���b�v���Ă����R���|�[�l���g�����ꂽ�u�ԌĂяo�����֐�
void ALevelStreamingVolumeActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this)
	{
		//�I�[�o�[���b�v�����A�N�^�[��Player�Ȃ�w�肳�ꂽ���O�̃T�u���x�����A�����[�h�A��\��
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