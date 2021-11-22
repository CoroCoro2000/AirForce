// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tickable.h"
#include "LoadingScreenLibrary.generated.h"

class UUserWidget;
class SWidget;
class URacingD_GameInstance;
class FObjectInitializer;

/**
 *  Core class
 */
class FLoadingScreenSystem : public FTickableGameObject
{
public:
	//�R���X�g���N�^
	FLoadingScreenSystem(URacingD_GameInstance* InGameInstance);
	//�f�X�g���N�^
	~FLoadingScreenSystem();
	//CycleCounter��Ԃ��֐�
	virtual TStatId GetStatId() const  override{ RETURN_QUICK_DECLARE_CYCLE_STAT(FLoadingScreenSystem, STATGROUP_Tickables); }
	//���t���[�����s�����֐�
	virtual void Tick(float DeltaTime) override;
	//�ǂݍ��݂�����p�b�P�[�W���̐ݒ�
	void SetPackageNameForLoadingProgress(FName InPackageName) { m_PackageName = InPackageName; }
	//���[�h��ʂ̕\��
	void ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass);
	//���[�h��ʂ̔�\��
	void HideLoadingScreen();
	//���[�h�̐i�����擾����֐�
	float GetLoadingProgress();


private:
	//�X���[�g�̍X�V����
	void OnAsyncLoadingFlushUpdate();

private:
	URacingD_GameInstance* m_pGameInstance;								//�Q�[���C���X�^���X
	TSharedPtr<SWidget>	m_pLoadingScreenWidget;						//���[�h���ɕ\������X���[�g�E�B�W�F�b�g
	bool m_bShowing;																		//�E�B�W�F�b�g���\������Ă��邩
	double m_LastTickTime;																//�Ō�Ɏ��s���ꂽTick�̏�������
	FName m_PackageName;															//���[�h����p�b�P�[�W��(�p�X�w��)
	float m_Progress;																		//���[�h�̐i�s��
	FDelegateHandle m_AsyncLoadingFlushUpdateHandle;					//OnAsyncLoadingFlushUpdate�̃o�C���h�A�������s���f���Q�[�g�n���h��
};

/**
 *		//Blueprint library
 */
UCLASS(meta = (ScriptName = "LoadingScreen"))
class AIRFORCE_API ULoadingScreenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//�R���X�g���N�^
	ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer);

public:
	//���[�h��ʂ̕\��
	UFUNCTION(BlueprintCallable)
		static void ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass, FName InPackageName);
	//���[�h��ʂ̔�\��
	UFUNCTION(BlueprintCallable)
		static void HideLoadingScreen();
	//���[�h�̐i�s�󋵂��擾
	UFUNCTION(BlueprintCallable)
		static float GetLoadingProgress();

};
