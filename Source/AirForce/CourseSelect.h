// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CourseSelect.generated.h"

class UDataTable;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class AIRFORCE_API ACourseSelect : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACourseSelect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		int GetSelectCourseNumber()const { return m_CourseNumber; }

	//���͉\�t���O�ݒ�
	UFUNCTION(BlueprintCallable)
		void SetInputEnable(const bool _bInputEnable) { m_bInputEnable = _bInputEnable; }

	//�I�𒆂̃R�[�X���擾
	UFUNCTION(BlueprintCallable)
		FText GetSelectCourseName()const;

private:
	//�y���̓o�C���h�z�e�X�e�B�b�N�̓���
	void Input_Right();
	void Input_Left();
	void Input_Decide();
	void Input_Cansel();


	//���b�V��������
	void InitializeMesh();
	//���b�V���̉�]
	void UpdateMeshRotation(const float& DeltaTime);
	//���b�V���̍��W�ړ�
	void UpdateLocation(const float& DeltaTime);

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* m_pDummyComponent;
	UPROPERTY(VisibleAnywhere)
		int m_CourseNumber;
	UPROPERTY(VisibleAnywhere)
		int m_CourseTotal;
	UPROPERTY(VisibleAnywhere)
		bool m_bInputEnable;
	UPROPERTY(VisibleAnywhere)
		float m_CurrentRotation;
	UPROPERTY(VisibleAnywhere)
		float m_TargetRotation;
	UPROPERTY(EditAnywhere)
		float m_TargetRotationSpeed;
	UPROPERTY(EditAnywhere)
		UDataTable* m_pLevelDataTable;
	UPROPERTY(EditAnywhere)
		TArray<UStaticMeshComponent*> m_pMinimapMeshes;
	UPROPERTY(EditAnywhere)
		UStaticMesh* m_pMinimapDefaultMesh;
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInterface*> m_pMinimapMaterials;
};
