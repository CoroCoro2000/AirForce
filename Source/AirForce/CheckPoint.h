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
	//�`�F�b�N�|�C���g�͈͓̔��ɂ��邩�ǂ�������
	FORCEINLINE bool IsWithinRangeOfCheckpoint()const;
	//���ʔԍ��擾
	FORCEINLINE int GetNumber()const { return m_CheckNumber; }
	//���������ԍ��ɂ��邩�̃t���O�擾
	FORCEINLINE int GetIsSameNumberNext()const { return m_bSameNumberNext; }
	//�ʉߔ���t���O���擾
	FORCEINLINE bool GetIsPassed()const { return m_bPassed; }
	//���ʔԍ��ݒ�
	FORCEINLINE void SetNumber(const int _number) { m_CheckNumber = _number; }
	//�`�F�b�N�|�C���g���A�N�e�B�u������
	FORCEINLINE void SetActive(const bool& _isActive) { m_bActive = _isActive; }

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pGateMesh;					//�`�F�b�N�|�C���g�̃��f�����b�V��
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pCheckPointCollision;				//�`�F�b�N�|�C���g�̒ʉߔ���p�R���W����
	UPROPERTY(VisibleAnywhere)
		int m_CheckNumber;												//���ʔԍ�
	UPROPERTY(VisibleAnywhere)
		bool m_bSameNumberNext;									//���̗v�f�������ԍ��ɂ��邩�ǂ���
	UPROPERTY(VisibleAnywhere)
		bool m_bPassed;													//�ʉߍς݂ǂ����̃t���O
	UPROPERTY(VisibleAnywhere)
		bool m_bActive;														//�A�N�e�B�u��Ԃ��ǂ���
	UPROPERTY(EditAnywhere)
		float m_RadiusOfSearchRange;								//��������͈�(���a)
};
