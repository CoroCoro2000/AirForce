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

private:
	//�X�g���[�~���O���[�h�ɗ��p����UUID���擾
	int32 GetStreamingLevelNum()const;
	//�T�u���x���̕\��������֐�
	UFUNCTION()
		void ShowLevel();
	//�T�u���x�����\���ɂ���֐�
	UFUNCTION()
		void HideLevel();

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pStreamingVolume;						//�X�g���[�~���O���J�n����g���K�[
	UPROPERTY(EditAnywhere)
		TArray<FName> m_LoadLevelNames;								//���[�h�A�\�����郌�x���̖��O
	UPROPERTY(EditAnywhere)
		FName m_UnloadLevelName;											//�A�����[�h�A��\���ɂ��郌�x���̖��O
	UPROPERTY(VisibleAnywhere)
		int m_LoadIndex;															//���[�h����}�b�v�̔ԍ�
};
