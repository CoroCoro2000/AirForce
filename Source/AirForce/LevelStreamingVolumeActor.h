// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamingVolumeActor.generated.h"

//�O���錾
class UBoxComponent;

UCLASS()
class AIRFORCE_API ALevelStreamingVolumeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamingVolumeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//�I�[�o�[���b�v�����u�ԌĂяo�����֐�
	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//�I�[�o�[���b�v���Ă����R���|�[�l���g�����ꂽ�u�ԌĂяo�����֐�
	UFUNCTION()
		virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//�X�g���[�~���O��Box������擾
	UFUNCTION(BlueprintCallable)
		UBoxComponent* GetStreamingVolume()const { return m_pStreamingVolume; }

private:
	//���[�h������֐�
	void LoadStream();
	//�A�����[�h������֐�
	void UnloadStream();
	//���[�h�������Ɏ��s����֐�
	UFUNCTION()
		void LoadCompleted();

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pStreamingVolume;						//�X�g���[�~���O���J�n����g���K�[
	UPROPERTY(EditAnywhere)
		FName m_LevelName;													//�X�g���[�~���O���[�h���郌�x���̖��O

#if WITH_EDITOR
	float m_LoadStartTime;
	float m_UnloadStartTime;
	bool m_bLoad;
#endif // WITH_EDITOR

};
