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
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class UNiagaraComponent;

//�e���̓��͏����Ǘ������
UENUM(BlueprintType)
namespace EINPUT_AXIS
{
	enum Type
	{
		THROTTLE					UMETA(DisplayName = "THROTTLE"),	//�㉺
		ELEVATOR					UMETA(DisplayName = "ELEVATOR"),	//�O��
		AILERON					UMETA(DisplayName = "AILERON"),	//���E
		LADDER						UMETA(DisplayName = "LADDER"),		//����
		NUM							UMETA(Hidden)
	};
}

//--------------------------------------------------------------------
//#define DEBUG_UpdateCamera			//�J�����̃f�o�b�O
//#define DEBUG_IsOverHeightMax
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

public:
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//�����Ă��邩���肷��(�����F���͎�)
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool IsMoving(const FVector _axisValue)const { return  (!_axisValue.IsZero() ? true : false); }

	//���[�X�̍��W�t�@�C����������
	void WritingRaceVector();

	//���[�X�̃N�I�[�^�j�I���t�@�C����������
	void WritingRaceQuaternion();
private:
	//�y���̓o�C���h�z�e�X�e�B�b�N�̓���
	void Input_Throttle(float _axisValue);
	void Input_Elevator(float _axisValue);
	void Input_Aileron(float _axisValue);
	void Input_Ladder(float _axisValue);

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SwitchGameMode(const TEnumAsByte<EGAMEMODE::Type> GameMode) { m_GameMode = (GameMode == EGAMEMODE::GAMEMODE_FPS ? EGAMEMODE::GAMEMODE_TPS : EGAMEMODE::GAMEMODE_FPS); }

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SwitchViewPort();

	UFUNCTION(BlueprintCallable, Category = "Target")
		UCameraComponent* GetCamera() const { return m_pCamera; }	//�J�����擾

	UFUNCTION(BlueprintCallable, Category = "InputAxis")
		float GetInputValue(const TEnumAsByte<EINPUT_AXIS::Type> _Axis)const { return m_AxisValue[_Axis]; }


	//�J�����̏����ݒ�
	void InitializeCamera();

	//�J�����X�V����
	void UpdateCamera(const float& DeltaTime);

	//���̓X�e�[�g����H�̉����x�ɕϊ����鏈��
	float RightInputValueToWingAcceleration(const int _arrayIndex);
	float LeftInputValueToWingAcceleration(const int _arrayIndex);

	//�H�̉����x�X�V����
	virtual void UpdateWingAccle(const float& DeltaTime);
	 
	//���͂̉����x�X�V����
	virtual void UpdateAxisAcceleration(const float& DeltaTime);

	//�X�e�[�g�X�V����
	virtual void UpdateState()override;

	//��]����
	void UpdateRotation(const float& DeltaTime);

	//�ړ�����
	void UpdateSpeed(const float& DeltaTime)override;

	//�J�����Ƃ̎Օ����̃R���W��������
	void UpdateCameraCollsion();

	//���̃G�t�F�N�g�̍X�V����
	void UpdateWindEffect(const float& DeltaTime);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCamera")
		USpringArmComponent* m_pSpringArm;									//�X�v�����O�A�[��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCamera")
		UCameraComponent* m_pCamera;										//�J����
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraTargetLength;											//�v���C���[�ƃJ�����̋���
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_FieldOfView;												//�J�����̎���p
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffset;										//�J�����̈ʒu
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffsetMax;									//�J�������Ǐ]����͈͂̏��
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraMoveLimit;											//�J�����̈ړ��ł�����
	//-------------------------------------------------------------------------------------------------------
private:
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraSystem* m_pLightlineEffect;									//���C���G�t�F�N�g

	TArray<AActor*> m_pHitActors;											//�X�v�����O�A�[���̒����ɏՓ˂��Ă���Actor

	UPROPERTY(VisibleAnywhere, Category = "Drone|Input")
		FVector4 m_AxisValue;												//�e���̓��͒l(0:AILERON�A1:ELEVATOR�A2:THROTTLE�A3:LADDER)

	float m_CameraRotationYaw;
};
