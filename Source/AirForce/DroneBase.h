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
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class USpotLightComponent;
class FName;

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
struct FWing
{
public:
	//�R���X�g���N�^
	FWing(const uint32 wingNum, UStaticMeshComponent* wingMesh)
		: WingNumber(wingNum)
		, pWingMesh(wingMesh)
		, AccelState(0.f)
	{}
	//�f�X�g���N�^
	~FWing() {}

public:
	uint32 GetWingNumber()const { return WingNumber; }							//�H�ԍ��擾
	UStaticMeshComponent* GetWingMesh()const { return pWingMesh; }		//�H�̃��b�V���擾

private:
	uint32 WingNumber;																				//���ʔԍ�(1:���O�A2:�E�O�A3:�����A4:�E���)
	UStaticMeshComponent* pWingMesh;														//���b�V��

public:
	float AccelState;																						//�����x�̒i�K(-1:�ŏ��̉����x�A0:�����x�Ȃ��A1:�����x����A2:�ő�̉����x)
};

//define�}�N��
//���݂�FPS���v��
#define GetFPS (1.f / DeltaTime)
//�t���[�����[�g���ቺ���Ă��ړ��ʂɉe���������悤�␳����l
#define MOVE_CORRECTION (60.f / GetFPS)
#define SLOPE_MIN 0.f
#define SPEED_MIN -10.5f
#define SPEED_MAX 10.5f
#define VECTOR3_COMPONENT_NUM 3
#define VECTOR4_COMPONENT_NUM 4

UCLASS()
class AIRFORCE_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	ADroneBase();
	//�f�X�g���N�^
	virtual ~ADroneBase();

protected:
	//�Q�[���J�n����1�x��������
	virtual void BeginPlay() override;

	//���v���P�[�g��o�^
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
public:
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	���̓t���O�̐ݒ�
	void SetisControl(const bool _isControl) { m_isControl = _isControl; }

protected:
	//�h���[���̓����蔻��ɃI�u�W�F�N�g���I�[�o�[���b�v�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//�h���[���̓����蔻��ɃI�u�W�F�N�g���q�b�g�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		void OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	//����\�t���O�擾
	UFUNCTION(BlueprintCallable, Category = "Drone")
		bool GetisControl() const { return  m_isControl; }

	//�{�f�B���b�V���̉�]�ݒ�
	UFUNCTION(BlueprintCallable, Category = "Drone")
		void SetBodyMeshRotation(const FRotator& NewRotator);
	//�{�f�B���b�V���̉�]�ݒ�
	void SetBodyMeshRotation(const FQuat& NewRotator);
	//�{�f�B���b�V���̉�]�ʎ擾
	UFUNCTION(BlueprintCallable, Category = "Drone")
		FRotator GetBodyMeshRotation()const;
	//�{�f�B���b�V���̉�]�ʎ擾
	UFUNCTION(BlueprintCallable, Category = "Drone")
		FRotator GetBodyMeshRelativeRotation()const;

	//	�h���[���̎���(kilometers per hour)�擾
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetKPH(const float DeltaTime)const { return m_Velocity.Size() * (60.f / (1.f / DeltaTime)) / 100000.f / DeltaTime * 3600.f * 2.f; }
	
	//	�h���[���̉����t���O�擾
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		bool GetIsOverAccle()const { return m_bIsPassedRing; }

	//�X�e�[�W�p�X�擾
	UFUNCTION(BlueprintCallable, Category = "SaveFilePath")
		void SetStagePath(FString _StagePath) { m_SaveStageFolderPath = _StagePath; }
	//���[�J�������擾
	FVector GetLocalAxis()const { return m_LocalAxis; }

	//���͒l�̐ݒ�
	UFUNCTION(BlueprintCallable)
		void SetAxisValue(const FVector4& NewAxisValue) { m_AxisValuePerFrame = NewAxisValue; }
	//���͒l�̎擾
	UFUNCTION(BlueprintCallable)
		FVector4 GetAxisValue()const { return m_AxisValuePerFrame; }

protected:
	//���b�V���A�Z�b�g�̃Z�b�g�A�b�v
	virtual void MeshAssetSetup();
	//�R���W�����̏����ݒ�
	virtual void InitializeCollision();
	//���b�V���̏����ݒ�
	virtual void InitializeMesh();
	UFUNCTION(Client, Reliable)
		virtual void Client_InitializeMesh();
	//���C�g�̏����ݒ�
	virtual void InitializeLight();
	UFUNCTION(Client, Reliable)
		virtual void Client_InitializeLight();

	//�H�̉����x�X�V����
	virtual void UpdateWingAccle(const float& DeltaTime);
	//��]����
	virtual void UpdateRotation(const float& DeltaTime);
	//���x�X�V����
	virtual void UpdateSpeed(const float& DeltaTime);
	//�H�̉�]�X�V����
	virtual void UpdateWingRotation(const float& DeltaTime);
	//���̃G�t�F�N�g�X�V����
	virtual void UpdateWindEffect(const float& DeltaTime);

	//�i�s���Ɠ��͎����t�������m�F
	bool IsReverseInput(const float& _movingAxis, const float& _axisValue)const { return (_movingAxis < 0.f && 0.f < _axisValue) || (_movingAxis > 0.f && 0.f > _axisValue); }
	//���x�̏�����𒴂��Ă��邩�m�F
	void UpdateAltitudeCheck(const float& DeltaTime);

	//�����̃G�t�F�N�g�̕\���ؑ�
	void UpdateCloudOfDustEffect();

protected:
	//-------------------------------------------------------------------------------------------------------
	//BODY
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMesh* m_BodyMesh;													//�@�̂̃��b�V��
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;
	//�h���[���̃R���W����
	UPROPERTY(EditAnywhere, Category = "Collision")
		USphereComponent* m_pDroneCollision;
	//�H
	UPROPERTY(EditAnywhere, EditFixedSize, Category = "Mesh|Wing")
		TArray<UStaticMesh*> m_WingMesh;													//�H�̃��b�V��
	TArray<TSharedPtr<FWing>>  m_pWings;												//�H���Ǘ�����\����
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

	UPROPERTY(EditAnywhere, Category = "Drone", Replicated/*, ReplicatedUsing = OnRep_m_isControl*/)
		bool m_isControl;								//����\�t���O

	UPROPERTY(EditAnywhere, Category = "Drone")
		FVector4 m_AxisValuePerFrame;													//���t���[���X�V�������͂̒l

	FVector m_LocalAxis;																//�h���[���̃��[�J����
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

	TArray<TArray<FString>> m_SaveVelocityText;			//�ǂݍ��񂾖��t���[���̈ړ��ʂ��i�[����z��
	TArray<TArray<FString>> m_SaveQuatText;				//�ǂݍ��񂾖��t���[���̉�]�ʂ��i�[����z��
	UPROPERTY(EditAnywhere, Category = "Drone")
		int m_PlaybackFlame;
	UPROPERTY(EditAnywhere, Category = "Drone")
		int m_PlayableFramesNum;

	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		FString m_SaveRecordFolderPath;									//���R�[�h�����ǂ�p�X��ݒ�
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		FString m_SaveStageFolderPath;									//�X�e�[�W�����ǂ�p�X��ݒ�
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveVelocityLoadPath;							//�ړ��ʂ��������܂ꂽ�t�@�C�������ǂ�p�X��ݒ�
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveQuatLoadPath;								//��]�ʂ��������܂ꂽ�t�@�C�������ǂ�p�X��ݒ�

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
