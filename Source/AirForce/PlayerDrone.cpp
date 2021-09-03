//------------------------------------------------------------------------
// �t�@�C����	:PlayerDrone.cpp
// �T�v				:�v���C���[�̃h���[���N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e		:
//------------------------------------------------------------------------
// �X�V��		:19CU0104 �r�c�Ĉ�Y
// �X�V���e		:2021/06/07 �h���[���̋O�ՃG�t�F�N�g��ǉ�
//				:2021/06/16 �h���[���̉H�̉�]�����̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h
#include "PlayerDrone.h"
#include "GameUtility.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h"

//�R���X�g���N�^
APlayerDrone::APlayerDrone()
	: m_GameMode(EGAMEMODE::GAMEMODE_TPS)
	, m_DroneMode(EDRONEMODE::DRONEMODE_AUTOMATICK)
	, m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(90.f)
	, m_FieldOfView(90.f)
	, m_CameraSocketOffset(FVector::ZeroVector)
	, m_CameraSocketOffsetMax(0.f)
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_pLightlineEffect(NULL)
	, m_pWindEffect(NULL)
	, m_WindRotationSpeed(5.f)
	, m_AxisValue(FVector4(0.f, 0.f, 0.f, 0.f))
{
	//���g��Tick()�𖈃t���[���Ăяo�����ǂ���
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = m_pDroneCollision;

	if (m_pBodyMesh)
	{
		m_pBodyMesh->SetupAttachment(m_pDroneCollision);
	}

	//�X�v�����O�A�[������
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_pSpringArm && m_pBodyMesh)
	{
		m_pSpringArm->SetupAttachment(m_pDroneCollision);
	}

	//�J��������
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera && m_pSpringArm)
	{
		m_pCamera->SetupAttachment(m_pSpringArm);
	}

	//�J�����̏����ݒ�
	InitializeCamera();

	//�h���[���p���C���G�t�F�N�g����
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> LineEffect(TEXT("/Game/Effect/LightLine/E_LightLine_System.E_LightLine_System"));
	if (LineEffect.Succeeded())
	{
		m_pLightlineEffect = LineEffect.Object;
	}

	//�`�F�b�N�|�C���g���w����󐶐�
	m_pWindEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ArrowEffect"));
	if (m_pWindEffect)
	{
		m_pWindEffect->SetupAttachment(m_pDroneCollision);
	}

	//�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


//�Q�[���J�n����1�x��������
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();

}

//���t���[������
void APlayerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�H�̍X�V����
	UpdateWingAccle();

	//�H�̍X�V����
	UpdateWingRotation(DeltaTime);

	//���͂̉����x�X�V����
	UpdateAxisAcceleration(DeltaTime);

	//��]����
	UpdateRotation(DeltaTime);

	//�ړ�����
	UpdateSpeed(DeltaTime);

	//�J�����̍X�V����
	UpdateCamera(DeltaTime);

	//�J�����Ƃ̎Օ����̃R���W��������
	UpdateCameraCollsion();

	//���_�̐؂�ւ�
	//SwitchViewPort();

	//���̃G�t�F�N�g�X�V����
	UpdateWindEffect(DeltaTime);
}

//�J�����̏����ݒ�
void APlayerDrone::InitializeCamera()
{
	//NULL�`�F�b�N
	if (!m_pCamera || !m_pSpringArm) { return; }
	
	//�X�v�����O�A�[���̒�����ݒ�
	m_pSpringArm->TargetArmLength = m_CameraTargetLength;
	//�X�v�����O�A�[���̉�]���O��K�p����
	m_pSpringArm->bEnableCameraRotationLag = true;
	m_pSpringArm->CameraRotationLagSpeed = 10.f;

	//�J�����̃R���W�����e�X�g���s��Ȃ��悤�ɂ���
	m_pSpringArm->bDoCollisionTest = false;

	//�J�����̎���p�̐ݒ�
	m_pCamera->SetFieldOfView(m_FieldOfView);
}

//	���_�̐؂�ւ�
void APlayerDrone::SwitchViewPort()
{

}

//�E�̃X�e�B�b�N���͂���H�̉����x�ɕϊ����鏈��
float APlayerDrone::RightInputValueToWingAcceleration(const int _arrayIndex)
{
	//�H�̉����x
	float wingsAccel[EWING::NUM] = { 0.f,0.f,0.f,0.f };

	//�X���b�g���̓��͂����鎞
	if (m_AxisValue.Z != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValue.Z;
		}
	}

	//���_�[�̓��͂����鎞
	if (m_AxisValue.W != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValue.W > 0.f ? true : false);
		wingsAccel[EWING::LEFT_FORWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? -1.f : 1.f);
		wingsAccel[EWING::LEFT_BACKWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_FORWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_BACKWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//���̃X�e�B�b�N���͂���H�̉����x�ɕϊ����鏈��
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[EWING::NUM] = { 0.f,0.f,0.f,0.f };

	//�G���x�[�^�[�̓��͂����鎞
	if (m_AxisValue.Y != 0.f)
	{
		//�O���͂�����Ă��邩�ǂ���
		const bool isForward = (m_AxisValue.Y > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValue.Y);
		wingAccel[(isForward ? EWING::LEFT_BACKWARD : EWING::LEFT_FORWARD)] += axisAbsValue;
		wingAccel[(isForward ? EWING::RIGHT_BACKWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
	}

	//�G�������̓��͂����鎞
	if (m_AxisValue.X != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValue.X > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValue.X);
		wingAccel[(isRight ? EWING::LEFT_FORWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
		wingAccel[(isRight ? EWING::LEFT_BACKWARD : EWING::RIGHT_BACKWARD)] += axisAbsValue;
	}
	return wingAccel[_arrayIndex];
}

//�H�̉����x�X�V����
void APlayerDrone::UpdateWingAccle()
{
	//�e�X�e�B�b�N�̓��͂̒l�����擾
	FVector2D RightAxis = FVector2D(m_AxisValue.W, m_AxisValue.Z);
	FVector2D LeftAxis = FVector2D(m_AxisValue.X, m_AxisValue.Y);

	//���͂��Ȃ���ΏI��
	if ((RightAxis.IsZero() && LeftAxis.IsZero()) || !m_isControl)
	{
		for (FWing& wing : m_Wings)
		{
			wing.AccelState = 0.f;
		}
		return;
	}

	//�����̓��͂�����ꍇ
	if (!RightAxis.IsZero() && !LeftAxis.IsZero())
	{
		for (FWing& wing : m_Wings)
		{
			//PPAP
			wing.AccelState = RightInputValueToWingAcceleration(wing.GetWingNumber()) + LeftInputValueToWingAcceleration(wing.GetWingNumber());
		}
		return;
	}

	//�E�X�e�B�b�N�݂̂̏ꍇ
	if (!RightAxis.IsZero())
	{
		for (FWing& wing : m_Wings)
		{
			wing.AccelState = RightInputValueToWingAcceleration(wing.GetWingNumber());
		}
	}
	//���X�e�B�b�N�݂̂̏ꍇ
	else if (!LeftAxis.IsZero())
	{
		for (FWing& wing : m_Wings)
		{
			wing.AccelState = LeftInputValueToWingAcceleration(wing.GetWingNumber());
		}
	}
}

//�H�̉�]�X�V����
void APlayerDrone::UpdateWingRotation(const float& DeltaTime)
{
	//2���̓��͗ʂ���������
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValue.W, m_AxisValue.Z).Size() +
		FVector2D(m_AxisValue.X, m_AxisValue.Y).Size()) / 2,
		0.f, 1.f);

	//���bm_rpsMax * WingAccel�񕪉�]���邽�߂ɖ��t���[���H���񂷊p�x�����߂�
	for (FWing& wing : m_Wings)
	{
		if (wing.GetWingMesh())
		{
			//�H�̉����x��0����1�͈̔͂ɏC�����A���K������
			const float NormalizeAccelSize = FMath::Clamp((wing.AccelState + 1.f) / 3.f, 0.f, 1.f);
			//���K�����������x���g���ĉH�̉����̊������Ԃ���
			const float WingAccel = FMath::Lerp(m_WingAccelMin
				, m_WingAccelMax, NormalizeAccelSize);
			//�E���̉H�����ʂ���(���O�ƉE���̉H���E���ɉ�]����)
			const bool isTurnRight = (wing.GetWingNumber() == EWING::LEFT_FORWARD || wing.GetWingNumber() == EWING::RIGHT_BACKWARD ? true : false);
			//1�t���[���ɉ�]����p�x�����߂�
			const float angularVelocity = m_RPSMax * 360.f * DeltaTime * WingAccel * (isTurnRight ? 1.f : -1.f) * MOVE_CORRECTION;

			//�H����]������
			wing.GetWingMesh()->AddLocalRotation(FRotator(0.f, angularVelocity, 0.f));

#ifdef DEBUG_WING
			//*�f�o�b�O�p*���x�ɉ����ĉH�̐F�ύX				
			const FVector WingColor = FVector(FLinearColor::LerpUsingHSV(FColor::Blue, FColor::Yellow, NormalizeAccelSize));
			wing.GetWingMesh()->SetVectorParameterValueOnMaterials(TEXT("WingColor"), WingColor);
#endif // DEBUG_WING
		}
	}
}

//���͂̉����x�X�V����
void APlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
{
	for (int i = 0; i < EINPUT_AXIS::NUM; i++)
	{
		//���͂��z�o�����O��Ԃ��傫���Ƃ�
		if (m_AxisValue[i] > 0.f)
		{
			if (m_AxisAccel[i] < m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//���͂��z�o�����O��Ԃ�菬������
		else if (m_AxisValue[i] < 0.f)
		{
			if (m_AxisAccel[i] > -m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//���͂��d�͂ƒނ荇����(�z�o�����O���)
		else
		{
			if (m_AxisAccel[i] == m_AxisAccel.W)
			{
				m_AxisAccel[i] = 0.f;
			}
				
			m_AxisAccel[i] *= m_Deceleration;
		}

		//m_AxisAccel[i] = CGameUtility::SetDecimalTruncation(m_AxisAccel[i], 3);
		m_AxisAccel[i] = FMath::Clamp(m_AxisAccel[i], -m_WingAccelMax, m_WingAccelMax);
	}
}

//�h���[���̉�]����
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);

	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }

	//�H�̉�]�ʂ���h���[���̊p���x�̍ő�l��ݒ�
	m_AngularVelocity = FVector(
		(m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState),
		(m_Wings[EWING::LEFT_BACKWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_FORWARD].AccelState),
		(m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState));
	m_AngularVelocity.Z = FMath::Abs(m_AngularVelocity.Z) * m_AxisAccel.W;

	//	�I�[�g�}�`�b�N�ő��삷��Ƃ�
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float deg = 15.f;
		//�O��Ƀh���[�����X�������Ȃ��悤�ɕ␳
		if (((m_pBodyMesh->GetRelativeRotation().Pitch < -deg) && (m_AngularVelocity.Y < 0.f) )|| ( (m_pBodyMesh->GetRelativeRotation().Pitch > deg) && (m_AngularVelocity.Y > 0.f)))
		{
			m_AngularVelocity.Y = 0.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Pitch > 0.f) && (m_AngularVelocity.Y == 0.f))
		{
			m_AngularVelocity.Y = -1.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Pitch < 0.f) && (m_AngularVelocity.Y == 0.f))
		{
			m_AngularVelocity.Y = 1.f;
		}

		//���E�Ƀh���[�����X�������Ȃ��悤�ɕ␳
		if ((m_pBodyMesh->GetRelativeRotation().Roll < -deg) && (m_AngularVelocity.X < 0.f) || (m_pBodyMesh->GetRelativeRotation().Roll > deg) && (m_AngularVelocity.X > 0.f))
		{
			m_AngularVelocity.X = 0.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Roll > 0.f) && (m_AngularVelocity.X == 0.f))
		{
			m_AngularVelocity.X = -1.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Roll < 0.f) && (m_AngularVelocity.X == 0.f))
		{
			m_AngularVelocity.X = 1.f;
		}
	}

	//�A�}�`���A�ő��삷��Ƃ�
	else
	{
		//�p���x�̎擾(yaw���͊܂߂Ȃ�)
		float angularVelocity = FVector(m_AngularVelocity.X, m_AngularVelocity.Y, 0.f).Size();
		//�p���x��rad�ɕϊ�
		float radAngularVelocity = FMath::DegreesToRadians(angularVelocity);
		//�d�� / �p���x ^ 2�Ńh���[�����~�^������Ƃ��̔��a�����߂�
		float radius = m_Gravity.Size() / (radAngularVelocity * radAngularVelocity);
		//���a * �p���x ^ 2�ŉ��S�͂��擾
		if (m_AngularVelocity != FVector::ZeroVector)
		{
			m_CentrifugalForce = FVector(0.f, 0.f, radius * (radAngularVelocity * radAngularVelocity));
		}
		else
		{
			m_CentrifugalForce = FVector::ZeroVector;
		}
	}

	//�I�C���[�p���N�H�[�^�j�I���ɕϊ�
	FQuat qAngularVelocity = FQuat::MakeFromEuler(m_AngularVelocity);
	//�h���[������]������
	//UE_LOG(LogTemp, Warning, TEXT("qAngularVelocity:%s"), *qAngularVelocity.ToString());
	m_pBodyMesh->AddLocalRotation(qAngularVelocity * MOVE_CORRECTION, true);
}

//���x�X�V����
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{

	//�I�[�g�}�`�b�N�ő��삷��Ƃ�
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		FRotator BodyRotation = m_pBodyMesh->GetComponentRotation();
		BodyRotation.Pitch = 0.f;
		BodyRotation.Roll = 0.f;

		FQuat BodyQuat = BodyRotation.Quaternion();

		float speed = 3.5f;
		m_Velocity = FVector::ZeroVector;
		m_Velocity += BodyQuat.GetRightVector() * speed * m_AxisAccel.X;
		m_Velocity += BodyQuat.GetForwardVector() * speed * -m_AxisAccel.Y;
		m_Velocity += BodyQuat.GetUpVector() * speed * m_AxisAccel.Z;
		m_Speed = m_Velocity.Size();
		AddActorWorldOffset(m_Velocity * MOVE_CORRECTION, true);
	}
	else
	{
		Super::UpdateSpeed(DeltaTime);
	}
}

//�X�e�[�g�X�V����
void APlayerDrone::UpdateState()
{

}

//�J�����X�V����
void APlayerDrone::UpdateCamera(const float& DeltaTime)
{
	//NULL�`�F�b�N
	if (!m_pCamera || !m_pSpringArm || !m_pBodyMesh) { return; }

#ifdef DEBUG_CAMERA
	//�J�����̑O���Ƀ��C���΂�
	//DrawDebugLine(GetWorld(),
	//	m_pCamera->GetComponentLocation(),
	//	m_pCamera->GetComponentLocation() + m_pCamera->GetForwardVector() * 1000.f,
	//	FColor::Blue,
	//	false,
	//	DeltaTime + 2.f);

	//�v���C���[�̑O���Ƀ��C���΂�
	DrawDebugLine(GetWorld(),
		m_pBodyMesh->GetComponentLocation(),
		m_pBodyMesh->GetComponentLocation() + m_pBodyMesh->GetForwardVector() * 1000.f,
		FColor::Red,
		false,
		DeltaTime + 0.05f);

#endif // DEBUG_CAMERA

	FRotator BodyRotation = m_pBodyMesh->GetComponentRotation();
	BodyRotation.Pitch = 0.f;
	BodyRotation.Roll = 0.f;
	FQuat BodyQuat = BodyRotation.Quaternion();

	FVector CameraLocation = m_pCamera->GetComponentLocation();
	FVector DroneLocation = GetActorLocation() + BodyQuat.GetForwardVector() * 100000000.f;
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - CameraLocation).Rotator();
	FRotator CameraRotation = m_pCamera->GetComponentRotation();
	FRotator NewRotation = FMath::RInterpTo(CameraRotation, LookAtRotation, DeltaTime, 10.f);
	NewRotation.Roll = 0.f;

	FRotator Camera = FRotator::ZeroRotator;
	//NewRotation.Pitch = m_pBodyMesh->GetComponentRotation().Pitch * -1.f;
	//Camera.Pitch = m_pBodyMesh->GetRelativeRotation().Pitch * -1.f;
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		//Camera.Roll = m_pBodyMesh->GetRelativeRotation().Roll * -1.f;
	}

	//m_pSpringArm->SetRelativeRotation(Camera.Quaternion());
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, m_pBodyMesh->GetRelativeRotation().Yaw, 0.f));
	m_pCamera->SetWorldRotation(NewRotation.Quaternion());

	//�\�P�b�g
	m_pSpringArm->SocketOffset = FVector(m_AxisAccel.Y, m_AxisAccel.X, 0.f) * m_CameraSocketOffsetMax / m_WingAccelMax;
}

//�J�����Ƃ̎Օ����̃R���W��������
void  APlayerDrone::UpdateCameraCollsion()
{

}

//���̃G�t�F�N�g�̍X�V����
void APlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	if (m_pWindEffect) 
	{
		//�G�t�F�N�g�ƃ`�F�b�N�|�C���g�̍��W���擾
		FVector EffectLocation = m_pWindEffect->GetComponentLocation();
		FVector  Direction = EffectLocation + m_Velocity.GetSafeNormal();
		//�G�t�F�N�g���i�s�����֌����悤�ɂ���
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction - EffectLocation).Rotator();
		//�ړ��ʂ̑傫������G�t�F�N�g�̕s�����x��ݒ�
		FVector Accel = m_AxisAccel;
		float AccelRate = FMath::Clamp(Accel.Size() / m_WingAccelMax, 0.f, 1.f);
		float WindOpacity = FMath::Lerp(0.f, 1.f, AccelRate);

		//��]����
		m_pWindEffect->SetWorldRotation(LookAtRotation.Quaternion() * MOVE_CORRECTION);
		//�G�t�F�N�g�̕s�����x��ύX
		m_pWindEffect->SetVariableFloat(TEXT("User.WindOpacity"), WindOpacity);
	}
#ifdef DEBUG_WindEffect
	else
	{
		//NULL�������ꍇ���O�\��
		UE_LOG(LogTemp, Error, TEXT("NULL:m_pWindEffect"));
	}
#endif // DEBUG_WindEffect
}

//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
void APlayerDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//���}�b�s���O
	InputComponent->BindAxis(TEXT("Throttle"), this, &APlayerDrone::Input_Throttle);
	InputComponent->BindAxis(TEXT("Elevator"), this, &APlayerDrone::Input_Elevator);
	InputComponent->BindAxis(TEXT("Aileron"), this, &APlayerDrone::Input_Aileron);
	InputComponent->BindAxis(TEXT("Ladder"), this, &APlayerDrone::Input_Ladder);
}

//�y���̓o�C���h�z�X���b�g��(�㉺)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Input_Throttle(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.Z = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.Z = 0.f;
	}
		
	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue.Z > 0.f)
	{
		//�㏸�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Up = true;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue.Z < 0.f)
	{
		//���~�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = true;
	}
	//�l�����͂���Ă��Ȃ��Ȃ�
	else
	{
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
}

//�y���̓o�C���h�z�G���x�[�^�[(�O��)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Input_Elevator(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.Y = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.Y = 0.f;
	}
		
	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue.Y > 0.f)
	{
		//�O���ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Forward = true;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue.Y < 0.f)
	{
		//����ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Forward = false;
		m_MoveDirectionFlag.sFlag.Backward = true;
	}
	else
	{
		m_MoveDirectionFlag.sFlag.Forward = false;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
}

//�y���̓o�C���h�z�G������(���E)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Input_Aileron(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.X = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.X = 0.f;
	}

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue.X > 0.f)
	{
		//�E�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Right = true;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue.X < 0.f)
	{
		//���ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Right = false;
		m_MoveDirectionFlag.sFlag.Left = true;
	}
	//�l�����͂���Ă��Ȃ��Ȃ�
	else
	{
		m_MoveDirectionFlag.sFlag.Right = false;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
}

//�y���̓o�C���h�z���_�[(����)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Input_Ladder(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.W = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.W = 0.f;
	}
		
	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue.W > 0.f)
	{
		//�E����t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.RightTurning = true;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue.W < 0.f)
	{
		//������t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.RightTurning = false;
		m_MoveDirectionFlag.sFlag.LeftTurning = true;
	}
	//�l�����͂���Ă��Ȃ��Ȃ�
	else
	{
		m_MoveDirectionFlag.sFlag.RightTurning = false;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
}