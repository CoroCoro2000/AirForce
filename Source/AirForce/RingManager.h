//-----------------------------------------------------------------------------------------------------------------
// �t�@�C����		:RingManager.h
// �T�v				:�����O���Ǘ�����A�N�^�[�N���X
// �쐬��			:2021/08/20
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/08/20	�r������		:	�Q�[���J�n���ɂ��ׂẴ����O�ɔԍ������蓖�Ă鏈�����쐬
//						:2021/08/20	�r������		:	�����O��������ꂽ��z�񂩂�폜���鏈�����쐬
//-----------------------------------------------------------------------------------------------------------------

//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingManager.generated.h"

//�O���錾
class ARing;
class ADroneBase;
class AGameManager;

//�J���[�X�e�[�g���
UENUM(BlueprintType)
namespace ECOLOR_STATE
{
	enum Type
	{
		RED							UMETA(DisplayName = "RED"),
		ORANGE					UMETA(DisplayName = "ORANGE"),
		YELLOW					UMETA(DisplayName = "YELLOW"),
		GREEN						UMETA(DisplayName = "GREEN"),
		BLUE							UMETA(DisplayName = "BLUE"),
		INDIGO						UMETA(DisplayName = "INDIGO"),
		PURPLE						UMETA(DisplayName = "PURPLE"),
		NUM							UMETA(Hidden),
	};
}

//�f�o�b�Odefine
#define DEBUG_RING_COUNT
//�J���[���
#define LINEARCOLOR_ORANGE FLinearColor(0.94921875f, 0.609375f, 0.0703125f)		//�I�����W
#define LINEARCOLOR_PURPLE FLinearColor(0.66015625f, 0.02734375f, 0.890625f)		//��
#define LINEARCOLOR_INDIGO FLinearColor(0, 0.296875f, 0.44140625f)						//���F

UCLASS()
class AIRFORCE_API ARingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	//�R���X�g���N�^
	ARingManager();

protected:
	//�Q�[���J�n����1�x�����Ă΂�鏈��
	virtual void BeginPlay() override;

public:	
	//���t���[���Ă΂�鏈��
	virtual void Tick(float DeltaTime) override;
	
private:
	//�^�[�Q�b�g�J���[���擾
	void UpdateColorState();
	//�����O�̐F�X�V
	void UpdateColor(const float& DeltaTime);
	//�X�e�[�g����J���[�^�[�Q�b�g���擾
	FLinearColor GetTargetColor(const int32& _colorIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ARing*> m_pChildRings;			//�����O���i�[����z��
	UPROPERTY(VisibleAnywhere)
		ADroneBase* m_pDrone;					//�h���[���̏��
	UPROPERTY(VisibleAnywhere)
		AGameManager* m_pGameManager;	//�Q�[���}�l�[�W���[�̏��
	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<ECOLOR_STATE::Type> m_ColorState;	//�J���[�X�e�[�g
	UPROPERTY(EditAnywhere)
		FLinearColor m_RingColor;													//���݂̐F���
	UPROPERTY(EditAnywhere)
		FLinearColor m_FresnelColor;
	UPROPERTY(VisibleAnywhere)
		FLinearColor m_TargetColor;												//���݂̐F���
	UPROPERTY(EditAnywhere)
		float m_ColorTransitionSpeed;												//�F�̑J�ڂ��鑬�x
	UPROPERTY(EditAnywhere)
		int32 m_DelayTempo;															//�F�����e���|�x�点�邩
};
