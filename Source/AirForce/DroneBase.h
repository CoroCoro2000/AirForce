//------------------------------------------------------------------------
// �t�@�C����		:DroneBase.h
// �T�v				:�h���[���̃x�[�X�N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/04/19 �v���C���[�ƃG�l�~�[�̋��ʍ��̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "DroneBase.generated.h"

//�O���錾
class UStaticMeshComponent;
class USphereComponent;
class USoundBase;

//�ړ��p�r�b�g�t�B�[���h
struct FMoveDirectionFlag
{
	//���ׂẴt���O�ɑ΂��Đݒ���s���֐�
	void SetAllFlag(const bool& _isUp, const bool& _isDown, const bool& _isForward, const bool& _isBackward, const bool& _isRight, const bool& _isLeft, const bool& _isRightTurning, const bool& _isLeftTurning)
	{

	}

		uint8 Up					 : 1;		//�㏸			0
		uint8 Down				 : 1;		//���~			1
		uint8 Forward			 : 1;		//�O���ړ�	2
		uint8 Backward			: 1;		//����ړ�	3
		uint8 Right			 		 : 1;		//�E�ړ�		4
		uint8 Left					 : 1;		//���ړ�		5
		uint8 RightTurning		: 1;		//�E��]		6
		uint8 LeftTurning		 : 1;		//����]		7
};

//�ړ��p���p��
union MoveDirection
{
	uint8 iBits : 8;		//�ꊇ�Ǘ�(0 ~ 255�̒l�ŊǗ�)
	FMoveDirectionFlag sFlag;	//�ʊǗ�
};

//��ԃr�b�g�t�B�[���h
struct FStateFlag
{
		uint8 Wait			: 1;		//�n�ʑҋ@						0
		uint8 Hovering	: 1;		//�z�o�����O(�󒆑ҋ@)		1
		uint8 Move			: 1;		//�ړ���							2
		uint8 Crash		: 1;		//�ė�							3
};

//��ԗp���p��
union State
{
	uint8 iBits : 4;								//�ꊇ�Ǘ�(0 ~ 15�̒l�ŊǗ�)
	FStateFlag sFlag;							//�ʊǗ�
};

//�H�̔ԍ��̗�
UENUM(BlueprintType)
namespace EWING
{
	enum Type
	{
		LEFT_FORWARD						UMETA(DisplayName = "LEFT_FORWARD"),
		RIGHT_FORWARD					UMETA(DisplayName = "RIGHT_FORWARD"),
		LEFT_BACKWARD					UMETA(DisplayName = "LEFT_BACKWARD"),
		RIGHT_BACKWARD				UMETA(DisplayName = "RIGHT_BACKWARD"),
		NUM										UMETA(Hidden)
	};
}

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

public:
	uint8 GetWingNumber()const { return WingNumber; }							//�H�ԍ��擾
	UStaticMeshComponent* GetWingMesh()const { return pWingMesh; }	//�H�̃��b�V���擾

private:
	UPROPERTY(EditAnywhere, DisplayName = "WingNumber")
		uint8 WingNumber;																//���ʔԍ�(1:���O�A2:�E�O�A3:�����A4:�E���)
	UPROPERTY(EditAnywhere, DisplayName = "WingMesh")
		UStaticMeshComponent* pWingMesh;										//���b�V��

public:
	UPROPERTY(EditAnywhere, DisplayName = "AcceleState")
		float AccelState;																		//�����x�̒i�K(-1:�ŏ��̉����x�A0:�����x�Ȃ��A1:�����x����A2:�ő�̉����x)
};

//define�}�N��
//���݂�FPS���v��
#define FPS (1.f / DeltaTime)
//�t���[�����[�g���ቺ���Ă��ړ��ʂɉe���������悤�␳����l
#define MOVE_CORRECTION (60.f / FPS)
//--------------------------------------------------------------------
//#define DEGUG_ACCEL					//�����x�̃f�o�b�O
//#define DEBUG_GRAVITY				//�d�͂̃f�o�b�O
//#define DEBUG_WING					//�H�̃f�o�b�O
//#define DEBUG_COLLISION_WINDRANGE		//���͈̔͂̓����蔻��̃f�o�b�O�\��
//#define DEBUG_COLLISION_WINDRANGE_OVERLAPDETA	//���͈̔͂̓����蔻��ɃI�[�o�[���b�v���Ă���A�N�^�[�̐�
//#define DEBUG_WindRangeOverlap_Begin		//�I�[�o�[���b�v�J�n���̃f�o�b�O
//#define DEBUG_WindRangeOverlap_End		//�I�[�o�[���b�v�J�n���̃f�o�b�O
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

public:
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	���̓t���O�̎擾
	void SetisControl(const bool _isControl) { m_isControl = _isControl; }

protected:
	//�h���[���̓����蔻��ɃI�u�W�F�N�g���I�[�o�[���b�v�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//�h���[���̓����蔻��ɃI�u�W�F�N�g���q�b�g�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		void OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:
	//�d�͉����x�̎擾
	float GetGravitationalAcceleration()const { return m_GravityScale * m_DescentTime * m_DescentTime / 2.f; }

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
	//�X�e�[�g�X�V����
	virtual void UpdateState();
	//��]����
	virtual void UpdateRotation(const float& DeltaTime);
	//���x�X�V����
	virtual void UpdateSpeed(const float& DeltaTime);
	//�H�̉�]�X�V����
	virtual void UpdateWingRotation(const float& DeltaTime);
	//�d�͍X�V����
	float UpdateGravity(const float& DeltaTime);
	//���̃G�t�F�N�g�X�V����
	virtual void UpdateWindEffect(const float& DeltaTime);

protected:
	//BODY
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;
	//�h���[���̃R���W����
	UPROPERTY(EditAnywhere, Category = "Collision")
		USphereComponent* m_pDroneCollision;
	//WING
	UPROPERTY(EditAnywhere, Category = "Wing")
		FWing m_Wings[EWING::NUM];								
	//1�b�Ԃ̉H�̍ő��]��
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_RPSMax;
	//�H�̉����x
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccele;
	//�ŏ��̉����x�̔{��
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMin;
	//�ő�̉����x�̔{��
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMax;

	//�ړ��t���O�Ǘ�
	MoveDirection m_MoveDirectionFlag;
	//�X�e�[�g�t���O�Ǘ�
	State m_StateFlag;

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float  m_Speed;									//�h���[���̕b��(m)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_SpeedPerSecondMax;						//�h���[���̍ő�b��(m)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector4 m_AxisAccel;
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Acceleration;							//�����x
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Deceleration;							//�����x
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Turning;								//�t���͂������̌�����
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_DroneWeight;							//�h���[���̏d��(kg)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_Velocity;								//���̃h���[���ɂ������Ă���̗͂�
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_CentrifugalForce;						//���S��
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_AngularVelocity;									//�p���x(�U���̊p���x)
	UPROPERTY(EditAnywhere, Category = "Physical|Gravity")
		float m_GravityScale;							//�d�͌W��
	UPROPERTY(EditAnywhere, Category = "Physical")
		FVector m_Gravity;								//�d��
	UPROPERTY(VisibleAnywhere, Category = "Physical|Gravity")
		float m_DescentTime;							//�������Ă��鎞��
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_pWingRotationSE;			//�H�̉�]SE
	UPROPERTY(EditAnywhere, Category = "Flag")
		bool m_isControl;								//����\�t���O
	UPROPERTY(EditAnywhere, Category = "Flag")
		bool m_isFloating;								//����\�t���O
	UPROPERTY(VisibleAnywhere, Category = "Ring")
		int m_RingAcquisition;							//�����O�l����
};
