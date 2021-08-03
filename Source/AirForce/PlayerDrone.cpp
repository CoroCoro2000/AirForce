   //------------------------------------------------------------------------
// �t�@�C����	:PlayerDrone.cpp
// �T�v				:�v���C���[�̃h���[���N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e		:
//------------------------------------------------------------------------
// �X�V��			:19CU0104 �r�c�Ĉ�Y
// �X�V���e		:2021/06/07 �h���[���̋O�ՃG�t�F�N�g��ǉ�
//						:2021/06/16 �h���[���̉H�̉�]�����̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h
#include "PlayerDrone.h"
#include "DroneBullet.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"

//#define DEBUG
//#define DEBUG_WING			//�H�̃f�o�b�O
//#define DEBUG_CAMERA			//�H�̃f�o�b�O

//�R���X�g���N�^
APlayerDrone::APlayerDrone()
	: m_GameMode(GAMEMODE::GAMEMODE_TPS)
	, m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(60.f)
	, m_FieldOfView(90.f)
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

	for (FWing* pWing : m_pWings)
	{
		//�̈�̊J��
		if (pWing)
		{
			delete pWing;
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
	if (m_AxisValue[(int)INPUT_AXIS::THROTTLE] != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValue[(int)INPUT_AXIS::THROTTLE];
		}
	}

	//���_�[�̓��͂����鎞
	if (m_AxisValue[(int)INPUT_AXIS::LADDER] != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValue[(int)INPUT_AXIS::LADDER] > 0.f ? true : false);
		wingsAccel[LF_WING] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::LADDER]) * (isRight ? -1.f : 1.f);
		wingsAccel[LB_WING] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::LADDER]) * (isRight ? 1.f : -1.f);
		wingsAccel[RF_WING] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::LADDER]) * (isRight ? 1.f : -1.f);
		wingsAccel[RB_WING] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::LADDER]) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//���̃X�e�B�b�N���͂���H�̉����x�ɕϊ����鏈��
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[WING_ARRAY_MAX] = { 0.f,0.f,0.f,0.f };

	//�G���x�[�^�[�̓��͂����鎞
	if (m_AxisValue[(int)INPUT_AXIS::ELEVATOR] != 0.f)
	{
		//�O���͂�����Ă��邩�ǂ���
		const bool isForward = (m_AxisValue[(int)INPUT_AXIS::ELEVATOR] > 0.f ? true : false);
		wingAccel[(isForward ? LB_WING : LF_WING)] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::ELEVATOR]);
		wingAccel[(isForward ? RB_WING : RF_WING)] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::ELEVATOR]);
	}

	//�G�������̓��͂����鎞
	if (m_AxisValue[(int)INPUT_AXIS::AILERON] != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValue[(int)INPUT_AXIS::AILERON] > 0.f ? true : false);
		wingAccel[(isRight ? LF_WING : RF_WING)] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::AILERON]);
		wingAccel[(isRight ? LB_WING : RB_WING)] += FMath::Abs(m_AxisValue[(int)INPUT_AXIS::AILERON]);
	}
	return wingAccel[_arrayIndex];
}

//�H�̉����x�X�V����
void APlayerDrone::UpdateWingAccle()
{
	//�e�X�e�B�b�N�̓��͂̒l�����擾
	FVector2D RightAxis = FVector2D(m_AxisValue[(int)INPUT_AXIS::LADDER], m_AxisValue[(int)INPUT_AXIS::THROTTLE]);
	FVector2D LeftAxis = FVector2D(m_AxisValue[(int)INPUT_AXIS::AILERON], m_AxisValue[(int)INPUT_AXIS::ELEVATOR]);

	//���͂��Ȃ���ΏI��
	if ((RightAxis.IsZero() && LeftAxis.IsZero()) || !m_isControl)
	{
		for (FWing* pWing : m_pWings)
		{
			if (pWing) 
			{
				pWing->AccelState = 0.f;
			}
		}
		return;
	}

	//�����̓��͂�����ꍇ
	if (!RightAxis.IsZero() && !LeftAxis.IsZero())
	{
		for (FWing* pWing : m_pWings)
		{
			if (pWing)
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
		for (FWing* pWing : m_pWings)
		{
			if (pWing)
			{
				pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
	}
	//���X�e�B�b�N�݂̂̏ꍇ
	else if (!LeftAxis.IsZero())
	{
		for (FWing* pWing : m_pWings)
		{
			if (pWing)
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
		FVector2D(m_AxisValue[(int)INPUT_AXIS::LADDER], m_AxisValue[(int)INPUT_AXIS::THROTTLE]).Size() +
		FVector2D(m_AxisValue[(int)INPUT_AXIS::AILERON], m_AxisValue[(int)INPUT_AXIS::ELEVATOR]).Size()) / 2,
		0.f, 1.f);

	//���bm_rpsMax * WingAccel�񕪉�]���邽�߂ɖ��t���[���H���񂷊p�x�����߂�
	for (FWing* pWing : m_pWings)
	{
		if (pWing)
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

//�h���[���̉�]����
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	Super::UpdateRotation(DeltaTime);

	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }
	for (FWing* pWing : m_pWings)
	{
		if (!pWing) { return; }
	}

	//�H�̉�]�ʂ���h���[���̊p���x�̍ő�l��ݒ�
	m_AngularVelocity = FVector(
		(m_pWings[LF_WING]->AccelState + m_pWings[LB_WING]->AccelState) - (m_pWings[RF_WING]->AccelState + m_pWings[RB_WING]->AccelState),
		(m_pWings[LB_WING]->AccelState + m_pWings[RB_WING]->AccelState) - (m_pWings[LF_WING]->AccelState + m_pWings[RF_WING]->AccelState),
		(m_pWings[RF_WING]->AccelState + m_pWings[LB_WING]->AccelState) - (m_pWings[LF_WING]->AccelState + m_pWings[RB_WING]->AccelState));
	m_AngularVelocity.Z *= 0.5f;

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

	//�I�C���[�p���N�H�[�^�j�I���ɕϊ�
	FQuat qAngularVelocity = FQuat::MakeFromEuler(m_AngularVelocity * 1.5f);
	//�h���[������]������
	m_pBodyMesh->AddLocalRotation(qAngularVelocity * MOVE_CORRECTION, true);
}

//���x�X�V����
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	Super::UpdateSpeed(DeltaTime);

	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }

	//���͂̑傫��
	float Buoyancy = 0.f;
	for (FWing* pWing : m_pWings)
	{
		if (pWing)
		{
			Buoyancy += pWing->AccelState;
		}
	}
	Buoyancy /= (float)WING_ARRAY_MAX;
	

	if (m_pBodyMesh)
	{
		//�h���[���̏�����x�N�g��*�H�̉�]��*�d�͂𐄐i�͂ɐݒ�
		FVector Propulsion = GetActorUpVector() * (Buoyancy + 1.f) * Gravity.Size();

		//�H�̉����x������Ȃ�
		if (Buoyancy + 1.f > 1.f)
		{
			if (m_Acceleration < 7.f)
			{
				m_Acceleration += Buoyancy * DeltaTime;
			}
		}
		else
		{
			m_Acceleration *= 58.5f  * DeltaTime;
		}
		




		//FVector direction = GetActorUpVector();

		FVector d = m_pBodyMesh->GetUpVector() * m_Acceleration;

		//�h���[�����b�V���̂̏�����̃x�N�g���ɐ��i�͂̐ݒ�
		AddActorWorldOffset(d, true);
		m_Speed = d.Size();
		//���[���h���W�̉������̃x�N�g���ɏd�͂�ݒ�
		AddActorWorldOffset(Gravity, true);
		//�h���[�����b�V���̂̌����̃x�N�g���ɉ��S�͂�ݒ�
		//AddActorLocalOffset(Centrifugalforce, true);
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
	m_pSpringArm->SetRelativeRotation(Camera.Quaternion());
	//UE_LOG(LogTemp, Warning, TEXT("W"));

	//FRotator CameraR = FRotator(
	//	-15.f * m_AxisValue[(int)INPUT_AXIS::ELEVATOR],
	//	0.f,
	//	0.f);

	//m_pCamera->SetRelativeRotation(CameraR.Quaternion());



	//m_pCamera->SetRelativeLocation(BodyBackward * 40.f);
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
	m_AxisValue[(int)INPUT_AXIS::THROTTLE] = FMath::Clamp(_axisValue, -1.f, 1.f);

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[(int)INPUT_AXIS::THROTTLE] > 0.f)
	{
		//�㏸�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Up = true;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[(int)INPUT_AXIS::THROTTLE] < 0.f)
	{
		//���~�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = true;
	}
	//�l�����͂���Ă��Ȃ��Ȃ�
	else
	{
		m_AxisValue[(int)INPUT_AXIS::THROTTLE] = 0.f;
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
}

//�y���̓o�C���h�z�G���x�[�^�[(�O��)�̓��͂��������ꍇ�Ăяo�����֐�
void APlayerDrone::Drone_Elevator(float _axisValue)
{
	m_AxisValue[(int)INPUT_AXIS::ELEVATOR] = FMath::Clamp(_axisValue, -1.f, 1.f);

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[(int)INPUT_AXIS::ELEVATOR] > 0.f)
	{
		//�O���ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Forward = true;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[(int)INPUT_AXIS::ELEVATOR] < 0.f)
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
	m_AxisValue[(int)INPUT_AXIS::AILERON] = FMath::Clamp(_axisValue, -1.f, 1.f);

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[(int)INPUT_AXIS::AILERON] > 0.f)
	{
		//�E�ړ��t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.Right = true;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[(int)INPUT_AXIS::AILERON] < 0.f)
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
	m_AxisValue[(int)INPUT_AXIS::LADDER] = FMath::Clamp(_axisValue, -1.f, 1.f);

	//���͂��ꂽ�l�����Ȃ�
	if (m_AxisValue[(int)INPUT_AXIS::LADDER] > 0.f)
	{
		//�E����t���O�𗧂Ă�
		m_MoveDirectionFlag.sFlag.RightTurning = true;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
	//���͂��ꂽ�l�����Ȃ�
	else if (m_AxisValue[(int)INPUT_AXIS::LADDER] < 0.f)
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