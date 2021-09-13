 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

//�O���錾
class UStaticMeshComponent;
class UBoxComponent;

#define DEBUG_IsWithinRangeOfCheckpoint

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
	//���ʔԍ��擾
	FORCEINLINE int GetNumber()const { return m_CheckNumber; }
	//���������ԍ��ɂ��邩�̃t���O�擾
	FORCEINLINE int GetIsSameNumberNext()const { return m_bSameNumberNext; }
	//�ʉߔ���t���O���擾
	FORCEINLINE bool GetIsPassed()const { return m_bPassed; }
	//�͈͓��Ƀh���[�������邩�ǂ����̔���擾
	FORCEINLINE bool GetIsDroneInRange()const { return m_bIsDroneInRange; }
	//���ʔԍ��ݒ�
	FORCEINLINE void SetNumber(const int _number) { m_CheckNumber = _number; }
	//�`�F�b�N�|�C���g�𖈃t���[���X�V���邩�ǂ�����ݒ�
	FORCEINLINE void SetEveryUpdate(const bool& _bEveryUpdate) { PrimaryActorTick.bCanEverTick = _bEveryUpdate; }
private:
	//�͈͓��̃h���[����T������
	void FindDroneInRange();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pGateMesh;					//�`�F�b�N�|�C���g�̃��f�����b�V��
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pCheckPointCollision;				//�`�F�b�N�|�C���g�̒ʉߔ���p�R���W����
	UPROPERTY(VisibleAnywhere)
		int m_CheckNumber;												//���ʔԍ�
	UPROPERTY(EditAnywhere)
		bool m_bSameNumberNext;									//���̗v�f�������ԍ��ɂ��邩�ǂ���
	UPROPERTY(VisibleAnywhere)
		bool m_bPassed;													//�ʉߍς݂ǂ����̃t���O
	UPROPERTY(VisibleAnywhere)
		bool m_bIsDroneInRange;										//�͈͓��Ƀh���[�������邩���肷��t���O
	UPROPERTY(EditAnywhere)
		float m_RadiusOfSearchRange;								//��������͈�(���a)

};
