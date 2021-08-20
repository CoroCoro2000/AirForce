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
#include "DroneBase.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"
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
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_pLightlineEffect(NULL)
	, m_bCanControl(true)
	, m_AxisValue{ 0.f, 0.f, 0.f, 0.f }
{
	//���g��Tick()�𖈃t���[���Ăяo�����ǂ���
	PrimaryActorTick.bCanEverTick = true;

	//�h���[���p�R���W��������
	m_pDroneBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerDroneComp"));
	if (m_pDroneBoxComp && m_pBodyMesh)
	{
		m_pDroneBoxComp->SetupAttachment(m_pBodyMesh);
		RootComponent = m_pBodyMesh;
	}

	//�X�v�����O�A�[������
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_pSpringArm && m_pBodyMesh)
	{
		m_pSpringArm->SetupAttachment(m_pBodyMesh);
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

	//�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


//�Q�[���J�n����1�x��������
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();
}

//���̃I�u�W�F�N�g���j�������Ƃ��ɌĂяo�����֐�
void APlayerDrone::BeginDestory()
{
	Super::BeginDestroy();

	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		//�̈�̊J��
		if (pWing.IsValid())
		{
			pWing.Reset();
		}
	}
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

	//�d�S�ړ�����
	UpdateCenterOfGravity(DeltaTime);

	//�J�����̍X�V����
	UpdateCamera(DeltaTime);

	//�J�����Ƃ̎Օ����̃R���W��������
	UpdateCameraCollsion();

	//���_�̐؂�ւ�
	//SwitchViewPort();
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
	float wingsAccel[WING_ARRAY_MAX] = { 0.f,0.f,0.f,0.f };

	//�X���b�g���̓��͂����鎞
	if (m_AxisValue[EINPUT_AXIS::THROTTLE] != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValue[EINPUT_AXIS::THROTTLE];
		}
	}

	//���_�[�̓��͂����鎞
	if (m_AxisValue[EINPUT_AXIS::LADDER] != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValue[EINPUT_AXIS::LADDER] > 0.f ? true : false);
		wingsAccel[LF_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? -1.f : 1.f);
		wingsAccel[LB_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? 1.f : -1.f);
		wingsAccel[RF_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? 1.f : -1.f);
		wingsAccel[RB_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//���̃X�e�B�b�N���͂���H�̉����x�ɕϊ����鏈��
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[WING_ARRAY_MAX] = { 0.f,0.f,0.f,0.f };

	//�G���x�[�^�[�̓��͂����鎞
	if (m_AxisValue[EINPUT_AXIS::ELEVATOR] != 0.f)
	{
		//�O���͂�����Ă��邩�ǂ���
		const bool isForward = (m_AxisValue[EINPUT_AXIS::ELEVATOR] > 0.f ? true : false);
		wingAccel[(isForward ? LB_WING : LF_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::ELEVATOR]);
		wingAccel[(isForward ? RB_WING : RF_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::ELEVATOR]);
	}

	//�G�������̓��͂����鎞
	if (m_AxisValue[EINPUT_AXIS::AILERON] != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValue[EINPUT_AXIS::AILERON] > 0.f ? true : false);
		wingAccel[(isRight ? LF_WING : RF_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::AILERON]);
		wingAccel[(isRight ? LB_WING : RB_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::AILERON]);
	}
	return wingAccel[_arrayIndex];
}

//�H�̉����x�X�V����
void APlayerDrone::UpdateWingAccle()
{
	//�e�X�e�B�b�N�̓��͂̒l�����擾
	FVector2D RightAxis = FVector2D(m_AxisValue[EINPUT_AXIS::LADDER], m_AxisValue[EINPUT_AXIS::THROTTLE]);
	FVector2D LeftAxis = FVector2D(m_AxisValue[EINPUT_AXIS::AILERON], m_AxisValue[EINPUT_AXIS::ELEVATOR]);

	//���͂��Ȃ���ΏI��
	if ((RightAxis.IsZero() && LeftAxis.IsZero()) || !m_isControl)
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid()) 
			{
				pWing->AccelState = 0.f;
			}
		}
		return;
	}

	//�����̓��͂�����ꍇ
	if (!RightAxis.IsZero() && !LeftAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid())
			{
				//PPAP
				pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber()) + LeftInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
		return;
	}

	//�E�X�e�B�b�N�݂̂̏ꍇ
	if (!RightAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid())
			{
				pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
	}
	//���X�e�B�b�N�݂̂̏ꍇ
	else if (!LeftAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid())
			{
				pWing->AccelState = LeftInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
	}
}

//�H�̉�]�X�V����
void APlayerDrone::UpdateWingRotation(const float& DeltaTime)
{
	//2���̓��͗ʂ���������
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValue[EINPUT_AXIS::LADDER], m_AxisValue[EINPUT_AXIS::THROTTLE]).Size() +
		FVector2D(m_AxisValue[EINPUT_AXIS::AILERON], m_AxisValue[EINPUT_AXIS::ELEVATOR]).Size()) / 2,
		0.f, 1.f);

	//���bm_rpsMax * WingAccel�񕪉�]���邽�߂ɖ��t���[���H���񂷊p�x�����߂�
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (pWing.IsValid())
		{
			//�H�̉����x��0����1�͈̔͂ɏC�����A���K������
			const float NormalizeAccelSize = FMath::Clamp((pWing->AccelState + 1.f) / 3.f, 0.f, 1.f);
			//���K�����������x���g���ĉH�̉����̊������Ԃ���
			const float WingAccel = FMath::Lerp(m_WingAccelMin
, m_WingAccelMax, NormalizeAccelSize);
			//�E���̉H�����ʂ���(���O�ƉE���̉H���E���ɉ�]����)
			const bool isTurnRight = (pWing->GetWingNumber() == LF_WING || pWing->GetWingNumber() == RB_WING ? true : false);
			//1�t���[���ɉ�]����p�x�����߂�
			const float angularVelocity = m_rpsMax * 360.f * DeltaTime * WingAccel * (isTurnRight ? 1.f : -1.f) * MOVE_CORRECTION;

			if (pWing->GetWingMesh())
			{
				//�H����]������
				pWing->GetWingMesh()->AddLocalRotation(FRotator(0.f, angularVelocity, 0.f));

#ifdef DEBUG_WING
				//*�f�o�b�O�p*���x�ɉ����ĉH�̐F�ύX				
				const FVector WingColor = FVector(FLinearColor::LerpUsingHSV(FColor::Blue, FColor::Yellow, NormalizeAccelSize));
				pWing->GetWingMesh()->SetVectorParameterValueOnMaterials(TEXT("WingColor"), WingColor);
#endif // DEBUG_WING
			}
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
			if (m_AxisAcceleration[i] < 1.5f)
			{
				m_AxisAcceleration[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//���͂��z�o�����O��Ԃ�菬������
		else if (m_AxisValue[i] < 0.f)
		{
			if (m_AxisAcceleration[i] > -1.5f)
			{
				m_AxisAcceleration[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//���͂��d�͂ƒނ荇����(�z�o�����O���)
		else
		{
			if (m_AxisAcceleration[i] == m_AxisAcceleration[EINPUT_AXIS::LADDER])
				m_AxisAcceleration[i] = 0.f;

			m_AxisAcceleration[i] *= 0.98f;
			m_AxisAcceleration[i] = SetDecimalTruncation(m_AxisAcceleration[i], 3.f);
		}

		m_AxisAcceleration[i] = FMath::Clamp(m_AxisAcceleration[i], -1.5f, 1.5f);
	}
}

//�h���[���̉�]����
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);

	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (!pWing.IsValid()) { return; }
	}

	//�H�̉�]�ʂ���h���[���̊p���x�̍ő�l��ݒ�
	m_AngularVelocity = FVector(
		(m_pWings[LF_WING]->AccelState + m_pWings[LB_WING]->AccelState) - (m_pWings[RF_WING]->AccelState + m_pWings[RB_WING]->AccelState),
		(m_pWings[LB_WING]->AccelState + m_pWings[RB_WING]->AccelState) - (m_pWings[LF_WING]->AccelState + m_pWings[RF_WING]->AccelState),
		(m_pWings[RF_WING]->AccelState + m_pWings[LB_WING]->AccelState) - (m_pWings[LF_WING]->AccelState + m_pWings[RB_WING]->AccelState));
	m_AngularVelocity.Z = FMath::Abs(m_AngularVelocity.Z) * m_AxisAcceleration[EINPUT_AXIS::LADDER];

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
		float radius = Gravity.Size() / (radAngularVelocity * radAngularVelocity);
		//���a * �p���x ^ 2�ŉ��S�͂��擾
		if (m_AngularVelocity != FVector::ZeroVector)
		{
			Centrifugalforce = FVector(0.f, 0.f, radius * (radAngularVelocity * radAngularVelocity));
		}
		else
		{
			Centrifugalforce = FVector::ZeroVector;
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
	//Super::UpdateSpeed(DeltaTime);

	//�I�[�g�}�`�b�N�ő��삷��Ƃ�
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float speed = 3.5f;
		FVector Auto = FVector::ZeroVector;
		Auto += m_pCamera->GetForwardVector() * speed * -m_AxisAcceleration[EINPUT_AXIS::ELEVATOR];
		Auto += m_pCamera->GetRightVector() * speed * m_AxisAcceleration[EINPUT_AXIS::AILERON];
		Auto += m_pCamera->GetUpVector() * speed * m_AxisAcceleration[EINPUT_AXIS::THROTTLE];
		m_Speed = Auto.Size();
		AddActorWorldOffset(Auto * MOVE_CORRECTION, true);
	}
}

//�d�S�ړ�����
void APlayerDrone::UpdateCenterOfGravity(const float& DeltaTime)
{
	Super::UpdateCenterOfGravity(DeltaTime);
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

	FRotator Camera = FRotator::ZeroRotator;
	Camera.Pitch = GetActorRotation().Pitch * -1.f;
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
		Camera.Roll = GetActorRotation().Roll * -1.f;
	FVector Direction = m_pBodyMesh->GetUpVector();

	m_pSpringArm->SetRelativeRotation(Camera.Quaternion());


}

//�J�����Ƃ̎Օ����̃R���W��������
void  APlayerDrone::UpdateCameraCollsion()
{
}

//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
void APlayerDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//���}�b�s���O
	InputComponent->BindAxis(TEXT("Throttle"), this, &APlayerDrone::Drone_Throttle);
	InputComponent->BindAxis(TEXT("Elevator"), this, &APlayerDrone::Drone_Elevator);
	InputComponent->BindAxis(TEXT("Aileron"), this, &APlayerDrone::Drone_Aileron);
	InputComponent->BindAxis(TEXT("Ladder"), this, &APlayerDrone::Drone_Ladder);
}

//�y���̓o�C���h�z�X���b�g��(�㉺)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Drone_Throttle(float _axisValue)
{
	if (m_isControl)
		m_AxisValue[EINPUT_AXIS::THROTTLE] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::THROTTLE] = 0.f;

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[EINPUT_AXIS::THROTTLE] > 0.f)
	{
		//�㏸�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Up = true;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[EINPUT_AXIS::THROTTLE] < 0.f)
	{
		//���~�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = true;
	}
	//�l�����͂���Ă��Ȃ��Ȃ�
	else
	{
		m_AxisValue[EINPUT_AXIS::THROTTLE] = 0.f;
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
}

//�y���̓o�C���h�z�G���x�[�^�[(�O��)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Drone_Elevator(float _axisValue)
{
	if (m_isControl)
		m_AxisValue[EINPUT_AXIS::ELEVATOR] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::ELEVATOR] = 0.f;

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[EINPUT_AXIS::ELEVATOR] > 0.f)
	{
		//�O���ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Forward = true;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[EINPUT_AXIS::ELEVATOR] < 0.f)
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
void APlayerDrone::Drone_Aileron(float _axisValue)
{
	if (m_isControl)
		m_AxisValue[EINPUT_AXIS::AILERON] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::AILERON] = 0.f;

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[EINPUT_AXIS::AILERON] > 0.f)
	{
		//�E�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Right = true;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[EINPUT_AXIS::AILERON] < 0.f)
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
void APlayerDrone::Drone_Ladder(float _axisValue)
{
	if(m_isControl)
		m_AxisValue[EINPUT_AXIS::LADDER] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::LADDER] = 0.f;

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[EINPUT_AXIS::LADDER] > 0.f)
	{
		//�E����t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.RightTurning = true;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[EINPUT_AXIS::LADDER] < 0.f)
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