// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingD_GameInstance.generated.h"

//�O���錾
class AGameManager;								//�Q�[���}�l�[�W���[�N���X

//���s���x���̗�
UENUM(BlueprintType)
namespace ECURRENT_LEVEL
{
	enum Type
	{
		TITLE						UMETA(DisplayName = "Title"),
		SELECT						UMETA(DisplayName = "Select"),
		RACE							UMETA(DisplayName = "Race"),
		RESULT						UMETA(DisplayName = "	Result"),
		NUM							UMETA(Hidden),
	};
}

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
private:
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		FLatentActionInfo m_LatentAction;
	UPROPERTY(VisibleAnywhere, Category = "LevelController")
		bool m_bLoadComplete;
};
