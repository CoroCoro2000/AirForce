// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingD_GameInstance.generated.h"

class FLoadingScreenSystem;

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

public:
//���x���R���g���[���֐�
//-----------------------------------------------------------
//���x�����[�h�������ɌĂяo�����֐�
	UFUNCTION()
		void Completed() { m_bLoadComplete = true; }
	//���x���̃��[�h����
	void LoadLevel(const FName& _level);
	//���x���̃A�����[�h����
	void UnloadLevel(const FName& _level);
	//���x���̕\������
	bool ShowLevel(const FName& _level) const;
	//���x���̔�\������
	bool HideLevel(const FName& _level) const;
	//���[�h/�A�����[�h���I���������m�F����֐�
	bool IsCompleted() const { return m_bLoadComplete; }
	//-----------------------------------------------------------
public:
	TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem;

private:
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		FLatentActionInfo m_LatentAction;
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		bool m_bLoadComplete;
};
