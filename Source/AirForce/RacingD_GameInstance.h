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
		RACE						UMETA(DisplayName = "Race"),
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
	
	

private:
	AGameManager* m_pGameManager;					//���x�����Ƃ̃}�l�[�W���[�N���X
};
