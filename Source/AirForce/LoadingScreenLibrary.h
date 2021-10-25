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
	virtual TStatId GetStatId() const  override{ RETURN_QUICK_DECLARE_CYCLE_STAT(FSimpleLoadingScreenSystem, STATGROUP_Tickables); }
	//���t���[�����s�����֐�
	virtual void Tick(float DeltaTime) override;
	//�ǂݍ��݂�����p�b�P�[�W���̐ݒ�
	void SetPackageNameForLoadingProgress(FName InPackageName) { m_PackageName = InPackageName; }
	//���[�h�̐i�����擾����֐�
	float GetLoadingProgress();
	//���[�h�J�n���ɌĂяo�����֐�
	void OnBeginLoadingScreen(const FString& MapName);
	//���[�h�������ɌĂяo�����֐�
	void OnEndLoadingScreen(UWorld* world);
	//���[�h�̍X�V����
	void OnAsyncLoadingFlushUpdate();

private:
	URacingD_GameInstance* m_pGameInstance;								//�Q�[���C���X�^���X
	TSharedPtr<SWidget>	m_pLoadingScreenWidget;							//���[�h���ɕ\������X���[�g�E�B�W�F�b�g
	bool m_bLoading;													//���[�h�����ǂ���
	double m_LastTickTime;												//�Ō�Ɏ��s���ꂽTick�̏�������
	FName m_PackageName;												//���[�h����p�b�P�[�W��(path)
	float m_Progress;													//���[�h�̐i�s��
};


/**
 *		//Blueprint library
 */
UCLASS(meta = (ScriptName = "LoadingScreen"))
class AIRFORCE_API ULoadingScreenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer);
public:
	//���[�h�̐i�s�󋵂��󂯎��p�b�P�[�W����ݒ�
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetTargetPackageForLoadingProgress(const UObject* WorldContextObject, FName InPackageName);

	/** 0~100 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static float GetLoadingProgress(const UObject* WorldContextObject);

	//���x���̃��[�h��ɊJ���֐�
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void LoadAndOpenLevel(const UObject* WorldContextObject, const FName PersistentLevelName, const bool bAbsolute = true, const FString Options = FString(TEXT("")));
};
