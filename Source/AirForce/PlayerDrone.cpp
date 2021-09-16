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
#include "Misc/FileHelper.h"
#include "DrawDebugHelpers.h"

//�R���X�g���N�^
APlayerDrone::APlayerDrone()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(90.f)
	, m_FieldOfView(90.f)
	, m_CameraSocketOffset(FVector::ZeroVector)
	, m_CameraSocketOffsetMax(FVector(30.f, 45.f, 30.f))
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_pLightlineEffect(NULL)
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
	m_pWindEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindEffect"));
	if (m_pWindEffect)
	{
		m_pWindEffect->SetupAttachment(m_pCamera);
	}

	//�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


//�Q�[���J�n����1�x��������
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.TickInterval = 0.f;
}

//���t���[������
void APlayerDrone::Tick(float DeltaTime)
{
	//���͗ʂ��t���[���ɓ���
	m_AxisValuePerFrame = m_AxisValue;
	

	//�H�̍X�V����
	UpdateWingAccle(DeltaTime);

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
	if (m_AxisValuePerFrame.Z != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValuePerFrame.Z;
		}
	}

	//���_�[�̓��͂����鎞
	if (m_AxisValuePerFrame.W != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValuePerFrame.W > 0.f ? true : false);
		wingsAccel[EWING::LEFT_FORWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? -1.f : 1.f);
		wingsAccel[EWING::LEFT_BACKWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_FORWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_BACKWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//���̃X�e�B�b�N���͂���H�̉����x�ɕϊ����鏈��
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[EWING::NUM] = { 0.f,0.f,0.f,0.f };

	//�G���x�[�^�[�̓��͂����鎞
	if (m_AxisValuePerFrame.Y != 0.f)
	{
		//�O���͂�����Ă��邩�ǂ���
		const bool isForward = (m_AxisValuePerFrame.Y > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValuePerFrame.Y);
		wingAccel[(isForward ? EWING::LEFT_BACKWARD : EWING::LEFT_FORWARD)] += axisAbsValue;
		wingAccel[(isForward ? EWING::RIGHT_BACKWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
	}

	//�G�������̓��͂����鎞
	if (m_AxisValuePerFrame.X != 0.f)
	{
		//�E���͂�����Ă��邩�ǂ���
		const bool isRight = (m_AxisValuePerFrame.X > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValuePerFrame.X);
		wingAccel[(isRight ? EWING::LEFT_FORWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
		wingAccel[(isRight ? EWING::LEFT_BACKWARD : EWING::RIGHT_BACKWARD)] += axisAbsValue;
	}
	return wingAccel[_arrayIndex];
}

//�H�̉����x�X�V����
void APlayerDrone::UpdateWingAccle(const float& DeltaTime)
{
	//�e�X�e�B�b�N�̓��͂̒l�����擾
	FVector2D RightAxis = FVector2D(m_AxisValuePerFrame.W, m_AxisValuePerFrame.Z);
	FVector2D LeftAxis = FVector2D(m_AxisValuePerFrame.X, m_AxisValuePerFrame.Y);

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

//���͂̉����x�X�V����
void APlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
{
	for (int i = 0; i < EINPUT_AXIS::NUM; i++)
	{
		//���͂��z�o�����O��Ԃ��傫���Ƃ�
		if (m_AxisValuePerFrame[i] > 0.f)
		{
			if (m_AxisAccel[i] < m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValuePerFrame[i] * DeltaTime;
			}
		}
		//���͂��z�o�����O��Ԃ�菬������
		else if (m_AxisValuePerFrame[i] < 0.f)
		{
			if (m_AxisAccel[i] > -m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValuePerFrame[i] * DeltaTime;
			}
		}
		//���͂��d�͂ƒނ荇����(�z�o�����O���)
		else
		{
			if (m_AxisAccel[i] == m_AxisAccel.W)
			{
				m_AxisAccel[i] = 0.f;
			}
			
			//0�ɋ߂��Ȃ�������ʂ�0�ɂ���
			if (FMath::Abs(m_AxisAccel[i]) > 0.005f)
			{
				m_AxisAccel[i] *= m_Deceleration;
			}
			else
			{
				m_AxisAccel[i] = 0.f;
			}
		}

		m_AxisAccel[i] = FMath::Clamp(m_AxisAccel[i], -m_WingAccelMax, m_WingAccelMax);
	}
}

//�h���[���̉�]����
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	Super::UpdateRotation(DeltaTime);

	if (m_isControl)
	{
		m_SaveQuatX.Add(FString::SanitizeFloat(m_SaveQuat.X));
		m_SaveQuatY.Add(FString::SanitizeFloat(m_SaveQuat.Y));
		m_SaveQuatZ.Add(FString::SanitizeFloat(m_SaveQuat.Z));
		m_SaveQuatW.Add(FString::SanitizeFloat(m_SaveQuat.W));
	}
}

//���x�X�V����
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	Super::UpdateSpeed(DeltaTime);

	if (m_isControl)
	{
		m_SaveVelocityX.Add(FString::SanitizeFloat(m_Velocity.X));
		m_SaveVelocityY.Add(FString::SanitizeFloat(m_Velocity.Y));
		m_SaveVelocityZ.Add(FString::SanitizeFloat(m_Velocity.Z));

	}
	else
	{
		if (CGameUtility::SetDecimalTruncation(m_Velocity, 3).GetAbsMax() != 0.f)
		{
			m_Velocity *= m_Deceleration;
		}
	}

	AddActorWorldOffset(m_Velocity * MOVE_CORRECTION, true);
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

	//���C�̊J�n�_�ƏI�_��ݒ�(�h���[���̍��W����O���Ɍ�������)
	float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
	FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

	FVector Start = GetActorLocation();
	FVector End = Start + BodyQuat.GetForwardVector() * 1500.f;
	//�q�b�g���ʂ��i�[����z��
	TArray<FHitResult> OutHits;
	//�g���[�X����Ώ�(���g�͑Ώۂ���O��)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	//���C���΂��AWorldStatic�̃R���W�����`�����l�������I�u�W�F�N�g�̃q�b�g������擾����
	bool isHit = GetWorld()->LineTraceMultiByObjectType(OutHits, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool isClimbingSlope = false;
	//���C���q�b�g������A�N�^�[�̃^�O���m�F
	if (isHit)
	{
		//Slope�̃^�O�����A�N�^�[������΃J�������グ��t���O�𗧂Ă�
		for (const FHitResult& HitResult : OutHits)
		{
			if (HitResult.GetActor())
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Slope")))
				{
					isClimbingSlope = true;

					//�X�΂Ƃ̋����𑪒肷��
					m_DistanceToSlope = FVector::Dist(GetActorLocation(), HitResult.Location);
					break;
				}
			}
		}
	}

#ifdef DEBUG_UpdateCamera
	FColor LineColor = isClimbingSlope ? FColor::Yellow : FColor::Blue;
	//�f�o�b�O�p�̃��C����`��
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.f);

#endif // DEBUG_UpdateCamera
	FRotator CameraRotation = m_pCamera->GetRelativeRotation();
	if (FMath::Abs(m_AxisValuePerFrame.X) > 0.2f)
	{
		if (FMath::Abs(m_pCamera->GetRelativeRotation().Roll) < FMath::Abs(m_pBodyMesh->GetRelativeRotation().Roll))
		{
			CameraRotation.Roll += m_AxisValuePerFrame.X * 5.f * DeltaTime;
		}
	}
	else
	{
		CameraRotation.Roll *= 0.94f;
	}

	//���C���X�΂ɓ������Ă�����A���݂̍����ƌX�΂Ƃ̋���������z�����߂�
	if (isClimbingSlope)
	{
		float radSlope = FMath::Atan2(m_HeightFromGround, m_DistanceToSlope);
		float degSlope = FMath::ClampAngle(FMath::RadiansToDegrees(radSlope), SLOPE_MIN, m_TiltLimit);
		FRotator NewPRotation = FMath::RInterpTo(
			FRotator(CameraRotation.Pitch, 0.f, 0.f),
			FRotator(degSlope, 0.f, 0.f),
			DeltaTime,
			4.f);
		CameraRotation.Pitch = NewPRotation.Pitch;

		m_pSpringArm->SocketOffset.Z = FMath::Lerp(0.f, -m_TiltLimit, FMath::Clamp(NewPRotation.Pitch / m_TiltLimit, 0.f, 1.f));
	}
	else
	{
		CameraRotation.Pitch *= 0.96f;
		m_pSpringArm->SocketOffset.Z *= 0.96f;
	}

	//�J�����̉�]���X�V
	m_pCamera->SetRelativeRotation(CameraRotation * MOVE_CORRECTION);
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, m_pBodyMesh->GetRelativeRotation().Yaw, 0.f) * MOVE_CORRECTION);

	//�\�P�b�g�̈ʒu���X�V
	FVector NewSocketOffset = FVector(m_AxisAccel.Y, m_AxisAccel.X, 0.f) * m_CameraSocketOffsetMax / m_WingAccelMax;
	m_pSpringArm->SocketOffset.X = NewSocketOffset.X;
	m_pSpringArm->SocketOffset.Y = NewSocketOffset.Y;

	//�ړ��ʂɉ����Ď���p��ύX
	m_pCamera->SetFieldOfView(90.f - m_AxisAccel.Y * 10.f);
}

//�J�����Ƃ̎Օ����̃R���W��������
void  APlayerDrone::UpdateCameraCollsion()
{

}

//���̃G�t�F�N�g�̍X�V����
void APlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	Super::UpdateWindEffect(DeltaTime);
}

//���[�X�̍��W�ۑ�
void APlayerDrone::WritingRaceVector()
{
	//�e�L�X�g�t�@�C����������
	FFileHelper::SaveStringArrayToFile(m_SaveVelocityX, *(FPaths::ProjectDir() + FString("Record/VX.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveVelocityY, *(FPaths::ProjectDir() + FString("Record/VY.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveVelocityZ, *(FPaths::ProjectDir() + FString("Record/VZ.txt")));
}

//���[�X�̃N�I�[�^�j�I��
void APlayerDrone::WritingRaceQuaternion()
{
	//�e�L�X�g�t�@�C����������
	FFileHelper::SaveStringArrayToFile(m_SaveQuatX, *(FPaths::ProjectDir() + FString("Record/QX.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveQuatY, *(FPaths::ProjectDir() + FString("Record/QY.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveQuatZ, *(FPaths::ProjectDir() + FString("Record/QZ.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveQuatW, *(FPaths::ProjectDir() + FString("Record/QW.txt")));
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
		m_AxisValue.Z = CGameUtility::SetDecimalTruncation(m_AxisValue.Z, 3);
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
		m_AxisValue.Y = CGameUtility::SetDecimalTruncation(m_AxisValue.Y, 3);
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
		m_AxisValue.X = CGameUtility::SetDecimalTruncation(m_AxisValue.X, 3);
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
		m_AxisValue.W = CGameUtility::SetDecimalTruncation(m_AxisValue.W, 3);
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