// Fill out your copyright notice in the Description page of Project Settings.

//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "DroneBase.h"
#include "GhostDrone.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API AGhostDrone : public ADroneBase
{
	GENERATED_BODY()
	
public:
	//�R���X�g���N�^
	AGhostDrone();
protected:
	//�Q�[���J�n����1�x��������
	virtual void BeginPlay() override;

public:
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//���[�X�̍��W�t�@�C���ǂݍ���
	void LoadingRaceVectorFile();

	//���[�X�̃N�I�[�^�j�I���t�@�C���ǂݍ���
	void LoadingRaceQuaternionFile();

private:
	//��]����
	void UpdateRotation(const float& DeltaTime);

	//�ړ�����
	void UpdateSpeed(const float& DeltaTime)override;

private:
	UPROPERTY(EditAnywhere, Category = "Drone")
		int PlaybackFlame;

	UPROPERTY(EditAnywhere, Category = "Drone")
		int m_PlayableFramesNum;
	UPROPERTY(EditAnywhere, Category = "Drone")
		float Time;

	UPROPERTY(EditAnywhere, Category = "Drone")
		int FlameCnt;

	UPROPERTY(EditAnywhere, Category = "Ghost")
		FVector m_LoadVelocity;														
	UPROPERTY(EditAnywhere, Category = "Ghost")
		FVector4 m_LoadQuat;
};
