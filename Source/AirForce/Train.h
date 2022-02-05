// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Train.generated.h"

//�O���錾
class UStaticMesh;
class UStaticMeshComponent;
class ASplineActor;

UCLASS()
class AIRFORCE_API ATrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//���b�V���̏�����
	void InitializeMesh();
	//���x�X�V����
	void UpdateSpeed(const float& DeltaTime);
	//�ړ��X�V����
	void UpdateMove(const float& DeltaTime);
	//��]�X�V����
	void UpdateRotation(const float& DeltaTime);
	//�X�v���C���̏I�_�ɓ������Ă��邩�m�F���鏈��
	void CheckMoveDistance();

public:
	//������
	void Init();

private:
	UPROPERTY(EditAnywhere)
		TArray<UStaticMesh*> m_pMeshes;										//�d�Ԃ̃��b�V��
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pFrontTrainMesh;
	UPROPERTY(VisibleAnywhere)
		TArray<UStaticMeshComponent*> m_pTrainMeshes;				//�d�Ԃ̃��b�V��
	UPROPERTY(EditAnywhere)
		ASplineActor* m_pSplineActor;												//�d�Ԃ̈ړ��Ɏg���X�v���C�������A�N�^�[
	UPROPERTY(EditAnywhere)
		float m_MaxSpeed;																	//�ō����x
	UPROPERTY(VisibleAnywhere)
		float m_CurrentSpeed;															//���݂̑��x
	UPROPERTY(EditAnywhere)
		float m_Acceleration;																//�����x
	UPROPERTY(EditAnywhere)
		float m_Deceleration;																//�����x
	UPROPERTY(VisibleAnywhere)
		float m_MoveDistance;															//�d�Ԃ��X�v���C���̊J�n�_����i�񂾋���
	UPROPERTY(EditAnywhere)
		bool m_bLoop;																		//�X�v���C���̏I�_�ɓ���������n�_���烋�[�v���邩�ǂ���
	UPROPERTY(EditAnywhere)
		bool m_bCanMove;																	//�ړ��\�t���O
};
