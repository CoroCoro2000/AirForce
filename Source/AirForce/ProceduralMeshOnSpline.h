//--------------------------------------------------------------------------------------------
// �t�@�C����		:ProceduralMeshOnSpline.h
// �T�v				:�X�v���C����Ɏw�萔�̃��b�V����������������A�N�^�[�N���X
// �쐬��			:2021/08/06
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------
//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshOnSpline.generated.h"

//�O���錾
class USplineComponent;
class UInstancedStaticMeshComponent;

//�f�o�b�O�pdefine
//#define DEBUG_TRANSFORM			//�g�����X�t�H�[���̃��O�o��
#define DEBUG_INSTANCECOUNT		//�ێ����Ă��郁�b�V���C���X�^���X�̐����o��

UCLASS()
class AIRFORCE_API AProceduralMeshOnSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshOnSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//�G�f�B�^��Ŕz�u���A�܂��͓����̒l���ύX���ꂽ���ɌĂяo�����֐�
	virtual void OnConstruction(const FTransform& Transform)override;

	//�X�v���C����ɐ������郁�b�V�����̍X�V
	void UpdateMeshOnSpline();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
		USplineComponent* m_pSpline;							//�I�u�W�F�N�g��z�u����ʒu�������X�v���C��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		UInstancedStaticMeshComponent* m_pMeshes;	//�X�v���C����ɔz�u����郁�b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		int m_MeshCount;												//�z�u���郁�b�V���̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		FRotator m_MeshRelativeRotation;						//�z�u���郁�b�V���̑��Ίp�x
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_bLockRotationPitch;								//���b�V����Pitch����]�����b�N���邩�ǂ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_bLockRotationYaw;									//���b�V����Yaw����]�����b�N���邩�ǂ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_bLockRotationRoll;									//���b�V����Roll����]�����b�N���邩�ǂ���
};
