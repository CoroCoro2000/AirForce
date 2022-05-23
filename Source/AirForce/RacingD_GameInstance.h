// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;
class USaveRecord;

/**
 * 
 */
UCLASS()
class AIRFORCE_API URacingD_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	URacingD_GameInstance();

private:
	//�Q�[���N�����Ɏ��s�����֐�
	virtual void Init() override;
	//�Q�[���I�����Ɏ��s�����֐�
	virtual void Shutdown() override;

public:
	//�Q�[���C���X�^���X�̎擾
	UFUNCTION(BlueprintCallable)
	static URacingD_GameInstance* Get();
	//���[�h�X�N���[���p�̃V�X�e�����擾
	TSharedPtr<FLoadingScreenSystem> GetLoadingScreenSystem()const { return m_pLoadingScreenSystem; }

private:
	TSharedPtr<FLoadingScreenSystem> m_pLoadingScreenSystem;						//���[�h�X�N���[���V�X�e��
};