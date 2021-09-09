 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

//�O���錾
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class AIRFORCE_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//�I�[�o�[���b�v�J�n���ɌĂяo�����C�x���g�֐�
	UFUNCTION()
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//�ʉߔ���t���O���擾
	FORCEINLINE bool GetIsPassed()const { return m_bPassed; }

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pGateMesh;					//�`�F�b�N�|�C���g�̃��f�����b�V��
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pCheckPointCollision;				//�`�F�b�N�|�C���g�̒ʉߔ���p�R���W����
	UPROPERTY(VisibleAnywhere)
		bool m_bPassed;													//�ʉߍς݂ǂ����̃t���O
};
