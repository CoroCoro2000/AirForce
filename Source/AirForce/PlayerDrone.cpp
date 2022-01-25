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
//-----------------------------------------------------------------------

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
#include "Misc/FileHelper.h"
#include "DrawDebugHelpers.h"

//#define DEBUG_CAMERA

//�R���X�g���N�^
APlayerDrone::APlayerDrone()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(90.f)
	, m_FieldOfView(90.f)
	, m_CameraSocketOffset(FVector::ZeroVector)
	, m_CameraSocketOffsetMax(FVector(30.f, 45.f, 45.f))
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_CameraRotationAttenRate(FRotator(3.f, 3.f, 2.f))
	, m_MotionBlurAmount(1.5f)
	, m_MotionBlurMax(15.f)
	, m_MotionBlurTargetFPS(8)
	, m_AxisValue(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_StartLocation(FVector::ZeroVector)
	, m_StartQuaternion(FQuat::Identity)
	, m_CameraRotationYaw(0.f)
	, m_bIsOutCourse(false)
	, m_BodyOpacity(1.f)
{
	//���g��Tick()�𖈃t���[���Ăяo�����ǂ���
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = m_pDroneCollision;

	if (m_pBodyMesh)
	{
		m_pBodyMesh->AttachToComponent(m_pDroneCollision,FAttachmentTransformRules::KeepRelativeTransform);
	}

	//�X�v�����O�A�[������
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_pSpringArm && m_pBodyMesh)
	{
		m_pSpringArm->AttachToComponent(m_pDroneCollision, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//�J��������
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera && m_pSpringArm)
	{
		m_pCamera->AttachToComponent(m_pSpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//�J�����̏����ݒ�
	InitializeCamera();

	//�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//�v���C���[�^�O��ǉ�
	Tags.Add(TEXT("Player"));
}


//�Q�[���J�n����1�x��������
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();

	//�G�t�F�N�g�̏����ݒ�
	InitializeEmitter();

	//���̐������z���p�ӂ���
	m_SaveQuatText.Empty();
	m_SaveVelocityText.Empty();
	m_SaveQuatText.SetNum(4);
	m_SaveVelocityText.SetNum(3);

	//�����ʒu�ƃ��b�V���̉�]��ۑ�
	m_StartLocation = this->GetActorLocation();
	m_StartQuaternion = m_pBodyMesh->GetComponentQuat();
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

	//���x�`�F�b�N
	UpdateAltitudeCheck();

	//���v���C�X�V����
	UpdateReplay(DeltaTime);

	//�J�����̍X�V����
	UpdateCamera(DeltaTime);

	//�J�����Ƃ̎Օ����̃R���W��������
	UpdateCameraCollsion(DeltaTime);

	//���̃G�t�F�N�g�X�V����
	UpdateWindEffect(DeltaTime);

	//�����̃G�t�F�N�g�̍X�V����
	UpdateCloudOfDustEffect();
}

//�G�t�F�N�g�̏����ݒ�
void APlayerDrone::InitializeEmitter()
{
	if (!m_pCamera) { return; }

	//���̃G�t�F�N�g����
	m_pWindEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(m_pWindEffect, m_pCamera, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
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
	m_pSpringArm->TargetOffset.Z = 20.f;
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
				//�H�̉�]�ʂ̍���
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
			pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber());
		}
	}
	//���X�e�B�b�N�݂̂̏ꍇ
	else if (!LeftAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			pWing->AccelState = LeftInputValueToWingAcceleration(pWing->GetWingNumber());
		}
	}
}

//���͂̉����x�X�V����
void APlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
{
	//�����O���������Ă�����
	if (m_bIsPassedRing)
	{
		//����ɂȂ�܂Ŏ��Ԃ��v��
		if (m_SincePassageCount < m_CountLimitTime)
		{
			m_SincePassageCount += DeltaTime;
		}
		//������z������A�t���O���~�낷
		else
		{
			m_bIsPassedRing = false;
		}
	}

	//���͎��𐳋K������(�x�N�g���̑傫����������z���Ȃ��悤��)
	FVector NormalizeValue = m_AxisValuePerFrame.GetSafeNormal();

	//���͂�����Ƃ���������
	//XYZ��
	for (int i = 0; i < VECTOR3_COMPONENT_NUM; i++)
	{
		float Acceleration = m_Acceleration;
		float MaxAcceleration = m_WingAccelMax;
		float AttenRate = DeltaTime * (m_AxisValuePerFrame[i] != 0.f ? Acceleration : m_Deceleration);
		//�����O���������Ă������������
		if (m_bIsPassedRing)
		{
			Acceleration *= m_OverAccelerator;
			MaxAcceleration *= m_OverAccelerator;
			AttenRate = DeltaTime * Acceleration * m_OverAccelerator;
		}

		//���͗ʂɉ����ĉ����x�𑝌�������
		m_AxisAccel[i] = FMath::Lerp(m_AxisAccel[i], NormalizeValue[i] * MaxAcceleration, FMath::Clamp(AttenRate, 0.f, 1.f));
	}

	//����(W��)
	const float AttenRate = FMath::Clamp((m_AxisValuePerFrame.W != 0.f ? DeltaTime * m_Acceleration : 1.f), 0.f, 1.f);
	//���͂����鎞�͏��X�ɐ��񂵁A�Ȃ��Ƃ��͉񂳂Ȃ�
	m_AxisAccel.W = FMath::Lerp(m_AxisAccel.W, m_AxisValuePerFrame.W * m_WingAccelMax, AttenRate);
}

//�h���[���̉�]����
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	if (m_isReplay && !IsEndPlayBackReplay())
	{
		//�ǂݍ��񂾈ړ��ʂ̃e�L�X�g�t�@�C����float�ɕϊ�����
		bool IsValidTextArray = true;
		bool IsValidAxisTextArray = true;

		for (int index = 0; index < VECTOR4_COMPONENT_NUM; ++index)
		{
			IsValidTextArray = m_SaveQuatText.IsValidIndex(index);
			IsValidAxisTextArray = m_SaveQuatText[index].IsValidIndex(m_PlaybackFlame);
			if (!IsValidTextArray || IsValidAxisTextArray)
			{
				break;
			}
		}

		FQuat ReplayQuat = FQuat::Identity;

		if (IsValidTextArray && IsValidAxisTextArray)
		{
			ReplayQuat.X = FCString::Atof(*(m_SaveQuatText[0][m_PlaybackFlame]));
			ReplayQuat.Y = FCString::Atof(*(m_SaveQuatText[1][m_PlaybackFlame]));
			ReplayQuat.Z = FCString::Atof(*(m_SaveQuatText[2][m_PlaybackFlame]));
			ReplayQuat.W = FCString::Atof(*(m_SaveQuatText[3][m_PlaybackFlame]));
		}
		m_pBodyMesh->SetWorldRotation(ReplayQuat * MOVE_CORRECTION);
		return; 
	}

	Super::UpdateRotation(DeltaTime);

	//�R���g���[���\�Ȃ��]�ʂ�ۑ�����
	if (m_isControl)
	{
		FQuat BodyQuat = m_pBodyMesh->GetComponentQuat();
		bool IsValidTextArray = true;
		for (int index = 0; index < VECTOR4_COMPONENT_NUM; ++index)
		{
			IsValidTextArray = m_SaveQuatText.IsValidIndex(index);
			if (!IsValidTextArray)
			{
				break;
			}
		}

		if (IsValidTextArray)
		{
			m_SaveQuatText[0].Add(FString::SanitizeFloat(BodyQuat.X));
			m_SaveQuatText[1].Add(FString::SanitizeFloat(BodyQuat.Y));
			m_SaveQuatText[2].Add(FString::SanitizeFloat(BodyQuat.Z));
			m_SaveQuatText[3].Add(FString::SanitizeFloat(BodyQuat.W));
		}
	}
}

//���x�X�V����
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	Super::UpdateSpeed(DeltaTime);

	//�R���g���[���\�Ȃ�ړ��ʂ�ۑ�����
	if (m_isControl)
	{
		int index = 0;
		for (TArray<FString>& SaveVelocityText : m_SaveVelocityText)
		{
			SaveVelocityText.Add(FString::SanitizeFloat(m_Velocity[index]));
			++index;
		}
	}
	else if (m_isReplay && !IsEndPlayBackReplay())
	{
		//�ǂݍ��񂾈ړ��ʂ̃e�L�X�g�t�@�C����float�ɕϊ�����
		int index = 0;
		for (const TArray<FString> SaveVelocityText : m_SaveVelocityText)
		{
			if (SaveVelocityText.IsValidIndex(m_PlaybackFlame))
			{
				m_Velocity[index] = FCString::Atof(*(SaveVelocityText[m_PlaybackFlame]));
			}
			++index;
		}
	}
	else
	{
		if (CGameUtility::SetDecimalTruncation(m_Velocity, 3).GetAbsMax() != 0.f)
		{
			m_Velocity = FMath::Lerp(m_Velocity, FVector::ZeroVector, DeltaTime * m_Deceleration);
		}
	}
	//���W���X�V
	AddActorWorldOffset(m_Velocity * MOVE_CORRECTION, true);
}

//�J�����X�V����
void APlayerDrone::UpdateCamera(const float& DeltaTime)
{
	//NULL�`�F�b�N
	if (!m_pCamera || !m_pSpringArm || !m_pBodyMesh) { return; }

	//�ړ��ʂɉ����ăJ�����̃u����傫������
	bool isMove = !m_AxisValuePerFrame.IsNearlyZero3();

	//���C�̊J�n�_�ƏI�_��ݒ�(�h���[���̍��W����O���Ɍ�������)
	float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
	FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

	float RayLength = 2000.f;
	FVector Start = GetActorLocation();
	FVector End = Start + BodyQuat.GetForwardVector() * RayLength;
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
			if (AActor* pHitActor = HitResult.GetActor())
			{
				if (pHitActor->ActorHasTag(TEXT("Slope")))
				{
					isClimbingSlope = true;
					//�X�΂Ƃ̋����𑪒肷��
					m_DistanceToSlope = HitResult.Distance;
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

	//�J�����Ƌ@�̂̊p�x���擾
	FRotator CameraRotation = m_pCamera->GetRelativeRotation();
	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();

	//�����䗦��ݒ�(�t���[�����������ۂ�Lerp�̏�����z���Ȃ��悤�ɏ����1�ŃN�����v����)
	FRotator AttenRate = FRotator(
		FMath::Clamp(DeltaTime * m_CameraRotationAttenRate.Pitch, 0.f, 1.f),
		FMath::Clamp(DeltaTime * m_CameraRotationAttenRate.Yaw, 0.f, 1.f),
		FMath::Clamp(DeltaTime * m_CameraRotationAttenRate.Roll, 0.f, 1.f));

	//�Ζʂ�o���Ă���Ƃ��͌��グ��悤�Ȋp�x�ɂ���
	if (isClimbingSlope)
	{
		float radSlope = FMath::Atan2(m_HeightFromGround, m_DistanceToSlope);
		float degSlope = FMath::ClampAngle(FMath::RadiansToDegrees(radSlope), SLOPE_MIN, m_TiltLimit);
		CameraRotation.Pitch = FMath::Lerp(CameraRotation.Pitch, degSlope, AttenRate.Pitch);
	}
	else
	{
		CameraRotation.Pitch = FMath::Lerp(CameraRotation.Pitch, FMath::Clamp(BodyRotation.Pitch, -5.f, 5.f), AttenRate.Pitch);
	}
	m_CameraRotationYaw = FMath::Lerp(0.f, m_AxisValuePerFrame.W * 5.f, AttenRate.Yaw);
	CameraRotation.Roll = FMath::Lerp(CameraRotation.Roll, BodyRotation.Roll * 0.7f, AttenRate.Roll);

	//�\�P�b�g�̈ʒu���X�V
	FVector SocketAttenRate = FVector(
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 0.8f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f));

	float HorizontalAxis = (FMath::Abs(m_AxisValuePerFrame.X) > FMath::Abs(m_AxisValuePerFrame.W) ? m_AxisValuePerFrame.X : m_AxisValuePerFrame.W);

	m_pSpringArm->SocketOffset.X = FMath::Lerp(m_pSpringArm->SocketOffset.X, m_AxisValuePerFrame.Y * m_CameraSocketOffsetMax.X, SocketAttenRate.X);
	m_pSpringArm->SocketOffset.Y = FMath::Lerp(m_pSpringArm->SocketOffset.Y, HorizontalAxis * m_CameraSocketOffsetMax.Y, SocketAttenRate.Y);
	m_pSpringArm->SocketOffset.Z = FMath::Lerp(m_pSpringArm->SocketOffset.Z, -CameraRotation.Pitch * 1.5f, SocketAttenRate.Z);
	m_pSpringArm->TargetArmLength = FMath::Lerp(m_pSpringArm->TargetArmLength, (isClimbingSlope ? 150.f : 90.f), SocketAttenRate.Z);

	//�J�����̉�]���X�V
	m_pCamera->SetRelativeRotation(CameraRotation.Quaternion() * MOVE_CORRECTION);
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, BodyRotation.Yaw + m_CameraRotationYaw, 0.f) * MOVE_CORRECTION);

	if (!m_isReplay)
	{
		//�ړ��ɉ����Ď���p��ύX
		float FOV = isMove ? (m_bIsPassedRing ? 125.f : 105.f) : 90.f;
		float FOVAttenRate = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
		float NewFOV = FMath::Lerp(m_pCamera->FieldOfView, FOV, FOVAttenRate);
		m_pCamera->SetFieldOfView(NewFOV);

		//�ړ��ɉ����ă��[�V�����u���[��������
		float MotionBlurAmount = isMove ? (m_bIsPassedRing ? m_MotionBlurAmount : m_MotionBlurAmount * 0.8f) : 0.5f;
		float MotionBlurMax = isMove ? (m_bIsPassedRing ? m_MotionBlurMax : m_MotionBlurMax * 0.8f) : 5.f;
		int32 MotionBlurTargetFPS = isMove ? (m_bIsPassedRing ? m_MotionBlurTargetFPS : m_MotionBlurTargetFPS * 0.8f) : 30;
		m_pCamera->PostProcessSettings.MotionBlurAmount = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurAmount, MotionBlurAmount, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurMax = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurMax, MotionBlurMax, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurTargetFPS = MotionBlurTargetFPS;

	}
	else 
	{
		bool isReplayMove = !m_Velocity.IsNearlyZero();
		//�ړ��ɉ����Ď���p��ύX
		float FOV = isReplayMove ? (m_bIsPassedRing ? 125.f : 105.f) : 90.f;
		float FOVAttenRate = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
		float NewFOV = FMath::Lerp(m_pCamera->FieldOfView, FOV, FOVAttenRate);
		m_pCamera->SetFieldOfView(NewFOV);

		//�ړ��ɉ����ă��[�V�����u���[��������
		float MotionBlurAmount = isReplayMove ? (m_bIsPassedRing ? m_MotionBlurAmount : m_MotionBlurAmount * 0.8f) : 0.5f;
		float MotionBlurMax = isReplayMove ? (m_bIsPassedRing ? m_MotionBlurMax : m_MotionBlurMax * 0.8f) : 5.f;
		int32 MotionBlurTargetFPS = isReplayMove ? (m_bIsPassedRing ? m_MotionBlurTargetFPS : m_MotionBlurTargetFPS * 0.8f) : 30;
		m_pCamera->PostProcessSettings.MotionBlurAmount = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurAmount, MotionBlurAmount, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurMax = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurMax, MotionBlurMax, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurTargetFPS = MotionBlurTargetFPS;
	}
}

//�J�����Ƃ̎Օ����̃R���W��������
void  APlayerDrone::UpdateCameraCollsion(const float& DeltaTime)
{
	if (!m_pCamera) { return; }
	if (!m_pSpringArm) { return; }
	if (!m_pBodyMesh) { return; }

	//���C�̎n�_�ƏI�_��ݒ�(�h���[������J�����܂ł̋���)
	FVector DroneLocation = GetActorLocation();
	FVector CameraLocation = m_pCamera->GetComponentLocation();
	//�J�����̍��E�̍��W
	float Len = m_pSpringArm->TargetArmLength;
	FVector CameraRightVec = m_pCamera->GetRightVector();
	FVector CameraUp = CameraLocation + (m_pCamera->GetUpVector() * Len);
	FVector CameraLeft = CameraLocation - (CameraRightVec * Len);
	FVector CameraRight = CameraLocation + (CameraRightVec * Len);
	
	//�q�b�g���ʂ��i�[����z��
	FHitResult OutSpringArmHit;
	TArray<FHitResult> OutCameraWidthHits;
	TArray<FHitResult> OutCameraVerticalHits;

	//�g���[�X����Ώ�(���g�͑Ώۂ���O��)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);
	//���C���΂��AWorldStatic�̃R���W�����`�����l�������I�u�W�F�N�g�̃q�b�g������擾����
	bool isSpringArmHit = GetWorld()->LineTraceSingleByObjectType(OutSpringArmHit, DroneLocation, CameraLocation, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool isCameraWidthHit = GetWorld()->LineTraceMultiByObjectType(OutCameraWidthHits, CameraLeft, CameraRight, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool isCameraVerticalHit = GetWorld()->LineTraceMultiByObjectType(OutCameraVerticalHits, CameraLocation, CameraUp, ECollisionChannel::ECC_WorldStatic, CollisionParam);

	float Atten = FMath::Clamp(DeltaTime * 7.f, 0.f, 1.f);

	FVector TargetOffset = FVector(0.f, 0.f, 20.f);

	//�J�����̉�����
	if (isCameraWidthHit)
	{		
		//�J�����ɏd�Ȃ����I�u�W�F�N�g���m�F
		if (OutCameraWidthHits.Num() > 0)
		{
			for (const FHitResult& hitResult : OutCameraWidthHits)
			{
				if (AActor* pHitActor = hitResult.GetActor())
				{
					//�J�������u���b�N����I�u�W�F�N�g������ꍇ�̓I�t�Z�b�g�ʒu�𒲐�����
					if (pHitActor->ActorHasTag(TEXT("CameraBlocking")))
					{
					
						break;
					}
				}
			}
		}
	}

	//�J�����̏c����
	if (isCameraVerticalHit)
	{
		//�J�����ɏd�Ȃ����I�u�W�F�N�g���m�F
		if (OutCameraVerticalHits.Num() > 0)
		{
			for (const FHitResult& hitResult : OutCameraVerticalHits)
			{
				if (AActor* pHitActor = hitResult.GetActor())
				{
					//�J�������u���b�N����I�u�W�F�N�g������ꍇ�̓I�t�Z�b�g�ʒu�𒲐�����
					if (pHitActor->ActorHasTag(TEXT("CameraBlocking")))
					{
						TargetOffset.Z = FMath::Lerp(-30.f, 20.f, FMath::Clamp(hitResult.Distance / Len, 0.f, 1.f));
						break;
					}
				}
			}
		}
	}

	//�J�����̃I�t�Z�b�g��ύX
	FVector NewCameraOffset = FMath::Lerp(m_pSpringArm->TargetOffset, TargetOffset, Atten * 0.5f);
	m_pSpringArm->TargetOffset = (NewCameraOffset - TargetOffset).IsNearlyZero(0.3f) ? TargetOffset : NewCameraOffset;
}

//���̃G�t�F�N�g�̍X�V����
void APlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	Super::UpdateWindEffect(DeltaTime);
}

//���v���C�̏����ݒ�
void APlayerDrone::InitializeReplay()
{
	SetActorLocation(m_StartLocation);
	m_pBodyMesh->SetWorldRotation(m_StartQuaternion);
	m_PlaybackFlame = 0;
	m_isReplay = true;
}
//���v���C�X�V����
void APlayerDrone::UpdateReplay(const float& DeltaTime)
{
	if (!m_isReplay || IsEndPlayBackReplay()) { return; }

	m_PlaybackFlame++;
}
//���[�X�̍��W�ۑ�
void APlayerDrone::WritingBestRaceVector()
{
	if (m_SaveVelocityText.Num() != m_SaveVelocityLoadPath.Num()) { return; }

	//�e�L�X�g�t�@�C����������
	int index = 0;
	for (const TArray<FString>& SaveVelocityText : m_SaveVelocityText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::BEST] + m_SaveVelocityLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveVelocityText, *FliePath);
		++index;
	}
}

//���[�X�̃N�I�[�^�j�I���ۑ�
void APlayerDrone::WritingBestRaceQuaternion()
{
	if (m_SaveQuatText.Num() != m_SaveQuatLoadPath.Num()) { return; }

	//�e�L�X�g�t�@�C����������
	int index = 0;
	for (const TArray<FString> SaveQuatText : m_SaveQuatText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::BEST] + m_SaveQuatLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveQuatText, *FliePath);
		++index;
	}
}

//���v���C�̃��[�X�̍��W�t�@�C����������
void APlayerDrone::WritingReplayRaceVector()
{
	if (m_SaveVelocityText.Num() != m_SaveVelocityLoadPath.Num()) { return; }

	//�e�L�X�g�t�@�C����������
	int index = 0;
	for (const TArray<FString>& SaveVelocityText : m_SaveVelocityText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::REPLAY] + m_SaveVelocityLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveVelocityText, *FliePath);
		++index;
	}
}

//���v���C�̃��[�X�̃N�I�[�^�j�I���t�@�C����������
void APlayerDrone::WritingReplayRaceQuaternion()
{
	if (m_SaveQuatText.Num() != m_SaveQuatLoadPath.Num()) { return; }

	//�e�L�X�g�t�@�C����������
	int index = 0;
	for (const TArray<FString> SaveQuatText : m_SaveQuatText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::REPLAY] + m_SaveQuatLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveQuatText, *FliePath);
		++index;
	}
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
}