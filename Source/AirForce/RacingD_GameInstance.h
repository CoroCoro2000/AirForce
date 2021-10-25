// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;
class UUserWidget;
class SWidget;

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
	virtual void Init() override;
	virtual void Shutdown() override;

	//���[�h���Ɏ��s�����֐����o�C���h
	void BindLoadingContent();

public:
	//�Q�[���C���X�^���X�̎擾
	static URacingD_GameInstance* Get();
	//���[�h�X�N���[���p�̃V�X�e�����擾
	TSharedPtr<FLoadingScreenSystem> GetLoadingScreenSystem()const { return m_pLoadingScreenSystem; }
	//���[�h�J�n���ɌĂяo���֐�
	void OnBeginLoadingScreen(const FString& MapName);
	//���[�h�I�����ɌĂяo���֐�
	void OnEndLoadingScreen(UWorld* world);

	//���[�h���ɕ\������E�B�W�F�b�g�N���X���擾
	TSubclassOf<UUserWidget> GetLoadingUMGClass()const { return m_LoadingUMGClass; }
protected:
	TSharedPtr<FLoadingScreenSystem> m_pLoadingScreenSystem;						//���[�h�X�N���[���V�X�e��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UUserWidget> m_LoadingUMGClass;										//���[�h���ɕ\������UMG�N���X
};
