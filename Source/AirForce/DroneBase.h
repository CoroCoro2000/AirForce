//------------------------------------------------------------------------
// �t�@�C����		:DroneBase.h
// �T�v				:�h���[���̃x�[�X�N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/04/19 �v���C���[�ƃG�l�~�[�̋��ʍ��̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h�K�[�h
#pragma once
#pragma warning(disable : 4582)

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "DroneBase.generated.h"

//�O���錾
class UStaticMeshComponent;
class UBoxComponent;

//�ړ��p�r�b�g�t�B�[���h
USTRUCT(BlueprintType)
struct FMoveDirectionFlag
{
	GENERATED_USTRUCT_BODY()

	//�R���X�g���N�^
	FMoveDirectionFlag(const uint8 up, const uint8 down, const uint8 forward, const uint8 backward, const uint8 right, const uint8 left, const uint8 rightTurning, const uint8 leftTurning)
	: Up(up)
	, Down(down)
	, Forward(forward)
	, Backward(backward)
	, Right(right)
	, Left(left)
	, RightTurning(rightTurning)
	, LeftTurning(leftTurning)
	{}
	FMoveDirectionFlag() : FMoveDirectionFlag(0, 0, 0, 0, 0, 0, 0, 0) {}

	UPROPERTY(EditAnywhere, DisplayName = "Up")
		uint8 Up					 : 1;		//�㏸			0
	UPROPERTY(EditAnywhere, DisplayName = "Down")
		uint8 Down				 : 1;		//���~			1
	UPROPERTY(EditAnywhere, DisplayName = "Forward")
		uint8 Forward			 : 1;		//�O���ړ�	2
	UPROPERTY(EditAnywhere, DisplayName = "Backward")
		uint8 Backward		 : 1;		//����ړ�	3
	UPROPERTY(EditAnywhere, DisplayName = "Right")
		uint8 Right			 	 : 1;		//�E�ړ�		4
	UPROPERTY(EditAnywhere, DisplayName = "Left")
		uint8 Left					 : 1;		//���ړ�		5
	UPROPERTY(EditAnywhere, DisplayName = "RightTurning")
		uint8 RightTurning	 : 1;		//�E��]		6
	UPROPERTY(EditAnywhere, DisplayName = "LeftTurning")
		uint8 LeftTurning		 : 1;		//����]		7
};

//�ړ��p���p��
union MoveDirection
{
	//�R���X�g���N�^
	MoveDirection()
		: iBits(0)
		, sFlag(FMoveDirectionFlag(0, 0, 0, 0, 0, 0, 0, 0))
	{}
	MoveDirection(const uint8 bits) :iBits(bits) {}
	MoveDirection(const FMoveDirectionFlag moveFlag) :sFlag(moveFlag) {}

	uint8 iBits : 8;		//�ꊇ�Ǘ�(0 ~ 255�̒l�ŊǗ�)
	FMoveDirectionFlag sFlag;	//�ʊǗ�
};

//��ԃr�b�g�t�B�[���h
USTRUCT(BlueprintType)
struct FStateFlag
{
	GENERATED_USTRUCT_BODY()
	//�R���X�g���N�^
	FStateFlag(const uint8 wait, const uint8 hovering, const uint8 move, const uint8 crash)
	: Wait(wait)
	, Hovering(hovering)
	, Move(move)
	, Crash(crash)
	{}
	FStateFlag() : FStateFlag(0, 0, 0, 0) {}

	UPROPERTY(EditAnywhere, DisplayName = "Wait")
		uint8 Wait			: 1;		//�n�ʑҋ@						0
	UPROPERTY(EditAnywhere, DisplayName = "Hovering")
		uint8 Hovering	: 1;		//�z�o�����O(�󒆑ҋ@)		1
	UPROPERTY(EditAnywhere, DisplayName = "Move")
		uint8 Move			: 1;		//�ړ���							2
	UPROPERTY(EditAnywhere, DisplayName = "Crash")
		uint8 Crash		: 1;		//�ė�							3
};

//��ԗp���p��
union State
{
	//�R���X�g���N�^
	State()
		: iBits(0)
		, sFlag(FStateFlag(0, 0, 0, 0))
	{}
	State(const uint8 bits) :iBits(bits) {}
	State(const FStateFlag stateFlag) :sFlag(stateFlag) {}

	uint8 iBits : 4;								//�ꊇ�Ǘ�(0 ~ 15�̒l�ŊǗ�)
	FStateFlag sFlag;							//�ʊǗ�
};

//�H�̏����Ǘ�����\����
USTRUCT(BlueprintType)
struct FWing
{
	GENERATED_USTRUCT_BODY()

		//�R���X�g���N�^
		FWing()
		: WingNumber(0)
		, pWingMesh(NULL)
		, AccelState(0.f)
	{}

	FWing(const uint8 wingNum, UStaticMeshComponent* wingMesh)
		: WingNumber(wingNum)
		, pWingMesh(wingMesh)
		, AccelState(0.f)
	{}

private:
	UPROPERTY(EditAnywhere, DisplayName = "WingNumber")
		uint8 WingNumber;																//���ʔԍ�(1:���O�A2:�E�O�A3:�����A4:�E���)
	UPROPERTY(EditAnywhere, DisplayName = "WingMesh")
		UStaticMeshComponent* pWingMesh;												//���b�V��
public:
	UPROPERTY(EditAnywhere, DisplayName = "AcceleState")
		float AccelState;																//�����x�̒i�K(-1:�ŏ��̉����x�A0:�����x�Ȃ��A1:�����x����A2:�ő�̉����x)

public:
	uint8 GetWingNumber()const { return WingNumber; }							//�H�ԍ��擾
	UStaticMeshComponent* GetWingMesh()const { return pWingMesh; }	//�H�̃��b�V���擾
};

//define�}�N��
//�H�̍ő吔
#define WING_ARRAY_MAX 4
//�H�̗v�f�ԍ�
#define LF_WING 0
#define RF_WING 1
#define LB_WING 2
#define RB_WING 3
//���͂̏��
#define BUOYANCY_HOVERING 0.f
//���݂�FPS���v��
#define FPS (1.f / DeltaTime)
//�t���[�����[�g���ቺ���Ă��ړ��ʂɉe���������悤�␳����l
#define MOVE_CORRECTION (60.f / FPS)
//--------------------------------------------------------------------
//#define DEGUG_ACCEL					//�����x�̃f�o�b�O
//#define DEBUG_GRAVITY				//�d�͂̃f�o�b�O
//#define DEBUG_WING					//�H�̃f�o�b�O
//#define DEBUG_OVERLAP_BEGIN	//�I�[�o�[���b�v�J�n���̃f�o�b�O
//#define DEBUG_OVERLAP_END		//�I�[�o�[���b�v�I�����̃f�o�b�O
//--------------------------------------------------------------------

UCLASS()
class AIRFORCE_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	ADroneBase();
protected:
	//�Q�[���J�n����1�x��������
	virtual void BeginPlay() override;
	//���̃I�u�W�F�N�g���j�������Ƃ��ɌĂяo�����֐�
	virtual void BeginDestory();
public:
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	���̓t���O�̎擾
	void SetisControl(const bool _isControl) { m_isControl = _isControl; }

	//�I�[�o�[���b�v���ɌĂ΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//�I�[�o�[���b�v���Ă����A�N�^�[���痣�ꂽ�u�ԌĂ΂��C�x���g�֐�
	UFUNCTION()
		virtual void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	//�d�͉����x�̎擾
	float GetGravitationalAcceleration()const { return m_GravityScale * m_DescentTime * m_DescentTime / 2.f; }
	//�H�̉����x�𐳋K�����ĕԂ�
	float GetWingNormalizeAccele()const { return m_WingAccele / (m_WingHoveringAccele * m_WingAccelMax); }

	//	�h���[���̎���(kilometers per hour)�擾
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetSpeed()const { return m_Speed; }

	//	�h���[���̎���(kilometers per hour)�擾
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetKPH(const float _deltaTime)const { return m_Speed / 100000.f / _deltaTime * 3600.f * 10.f; }
	
	//	�h���[���̃����O�l�����擾
	UFUNCTION(BlueprintCallable, Category = "Drone|Ring")
		int GetRingAcquisition() { return m_RingAcquisition; }
protected:
	//�H�̉����x�X�V����
	virtual void UpdateWingAccle();
	//�d�S�ړ�����
	virtual void UpdateCenterOfGravity(const float& DeltaTime);
	//�X�e�[�g�X�V����
	virtual void UpdateState();
	//��]����
	virtual void UpdateRotation(const float& DeltaTime);
	//���x�X�V����
	virtual void UpdateSpeed(const float& DeltaTime);
	//�ړ�����
	virtual void UpdateMove(const float& DeltaTime);

	//�H�̉�]�X�V����
	virtual void UpdateWingRotation(const float& DeltaTime);

	//�d�͍X�V����
	float UpdateGravity(const float& DeltaTime);

	//������n�ʖ����؂�̂�
	virtual float SetDecimalTruncation(float value, float n);

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;

	//�H
	/*-------------------------------------------------------------------------*/
	TArray<TSharedPtr<FWing>> m_pWings;								//�h���[���̉H
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_rpsMax;											//1�b�Ԃ̉H�̍ő��]��
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccele;									//�H�̉����x
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingOldAccele;								//�H�̉����x
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMin;								//�ŏ��̉����x�̔{��
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingHoveringAccele;						//�z�o�����O(���͂Ȃ�)���̉����x�̔{��
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMax;								//�ő�̉����x�̔{��
	/*-------------------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, Category = "Collision")
		UBoxComponent* m_pDroneBoxComp;					//�h���[���̓����蔻��

	MoveDirection m_MoveDirectionFlag;					//�ړ��t���O�Ǘ�
	State m_StateFlag;									//�X�e�[�g�t���O�Ǘ�

	FVector m_CurrentLocation;							//�h���[���̌��ݒn
	FVector m_PrevCurrentLocation;						//1�O�̃h���[���̌��ݒn

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float  m_Speed;									//�h���[���̕b��(m)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_SpeedPerSecondMax;						//�h���[���̍ő�b��(m)

	//-----------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Acceleration;							//�����x
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Deceleration;							//�����x
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Turning;								
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_MaxSpeed;

	//-------------------------------------------------------------------------------
	FQuat m_OldRotation;						//1�t���[���O�̌X��

	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_DroneWeight;							//�h���[���̏d��(kg)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_Velocity;								//���̃h���[���ɂ������Ă���̗͂�
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_CenterOfGravity;						//�h���[���̏d�S
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector Centrifugalforce;						//���S��

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_AngularVelocity;									//�p���x(�U���̊p���x)

	UPROPERTY(EditAnywhere, Category = "Physical|Gravity")
		float m_GravityScale;							//�d�͌W��
	UPROPERTY(EditAnywhere, Category = "Physical")
		FVector Gravity;								//�d��
	UPROPERTY(VisibleAnywhere, Category = "Physical|Gravity")
		float m_DescentTime;							//�������Ă��鎞��

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* WingRotationSE;								//�H�̉�]SE

	UPROPERTY(EditAnywhere, Category = "Flag")
		bool m_isControl;								//����\�t���O

	UPROPERTY(EditAnywhere, Category = "Flag")
		bool m_isFloating;								//����\�t���O

	UPROPERTY(VisibleAnywhere, Category = "Ring")
		int m_RingAcquisition;							//�����O�l����
};
