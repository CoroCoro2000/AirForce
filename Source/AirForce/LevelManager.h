// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

namespace EINPUTKEY
{
	enum Type
	{
		EINPUTKEY_VERTICAL = 0		UMETA(DisPlayName = "VERTICAL"),		//�㉺����
		EINPUTKEY_HORIZONTAL 		UMETA(DisPlayName = "HORIZONTAL"),	//���E����
	};
}

UCLASS()
class AIRFORCE_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputLeft();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputRight();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputA();

	//���͉\�t���O�ݒ�
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetInputEnable(bool _flag) { m_isInputEnable = _flag; }
	//���͉\�t���O�擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetInputEnable() const { return m_isInputEnable; }

	//���ݑI���������x���ԍ��擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		int GetLevelIndex() const { return m_LevelIndex; }

	//���ݑI���������x�����擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		FName GetLevelName() const { return m_LevelName[m_LevelIndex]; }
private:
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isInputEnable;						//���͉\�t���O

	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<FName> m_LevelName;					//�J�ڂ��郌�x����
	UPROPERTY(VisibleAnywhere, Category = "Level")
		int m_LevelIndex;							//���ݑI�����Ă��郌�x���ԍ�
	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<int> m_LevelAddValue;				//�e���Ń��x���ԍ������Z�����
	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<int> m_LevelIndexMin;				//�e���Ń��x���ԍ������Z���鉺��
	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<int> m_LevelIndexMax;				//�e���Ń��x���ԍ������Z������
};
