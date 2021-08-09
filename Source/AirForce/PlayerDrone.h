//------------------------------------------------------------------------
// �t�@�C����		:PlayerDrone.h
// �T�v				:�h���[���̃x�[�X���p�������v���C���[�̃h���[���N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e		:
//------------------------------------------------------------------------
// �X�V��			:19CU0104 �r�c�Ĉ�Y
// �X�V���e		:2021/06/07 �h���[���̋O�ՃG�t�F�N�g��ǉ�
//------------------------------------------------------------------------

//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "DroneBase.h"
#include "PlayerDrone.generated.h"

//�O���錾
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;

//	���_�؂�ւ�
UENUM()
enum class GAMEMODE :uint8
{
	GAMEMODE_FPS	UMETA(DisplayName = "FPS"),		//1�l��
	GAMEMODE_TPS	UMETA(DisplayName = "TPS"),		//3�l��
};

//�e���̓��͏����Ǘ������
UENUM()
enum class INPUT_AXIS :uint8
{
	THROTTLE = 0		UMETA(DisplayName = "INPUT_THROTTLE"),
	ELEVATOR				UMETA(DisplayName = "INPUT_ELEVATOR"),
	AILERON				UMETA(DisplayName = "INPUT_AILERON"),
	LADDER					UMETA(DisplayName = "INPUT_LADDER"),
};

//--------------------------------------------------------------------
//#define DEBUG_CAMERA			//�J�����̃f�o�b�O
//--------------------------------------------------------------------

UCLASS()
class AIRFORCE_API APlayerDrone : public ADroneBase
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	APlayerDrone();
protected:
	//�Q�[���J�n����1�x��������
	virtual void BeginPlay() override;
	//���̃I�u�W�F�N�g���j�������Ƃ��ɌĂяo�����֐�
	virtual void BeginDestory()override;
public:
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//�����Ă��邩���肷��(�����F���͎�)
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool IsMoving(const FVector _axisValue)const { return  (!_axisValue.IsZero() ? true : false); }

private:
	//�y���̓o�C���h�z�e�X�e�B�b�N�̓���
	void Drone_Throttle(float _axisValue);
	void Drone_Elevator(float _axisValue);
	void Drone_Aileron(float _axisValue);
	void Drone_Ladder(float _axisValue);

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SwitchGameMode(const GAMEMODE GameMode) { m_GameMode = (GameMode == GAMEMODE::GAMEMODE_FPS ? GAMEMODE::GAMEMODE_TPS : GAMEMODE::GAMEMODE_FPS); }

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SwitchViewPort();

	UFUNCTION(BlueprintCallable, Category = "Target")
		UCameraComponent* GetCamera() const { return m_pCamera; }	//�J�����擾

	UFUNCTION(BlueprintCallable, Category = "InputAxis")
		float GetInputValue(const INPUT_AXIS _Axis)const { return m_AxisValue[(int)_Axis]; }


	//�J�����̏����ݒ�
	void InitializeCamera();

	//�J�����X�V����
	void UpdateCamera(const float& DeltaTime);

	//���̓X�e�[�g����H�̉����x�ɕϊ����鏈��
	float RightInputValueToWingAcceleration(const int _arrayIndex);
	float LeftInputValueToWingAcceleration(const int _arrayIndex);

	//�H�̉����x�X�V����
	virtual void UpdateWingAccle();

	//�X�e�[�g�X�V����
	virtual void UpdateState()override;
	//�d�S�ړ�����
	virtual void UpdateCenterOfGravity(const float& DeltaTime)override;

	//��]����
	void UpdateRotation(const float& DeltaTime);

	//�ړ�����
	void UpdateSpeed(const float& DeltaTime)override;
	//�H�̉�]�X�V����
	virtual void UpdateWingRotation(const float& DeltaTime)override;
	//�J�����Ƃ̎Օ����̃R���W��������
	void UpdateCameraCollsion();

protected:
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "GameMode")
		GAMEMODE m_GameMode;	//	���_�؂�ւ�

	//-------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCamera")
		USpringArmComponent* m_pSpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCamera")
		UCameraComponent* m_pCamera;										//�J����
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraTargetLength;											//�v���C���[�ƃJ�����̋���
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_FieldOfView;												//�J�����̎���p
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffset;										//�J�����̈ʒu
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraMoveLimit;															//�J�����̈ړ��ł�����
	//-------------------------------------------------------------------------------------------------------
private:
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* m_pLightlineEffect;									//���C���G�t�F�N�g

	TArray<AActor*> m_pHitActors;											//�X�v�����O�A�[���̒����ɏՓ˂��Ă���Actor

	UPROPERTY(EditAnywhere, Category = "Bullet")
		AActor* m_pPlayerBullet;															//	�v���C���[�̒e

	//����
	UPROPERTY(VisibleAnywhere)
		bool m_bCanControl;								//���͉\���ǂ���

	UPROPERTY(VisibleAnywhere, Category = "Drone|Input")
		float m_AxisValue[4];							//�e���̓��͒l(0:THROTTLE�A1:ELEVATOR�A2:AILERON�A3:LADDER)
};
