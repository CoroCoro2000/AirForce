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
		GRID					UMETA(DisplayName = "Grid"),
		NUM					UMETA(Hidden),
	};
}

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

USTRUCT(BlueprintType)
struct FRandomizeStatus
{
	GENERATED_USTRUCT_BODY()

		//�R���X�g���N�^
		FRandomizeStatus()
		: bRandomizeScale(false)
		, RandomScaleMax(1.05f)
		, RandomScaleMin(0.95f)
		, bRandomizeDistance(false)
		, RandomDistanceMax(1.05f)
		, RandomDistanceMin(0.95f)
		, bRandomizeRotaion(false)
		, RandomRotaionYawAngle(1.f)
	{}

	UPROPERTY(EditAnywhere)
		bool bRandomizeScale;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeScale"))
		float RandomScaleMax;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeScale"))
		float RandomScaleMin;
	UPROPERTY(EditAnywhere)
		bool bRandomizeDistance;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeDistance"))
		float RandomDistanceMax;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeDistance"))
		float RandomDistanceMin;
	UPROPERTY(EditAnywhere)
		bool bRandomizeRotaion;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRandomizeRotaion"))
		float RandomRotaionYawAngle;
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
	//�i�q��ɐ������鏈��
	void CreateGrid();
	//���b�V�����̍X�V
	void UpdateMesh();
	//�g�����X�t�H�[�����i�[����z��̃��Z�b�g
	void ClearTempTransform();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		UHierarchicalInstancedStaticMeshComponent* m_pMeshes;							//���b�V��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		TEnumAsByte<EARRANGEMENT::Type> m_ArrangementType;						//���b�V�����ǂ̂悤�ɕ��ׂ邩
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix && m_ArrangementType != EARRANGEMENT::GRID"))
		int m_MeshCount;																						//�z�u���郁�b�V���̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix && m_ArrangementType != EARRANGEMENT::GRID"))
		float m_Distance;																						//���b�V�����m�̊Ԋu
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		FRotator m_MeshRelativeRotation;																//�z�u���郁�b�V���̑��Ίp�x
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix"))
		FRandomizeStatus m_RandomizeStatus;														//�����_��������ۂ̃p�����[�^�[
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting", meta = (EditCondition = "!m_isFix && m_ArrangementType == EARRANGEMENT::GRID"))
		FGridStatus m_GridStatus;																			//�i�q��ɔz�u����ۂɐݒ肷��p�����[�^�[
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshSetting")
		bool m_IsFix;																								//���݂̔z�u�ŌŒ肷�邩�ǂ���
		TArray<FTransform> m_pTempInstanceTransform;										//�z�u����Ă��郁�b�V���̃g�����X�t�H�[����ۑ����Ă����z��
};
