// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;
class USaveRecord;
class UAsyncActionHandleSaveGame;

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
	//�Q�[���f�[�^�̃��[�h
	void LoadGameData(const FString& SlotName, const int32& SlotIndex);
	//�Q�[���f�[�^�̔񓯊����[�h(C++)
	void AsyncLoadGameData(const FString& SlotName, const int32 SlotIndex, FAsyncLoadGameFromSlotDelegate Callback);
	//�Q�[���f�[�^�̔񓯊����[�h(C++)
	void AsyncLoadGameData(const FString& SlotName, const int32 SlotIndex);
	//�Q�[���f�[�^�̃Z�[�u
	void SaveGameData(const FString& SlotName, const int32& SlotIndex)const;
	//�Q�[���f�[�^�̔񓯊��Z�[�u(C++)
	void AsyncSaveGameData(const FString& SlotName, const int32 SlotIndex, FAsyncSaveGameToSlotDelegate Callback);
	//�Q�[���f�[�^�̔񓯊��Z�[�u(C++)
	void AsyncSaveGameData(const FString& SlotName, const int32 SlotIndex);
	//�Z�[�u�f�[�^�̐ݒ�
	void SetSeveRecord(USaveRecord* pSaveRecord){m_pSaveRecord = pSaveRecord;}
	//�Z�[�u�f�[�^�̎擾
	UFUNCTION(BlueprintCallable)
	USaveRecord* GetSaveRecord()const { return m_pSaveRecord; }
	//�Q�[���f�[�^�̕ۑ����擾
	UFUNCTION(BlueprintCallable)
	static FString GetSaveSlotName();

private:
	//�񓯊����[�h�������ɌĂ΂��R�[���o�b�N
	UFUNCTION()
	void LoadCompleted(const FString& SlotName, const int32 SlotIndex, USaveGame* LoadData);
	//�񓯊��Z�[�u�������ɌĂ΂��R�[���o�b�N
	UFUNCTION()
	void SaveCompleted(const FString& SavedSlotName, const int32 SavedSlotIndex, bool bSuccess);

private:
	TSharedPtr<FLoadingScreenSystem> m_pLoadingScreenSystem;						//���[�h�X�N���[���V�X�e��
	UPROPERTY(EditAnywhere)
		USaveRecord* m_pSaveRecord;													//�Z�[�u�f�[�^

public:
	static const FString SaveSlotName;												//�Z�[�u�p�̃X���b�g��
};