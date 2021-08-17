//--------------------------------------------------------------------------------------------
// �t�@�C����		:RegularlyAlignedProceduralMesh.h
// �T�v				:�K���I�ɕ��ԃ��b�V����������������A�N�^�[�N���X
// �쐬��			:2021/08/07
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------
//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlignedProceduralMesh.generated.h"

//�O���錾
class UHierarchicalInstancedStaticMeshComponent;

//���b�V���̕��ו����w�肷���
UENUM(BlueprintType)
namespace EARRANGEMENT
{
	enum Type
	{
		LINEAR				UMETA(DisplayName = "Linear"),
		CURVE				UMETA(DisplayName = "Curve"),
		CIRCLE				UMETA(DisplayName = "Circle"),
		SPIRAL				UMETA(DisplayName = "Spiral"),
		GRID					UMETA(DisplayName = "Grid"),
		NUM					UMETA(Hidden),
	};
}

USTRUCT(BlueprintType)
struct FLinearStatus
{
	GENERATED_USTRUCT_BODY()

		FLinearStatus()
		: Distance(10.f)
	{}

	float Distance;
};

USTRUCT(BlueprintType)
struct FCurveStatus
{
	GENERATED_USTRUCT_BODY()

};

USTRUCT(BlueprintType)
struct FCircleStatus
{
	GENERATED_USTRUCT_BODY()

};

USTRUCT(BlueprintType)
struct FSpiralStatus
{
	GENERATED_USTRUCT_BODY()

	FSpiralStatus()
		: Direction(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector Direction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FRotator Rotation;
};

USTRUCT(BlueprintType)
struct FGridStatus
{
	GENERATED_USTRUCT_BODY()

	FGridStatus()
		: MeshCountX(1)
		, MeshCountY(1)
		, MeshCountZ(1)
		, Distance(FVector::ZeroVector)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MeshCountX;					//X���̃��b�V���̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MeshCountY;					//Y���̃��b�V���̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int MeshCountZ;					//Z���̃��b�V���̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector Distance;				//�אڂ��郁�b�V���Ƃ̋���
};

//�f�o�b�O�pdefine
//#define DEBUG_TRANSFORM			//�g�����X�t�H�[���̃��O�o��
#define DEBUG_INSTANCECOUNT		//�ێ����Ă��郁�b�V���C���X�^���X�̐����o��

UCLASS()
class AIRFORCE_API AAlignedProceduralMesh : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAlignedProceduralMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	//�G�f�B�^��Ŕz�u���A�܂��͓����̒l���ύX���ꂽ���ɌĂяo�����֐�
	virtual void OnConstruction(const FTransform& Transform)override;

	//������ɐ������鏈��
	void CreateLinear();
	//�Ȑ���ɐ������鏈��
	void CreateCurved();
	//�~��ɐ������鏈��
	void CreateCircular();
	//������ɐ������鏈��
	void CreateSpiral();
	//�i�q��ɐ������鏈��
	void CreateGrid();
	//���b�V�����̍X�V
	void UpdateMesh();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		UHierarchicalInstancedStaticMeshComponent* m_pMeshes;							//���b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		int m_MeshCount;																		//�z�u���郁�b�V���̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		float m_Distance;																		//���b�V�����m�̊Ԋu
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		TEnumAsByte<EARRANGEMENT::Type> m_ArrangementType;		//���b�V�����ǂ̂悤�ɕ��ׂ邩
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		FSpiralStatus m_SpiralStatus;														//������ɔz�u����ۂɐݒ肷��p�����[�^�[
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		FGridStatus m_GridStatus;															//�i�q��ɔz�u����ۂɐݒ肷��p�����[�^�[
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
		FRotator m_MeshRelativeRotation;												//�z�u���郁�b�V���̑��Ίp�x
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
	//	bool m_bLockRotationPitch;														//���b�V����Pitch����]�����b�N���邩�ǂ���
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
	//	bool m_bLockRotationYaw;															//���b�V����Yaw����]�����b�N���邩�ǂ���
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting|Transform")
	//	bool m_bLockRotationRoll;															//���b�V����Roll����]�����b�N���邩�ǂ���
};
