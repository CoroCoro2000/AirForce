// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamingVolumeActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelStreamingVolumeActor::ALevelStreamingVolumeActor()
	: m_pStreamingVolume(NULL)
	, m_LoadLevelNames()
	, m_UnloadLevelName(TEXT("None"))
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

//�X�g���[�~���O���[�h���̃��x�������擾
int32 ALevelStreamingVolumeActor::GetStreamingLevelNum()const
{
	int32 StreamingLevelNum = -1;
	if (UWorld* pWorld = GetWorld())
	{
		StreamingLevelNum = pWorld->GetStreamingLevels().Num();
	}
	return StreamingLevelNum;
}

//�I�[�o�[���b�v�����u�ԌĂяo�����֐�
void ALevelStreamingVolumeActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�I�[�o�[���b�v�����A�N�^�[���h���[���Ȃ�ׂ̃T�u���x�������[�h�A�\��
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			for (const FName LoadLevelName : m_LoadLevelNames)
			{
				int32 UUID = GetStreamingLevelNum();
				FLatentActionInfo LatentAction(-1, UUID, TEXT(""), NULL);
				UGameplayStatics::LoadStreamLevel(this, LoadLevelName, true, false, LatentAction);
			}
		}
	}
}

//�I�[�o�[���b�v���Ă����R���|�[�l���g�����ꂽ�u�ԌĂяo�����֐�
void ALevelStreamingVolumeActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this)
	{
		//�I�[�o�[���b�v�����A�N�^�[���h���[���Ȃ�ׂ̃T�u���x�����A�����[�h�A��\��
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			int32 UUID = GetStreamingLevelNum();
			FLatentActionInfo LatentAction(-1, UUID, TEXT(""), NULL);
			UGameplayStatics::UnloadStreamLevel(this, m_UnloadLevelName, LatentAction, false);
		}
	}
}