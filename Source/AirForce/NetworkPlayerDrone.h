//------------------------------------------------------------------------
// �t�@�C����		:NetworkPlayerDrone.h
// �T�v				:�l�b�g���[�N�p�̃v���C���[�h���[���N���X
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DroneBase.h"
#include "PlayerDrone.h"
#include "NetworkPlayerDrone.generated.h"

//�O���錾
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class USpotLightComponent;
class FName;

UCLASS()
class AIRFORCE_API ANetworkPlayerDrone : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetworkPlayerDrone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//�h���[���̓����蔻��ɃI�u�W�F�N�g���I�[�o�[���b�v�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//�h���[���̓����蔻��ɃI�u�W�F�N�g���q�b�g�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		void OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	//���v���P�[�g��o�^
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//�����Ă��邩���肷��(�����F���͎�)
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool IsMoving(const FVector _axisValue)const { return  (!_axisValue.IsZero() ? true : false); }
	//�v���C���[��ID�ݒ�
	UFUNCTION(BlueprintCallable, Category = "Network")
		void SetPlayerId(const int32& PlayerId) { m_PlayerId = PlayerId; }
	//�v���C���[��ID�擾
	UFUNCTION(BlueprintCallable, Category = "Network")
		int32 GetPlayerId()const { return m_PlayerId; }
	//	���̓t���O�̐ݒ�
	UFUNCTION(BlueprintCallable, Category = "Control")
		void SetisControl(const bool _isControl) { m_isControl = _isControl; }
	//	�h���[���̉����t���O�擾
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		bool GetIsOverAccle()const { return m_bIsPassedRing; }

private:
	//�y���̓o�C���h�z�e�X�e�B�b�N�̓���
	void Input_Throttle(float _axisValue);
	void Input_Elevator(float _axisValue);
	void Input_Aileron(float _axisValue);
	void Input_Ladder(float _axisValue);

	//���͗ʂ̎擾
	UFUNCTION(BlueprintCallable, Category = "InputAxis")
		float GetInputValue(const TEnumAsByte<EINPUT_AXIS::Type> _Axis)const { return m_AxisValue[_Axis]; }

	//���͒l���H�̉����x�ɕϊ����鏈��
	float RightInputValueToWingAcceleration(const int _arrayIndex);
	float LeftInputValueToWingAcceleration(const int _arrayIndex);
	//�H�̉����x�X�V����
	virtual void UpdateWingAccle(const float& DeltaTime);
	//�H�̉�]�X�V����
	virtual void UpdateWingRotation(const float& DeltaTime);
	//���͂̉����x�X�V����
	virtual void UpdateAxisAcceleration(const float& DeltaTime);
	//��]����
	void UpdateRotation(const float& DeltaTime);
	//�ړ�����
	void UpdateSpeed(const float& DeltaTime);
	//�J�����X�V����
	void UpdateCamera(const float& DeltaTime);
	//�J�����Ƃ̎Օ����̃R���W��������
	void UpdateCameraCollsion(const float& DeltaTime);
	//���̃G�t�F�N�g�̍X�V����
	void UpdateWindEffect(const float& DeltaTime);
	//�i�s���Ɠ��͎����t�������m�F
	bool IsReverseInput(const float& _movingAxis, const float& _axisValue)const { return (_movingAxis < 0.f && 0.f < _axisValue) || (_movingAxis > 0.f && 0.f > _axisValue); }
	//���x�̏�����𒴂��Ă��邩�m�F
	void UpdateAltitudeCheck();
	//�����̃G�t�F�N�g�̕\���ؑ�
	void UpdateCloudOfDustEffect();
	//���b�V���A�Z�b�g�̃Z�b�g�A�b�v
	virtual void MeshAssetSetup();
	//���b�V���̏����ݒ�
	virtual void InitializeMesh();
	//���C�g�̏����ݒ�
	virtual void InitializeLight();
	//�G�t�F�N�g�̏����ݒ�
	void InitializeEmitter();
	//�J�����̏����ݒ�
	void InitializeCamera();

private:
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMesh* m_BodyMesh;													//�@�̂̃��b�V��
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;
	//�H
	UPROPERTY(EditAnywhere, EditFixedSize, Category = "Mesh|Wing")
		TArray<UStaticMesh*> m_WingMesh;													//�H�̃��b�V��
	TArray<TSharedPtr<FWing>>  m_pWings;												//�H���Ǘ�����\����
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		USpringArmComponent* m_pSpringArm;									//�X�v�����O�A�[��
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		UCameraComponent* m_pCamera;										//�J����
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraTargetLength;											//�v���C���[�ƃJ�����̋���
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_FieldOfView;														//�J�����̎���p
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffset;										//�J�����̈ʒu
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffsetMax;									//�J�������Ǐ]����͈͂̏��
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraMoveLimit;											//�J�����̈ړ��ł�����
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FRotator m_CameraRotationAttenRate;							//�J�������X������
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_MotionBlurAmount;											//���[�V�����u���[�̋��x
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_MotionBlurMax;													//���[�V�����u���[�̍ő�c��
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		int32 m_MotionBlurTargetFPS;											//���[�V�����u���[�̃^�[�Q�b�gFPS
	UPROPERTY(VisibleAnywhere, Category = "Drone|Input")
		FVector4 m_AxisValue;												//�e���̓��͒l(0:AILERON�A1:ELEVATOR�A2:THROTTLE�A3:LADDER)
	UPROPERTY(VisibleAnywhere, Category = "UI")
		FVector m_StartLocation;
	UPROPERTY(VisibleAnywhere, Category = "UI")
		FQuat m_StartQuaternion;
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraRotationYaw;
	UPROPERTY(EditAnywhere, Category = "Network")
		int32 m_PlayerId;
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
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_TiltLimit;									//�X���̏��
	UPROPERTY(EditAnywhere, Category = "Physical")
		float  m_Speed;									//�h���[���̕b��(m)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_SpeedPerSecondMax;						//�h���[���̍ő�b��(m)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector4 m_AxisAccel;						//�e���̉����x
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Acceleration;							//�����x
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Deceleration;							//�����x
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Turning;								//�t���͂������̌�����
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Attenuation;							//�Փˎ��̑��x������
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_DroneWeight;							//�h���[���̏d��(kg)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_Velocity;								//���̃h���[���ɂ������Ă���̗͂�
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_pWingRotationSE;			//�H�̉�]SE
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_HeightMax;															//�h���[������Ԃ��Ƃ̂ł���n�ʂ���̍����͈̔�
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_HeightFromGround;												//�n�ʂ���̍���
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_DistanceToSlope;													//�Ζʂ܂ł̋���
	UPROPERTY(EditAnywhere, Category = "Drone")
		bool m_isControl;								//����\�t���O
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraSystem* m_pWindEffect;										//���̃G�t�F�N�g
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraComponent* m_pWindEmitter;									//���̃G�t�F�N�g
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindRotationSpeed;											//���̃G�t�F�N�g�̉�]���x
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindOpacity;
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindNoise;
	UPROPERTY(VisibleAnywhere, Category = "Ring")
		bool m_bIsPassedRing;															//�����O�������������ǂ���
	UPROPERTY(VisibleAnywhere, Category = "Ring")
		float m_SincePassageCount;													//�����O���������Ă���̌o�ߎ���
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_CountLimitTime;															//�����O���������Ă��琔���鎞�Ԃ̏��	
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_OverAccelerator;														//�����O�����������Ƃ��̉����{��
	UPROPERTY(EditAnywhere, Category = "Light")
		USpotLightComponent* m_pLeftSpotLight;
	UPROPERTY(EditAnywhere, Category = "Light")
		USpotLightComponent* m_pRightSpotLight;
	UPROPERTY(EditAnywhere, Category = "Effect")
		TMap<FString, UNiagaraSystem*> m_pDroneEffects;				//�h���[���̃G�t�F�N�g���i�[����z��
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraComponent* m_pCloudOfDustEmitter;								//�����̃G�t�F�N�g
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_ShowEffectDistance;															//�G�t�F�N�g��\�����鋗��
	UPROPERTY(EditAnywhere, Category = "Effect")
		FString m_GroundMaterialName;														//���C���q�b�g�����n�ʂ̃}�e���A����
};
