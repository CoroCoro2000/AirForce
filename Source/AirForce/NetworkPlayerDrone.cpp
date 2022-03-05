//------------------------------------------------------------------------
// �t�@�C����		:NetworkPlayerDrone.cpp
// �T�v				:�l�b�g���[�N�p�̃v���C���[�h���[���N���X
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#include "NetworkPlayerDrone.h"
#include "Net/UnrealNetwork.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameUtility.h"

// Sets default values
ANetworkPlayerDrone::ANetworkPlayerDrone()
	: m_BodyMesh(nullptr)
	, m_pBodyMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body")))
	, m_WingMesh()
	, m_pWings()
	, m_pSpringArm(CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm")))
	, m_pCamera(CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")))
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
	, m_PlayerId(-1)
	, m_RPSMax(10.f)
	, m_WingAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_TiltLimit(45.f)
	, m_Speed(7.f)
	, m_SpeedPerSecondMax(50.f)
	, m_AxisAccel(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_Acceleration(0.8f)
	, m_Deceleration(1.2f)
	, m_Turning(0.6f)
	, m_Attenuation(0.5f)
	, m_DroneWeight(0.3f)
	, m_Velocity(FVector::ZeroVector)
	, m_pWingRotationSE(NULL)
	, m_HeightMax(400.f)
	, m_HeightFromGround(0.f)
	, m_DistanceToSlope(0.f)
	, m_isControl(false)
	, m_pWindEffect(NULL)
	, m_pWindEmitter(NULL)
	, m_WindRotationSpeed(5.f)
	, m_WindOpacity(0.f)
	, m_WindNoise(15.f)
	, m_bIsPassedRing(false)
	, m_SincePassageCount(0.f)
	, m_CountLimitTime(1.f)
	, m_OverAccelerator(1.5f)
	, m_pLeftSpotLight(NULL)
	, m_pRightSpotLight(NULL)
	, m_pDroneEffects()
	, m_pCloudOfDustEmitter(NULL)
	, m_ShowEffectDistance(50.f)
	, m_GroundMaterialName(TEXT(""))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���b�V���̏�����
	InitializeMesh();
	//�J�����̏�����
	InitializeCamera();

	//�f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//�v���C���[�^�O��ǉ�
	Tags.Add(TEXT("Player"));
}

// Called when the game starts or when spawned
void ANetworkPlayerDrone::BeginPlay()
{
	Super::BeginPlay();
	
	//�G�t�F�N�g�̏����ݒ�
	InitializeEmitter();

	if (UCapsuleComponent* pCapsuleComponent = GetCapsuleComponent())
	{
		//�I�[�o�[���b�v�A�q�b�g���̃C�x���g�֐����o�C���h
		pCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ANetworkPlayerDrone::OnDroneCollisionOverlapBegin);
		pCapsuleComponent->OnComponentHit.AddDynamic(this, &ANetworkPlayerDrone::OnDroneCollisionHit);
	}
}

//���v���P�[�g��o�^
void ANetworkPlayerDrone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called every frame
void ANetworkPlayerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	//�J�����̍X�V����
	UpdateCamera(DeltaTime);

	//�J�����Ƃ̎Օ����̃R���W��������
	UpdateCameraCollsion(DeltaTime);

	//���̃G�t�F�N�g�X�V����
	UpdateWindEffect(DeltaTime);

	//�����̃G�t�F�N�g�̍X�V����
	UpdateCloudOfDustEffect();
}

//���b�V���A�Z�b�g�̃Z�b�g�A�b�v
void ANetworkPlayerDrone::MeshAssetSetup()
{
	m_BodyMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/NewDrone/Drone.Drone"))).LoadSynchronous();

	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST"))).LoadSynchronous());
	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST"))).LoadSynchronous());
}

//���b�V���̏����ݒ�
void ANetworkPlayerDrone::InitializeMesh()
{
	MeshAssetSetup();

	if (m_pBodyMesh && m_BodyMesh)
	{
		//���b�V���̃A�^�b�`
		m_pBodyMesh->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		m_pBodyMesh->SetStaticMesh(m_BodyMesh);

		//�H�̃��b�V���R���|�[�l���g����
		for (int index = 0; index < EWING::NUM; ++index)
		{
			//�E���̉H�𒲂ׂ�
			const bool isRightTrun = (index == 0 || index == 3);
			//�H�̖��O��ݒ�
			const FName WingName = isRightTrun ?
				(index + 1 < 3) ? TEXT("LF_Wing") : TEXT("RB_Wing") :
				(index + 1 > 2) ? TEXT("LB_Wing") : TEXT("RF_Wing");
			FRotator InitRotaion = FRotator::ZeroRotator;
			InitRotaion.Yaw = (index < 2) ?
				(index == 0) ? -45.f : 45.f :
				(index == 2) ? 45.f : -45.f;

			//�z��̒ǉ�(���ʔԍ��A�H�̃��b�V��)
			m_pWings.Add(MakeShareable(new FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName))));

			if (m_pWings.IsValidIndex(index))
			{
				if (UStaticMeshComponent* pWingMesh = m_pWings[index]->GetWingMesh())
				{
					if (m_WingMesh.IsValidIndex(0) && m_WingMesh.IsValidIndex(1))
					{
						//�H�̃��b�V����ݒ�
						pWingMesh->SetStaticMesh(m_WingMesh[isRightTrun]);
						//�H�̃��b�V���R���W�����𖳌��ɂ���
						pWingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						//�\�P�b�g�̈ʒu�ɉH���A�^�b�`
						pWingMesh->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, WingName);
						pWingMesh->SetRelativeRotation(InitRotaion);
					}
				}
			}
		}
	}
}

//���C�g�̏����ݒ�
void ANetworkPlayerDrone::InitializeLight()
{
	//���C�g�R���|�[�l���g����
	m_pLeftSpotLight = NewObject<USpotLightComponent>(this);
	m_pRightSpotLight = NewObject<USpotLightComponent>(this);

	if (m_pLeftSpotLight && m_pRightSpotLight && m_pBodyMesh)
	{
		m_pLeftSpotLight->RegisterComponent();
		m_pRightSpotLight->RegisterComponent();
		m_pLeftSpotLight->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("LeftSpotLight"));
		m_pRightSpotLight->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightSpotLight"));
		m_pLeftSpotLight->SetOuterConeAngle(20.f);
		m_pRightSpotLight->SetOuterConeAngle(20.f);
		m_pLeftSpotLight->Intensity = 2500.f;
		m_pRightSpotLight->Intensity = 2500.f;
		m_pLeftSpotLight->LightColor = FColor(51, 153, 204);
		m_pRightSpotLight->LightColor = FColor(51, 153, 204);
		m_pLeftSpotLight->SetRelativeRotation(FRotator(0.f, -20.f, 0.f));
		m_pRightSpotLight->SetRelativeRotation(FRotator(0.f, 20.f, 0.f));
	}
}

//�G�t�F�N�g�̏����ݒ�
void ANetworkPlayerDrone::InitializeEmitter()
{
	if (!m_pCamera) { return; }

	//���̃G�t�F�N�g����
	m_pWindEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(m_pWindEffect, m_pCamera, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
}

//�J�����̏����ݒ�
void ANetworkPlayerDrone::InitializeCamera()
{
	if (!m_pCamera) { return; }
	if (!m_pSpringArm) { return; }

	//�A�^�b�`����
	m_pSpringArm->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	m_pCamera->AttachToComponent(m_pSpringArm, FAttachmentTransformRules::KeepRelativeTransform);
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

//�H�̉����x�X�V����
void ANetworkPlayerDrone::UpdateWingAccle(const float& DeltaTime)
{
	//�e�X�e�B�b�N�̓��͂̒l�����擾
	FVector2D RightAxis = FVector2D(m_AxisValue.W, m_AxisValue.Z);
	FVector2D LeftAxis = FVector2D(m_AxisValue.X, m_AxisValue.Y);

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

//�H�̉�]�X�V����
void ANetworkPlayerDrone::UpdateWingRotation(const float& DeltaTime)
{
	//2���̓��͗ʂ���������
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValue.W, m_AxisValue.Z).Size() +
		FVector2D(m_AxisValue.X, m_AxisValue.Y).Size()) / 2,
		0.f, 1.f);

	//���bm_rpsMax * WingAccel�񕪉�]���邽�߂ɖ��t���[���H���񂷊p�x�����߂�
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (pWing.IsValid())
		{
			if (pWing->GetWingMesh())
			{
				//�H�̉����x��0����1�͈̔͂ɏC�����A���K������
				const float NormalizeAccelSize = FMath::Clamp((pWing->AccelState + 1.f) / 3.f, 0.f, 1.f);
				//���K�����������x���g���ĉH�̉����̊������Ԃ���
				const float WingAccel = FMath::Lerp(m_WingAccelMin, m_WingAccelMax, NormalizeAccelSize);
				//�E���̉H�����ʂ���(���O�ƉE���̉H���E���ɉ�]����)
				const bool isTurnRight = (pWing->GetWingNumber() == EWING::LEFT_FORWARD || pWing->GetWingNumber() == EWING::RIGHT_BACKWARD ? true : false);
				//1�t���[���ɉ�]����p�x�����߂�
				const float angularVelocity = m_RPSMax * 360.f * DeltaTime * WingAccel * (isTurnRight ? 1.f : -1.f) * MOVE_CORRECTION;

				//�H����]������
				pWing->GetWingMesh()->AddLocalRotation(FRotator(0.f, angularVelocity, 0.f));

#ifdef DEBUG_WING
				//*�f�o�b�O�p*���x�ɉ����ĉH�̐F�ύX				
				const FVector WingColor = FVector(FLinearColor::LerpUsingHSV(FColor::Blue, FColor::Yellow, NormalizeAccelSize));
				wing.GetWingMesh()->SetVectorParameterValueOnMaterials(TEXT("WingColor"), WingColor);
#endif // DEBUG_WING
			}
		}
	}
}

//���͂̉����x�X�V����
void ANetworkPlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
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
	FVector NormalizeValue = m_AxisValue.GetSafeNormal();

	//���͂�����Ƃ���������
	//XYZ��
	for (int i = 0; i < VECTOR3_COMPONENT_NUM; i++)
	{
		float Acceleration = m_Acceleration;
		float MaxAcceleration = m_WingAccelMax;
		float AttenRate = DeltaTime * (m_AxisValue[i] != 0.f ? Acceleration : m_Deceleration);
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
	const float AttenRate = FMath::Clamp((m_AxisValue.W != 0.f ? DeltaTime * m_Acceleration : 1.f), 0.f, 1.f);
	//���͂����鎞�͏��X�ɐ��񂵁A�Ȃ��Ƃ��͉񂳂Ȃ�
	m_AxisAccel.W = FMath::Lerp(m_AxisAccel.W, m_AxisValue.W * m_WingAccelMax, AttenRate);
}

//��]����
void ANetworkPlayerDrone::UpdateRotation(const float& DeltaTime)
{
	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }

	//���b�V���̌X���ƃA�N�^�[��Yaw�̉�]�ʂ��擾
	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();
	FRotator RootRotation = GetActorRotation();

	//�ő�p�x
	const float MaxDeg = 25.f;
	float HorizontalAxis = (FMath::Abs(m_AxisValue.X) > FMath::Abs(m_AxisValue.W) ? m_AxisValue.X : m_AxisValue.W);

	float RotationSpeed = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
	BodyRotation.Pitch = FMath::Lerp(BodyRotation.Pitch, m_AxisValue.Y * MaxDeg, RotationSpeed);
	RootRotation.Yaw = FMath::Lerp(RootRotation.Yaw, RootRotation.Yaw + m_AxisValue.W * MaxDeg, RotationSpeed);
	BodyRotation.Roll = FMath::Lerp(BodyRotation.Roll, HorizontalAxis * MaxDeg, RotationSpeed);


	FRotator YawRotation = FRotator(0.f, 25.f, 0.f);
	BodyRotation.Yaw = RootRotation.Yaw;
	m_pBodyMesh->SetRelativeRotation(BodyRotation.Quaternion(), true);
	SetActorRotation(RootRotation);
}

//���x�X�V����
void ANetworkPlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	if (m_isControl)
	{
		float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
		FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

		FVector LocalAxis = FVector::ZeroVector;
		FVector NormalizeValue = FVector(m_AxisAccel).GetAbs().GetSafeNormal();
		LocalAxis += BodyQuat.GetRightVector() * m_AxisAccel.X;
		LocalAxis += BodyQuat.GetForwardVector() * -m_AxisAccel.Y;
		LocalAxis += BodyQuat.GetUpVector() * m_AxisAccel.Z;

		//�h���[���ɂ�����͂̐ݒ�
		m_Velocity = CGameUtility::SetDecimalTruncation(LocalAxis * m_Speed, 3);

		//���x����𒴂��Ă����玩���I�ɍ��x��������
		if (m_HeightFromGround >= m_HeightMax)
		{
			m_Velocity.Z = -3.f;
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
void ANetworkPlayerDrone::UpdateCamera(const float& DeltaTime)
{
	//NULL�`�F�b�N
	if (!m_pCamera || !m_pSpringArm || !m_pBodyMesh) { return; }

	//�ړ��ʂɉ����ăJ�����̃u����傫������
	bool isMove = !m_AxisValue.IsNearlyZero3();

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
	m_CameraRotationYaw = FMath::Lerp(0.f, m_AxisValue.W * 5.f, AttenRate.Yaw);
	CameraRotation.Roll = FMath::Lerp(CameraRotation.Roll, BodyRotation.Roll * 0.7f, AttenRate.Roll);

	//�\�P�b�g�̈ʒu���X�V
	FVector SocketAttenRate = FVector(
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 0.8f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f));

	float HorizontalAxis = (FMath::Abs(m_AxisValue.X) > FMath::Abs(m_AxisValue.W) ? m_AxisValue.X : m_AxisValue.W);

	m_pSpringArm->SocketOffset.X = FMath::Lerp(m_pSpringArm->SocketOffset.X, m_AxisValue.Y * m_CameraSocketOffsetMax.X, SocketAttenRate.X);
	m_pSpringArm->SocketOffset.Y = FMath::Lerp(m_pSpringArm->SocketOffset.Y, HorizontalAxis * m_CameraSocketOffsetMax.Y, SocketAttenRate.Y);
	m_pSpringArm->SocketOffset.Z = FMath::Lerp(m_pSpringArm->SocketOffset.Z, -CameraRotation.Pitch * 1.5f, SocketAttenRate.Z);
	m_pSpringArm->TargetArmLength = FMath::Lerp(m_pSpringArm->TargetArmLength, (isClimbingSlope ? 150.f : 90.f), SocketAttenRate.Z);

	//�J�����̉�]���X�V
	m_pCamera->SetRelativeRotation(CameraRotation.Quaternion() * MOVE_CORRECTION);
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, BodyRotation.Yaw + m_CameraRotationYaw, 0.f) * MOVE_CORRECTION);

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

//�E�̃X�e�B�b�N���͂���H�̉����x�ɕϊ����鏈��
float ANetworkPlayerDrone::RightInputValueToWingAcceleration(const int _arrayIndex)
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
float ANetworkPlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
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

//�J�����Ƃ̎Օ����̃R���W��������
void  ANetworkPlayerDrone::UpdateCameraCollsion(const float& DeltaTime)
{
	if (!m_pCamera) { return; }
	if (!m_pSpringArm) { return; }
	if (!m_pBodyMesh) { return; }

	//���C�̎n�_�ƏI�_��ݒ�(�h���[������J�����܂ł̋���)
	FVector DroneLocation = GetActorLocation();
	FVector CameraLocation = m_pCamera->GetComponentLocation();
	//�J�����̍��E�̍��W
	float Len = m_pSpringArm->TargetArmLength;
	FVector CameraUp = DroneLocation + (GetActorUpVector() * Len);

	//�q�b�g���ʂ��i�[����z��
	TArray<FHitResult> OutCameraVerticalHits;

	//�g���[�X����Ώ�(���g�͑Ώۂ���O��)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);
	//���C���΂��AWorldStatic�̃R���W�����`�����l�������I�u�W�F�N�g�̃q�b�g������擾����
	bool isCameraVerticalHit = GetWorld()->LineTraceMultiByObjectType(OutCameraVerticalHits, DroneLocation, CameraUp, ECollisionChannel::ECC_WorldStatic, CollisionParam);

	float Atten = FMath::Clamp(DeltaTime * 7.f, 0.f, 1.f);
	const float OFFSET_MAX = 30.f;

	float ZOffset = 20.f;

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
						ZOffset = FMath::Lerp(-OFFSET_MAX, 20.f, FMath::Clamp(hitResult.Distance / Len, 0.f, 1.f));
						break;
					}
				}
			}
		}
	}

	//�J�����̃I�t�Z�b�g��ύX
	FVector TargetOffset = FVector(0.f, 0.f, ZOffset);
	FVector NewCameraOffset = FMath::Lerp(m_pSpringArm->TargetOffset, TargetOffset, Atten * 0.5f);
	m_pSpringArm->TargetOffset = (NewCameraOffset - TargetOffset).IsNearlyZero(0.3f) ? TargetOffset : NewCameraOffset;
}

//���̃G�t�F�N�g�X�V����
void ANetworkPlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	if (!m_pWindEmitter || !m_pBodyMesh) { return; }

	//�G�t�F�N�g�ƃh���[���̍��W���擾
	FVector EffectLocation = m_pWindEmitter->GetComponentLocation();
	FVector  DroneLocation = m_pBodyMesh->GetComponentLocation();
	//�G�t�F�N�g���i�s�����֌����悤�ɂ���
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - EffectLocation).Rotator();
	//�ړ��ʂ̑傫������G�t�F�N�g�̕s�����x��ݒ�
	float AxisValue = FVector2D(m_AxisValue.X, m_AxisValue.Y).GetSafeNormal().Size();
	//���������v�Z
	float AccelRate = FMath::Clamp(m_AxisAccel.Size3() / m_WingAccelMax, 0.f, 1.f);
	float Opacity = (m_AxisValue.Y < 0.f) ? AxisValue * (m_bIsPassedRing ? 1.f : 0.8f) : 0.f;
	m_WindOpacity = FMath::Lerp(m_WindOpacity, Opacity, DeltaTime * 5.f);
	float WindNoise = (AxisValue != 0.f ? (m_bIsPassedRing ? 8.f : 18.f) : 40.f);
	m_WindNoise = FMath::Lerp(m_WindNoise, WindNoise, DeltaTime * 5.f);
	float effectScale = FMath::Lerp(5.f, 3.f, AxisValue);
	float effectLocationX = FMath::Lerp(-40.f, 0.f, AxisValue);

	m_pWindEmitter->SetRelativeScale3D(FVector(effectScale));
	m_pWindEmitter->SetWorldRotation(LookAtRotation.Quaternion());
	m_pWindEmitter->SetRelativeLocation(FVector(effectLocationX, 0.f, 0.f));
	//�G�t�F�N�g�̕s�����x��ύX
	m_pWindEmitter->SetVariableFloat(TEXT("User.Mask"), m_WindNoise);
	m_pWindEmitter->SetVariableFloat(TEXT("User.WindOpacity"), m_WindOpacity);
}

//���x�̏�����𒴂��Ă��邩�m�F
void ANetworkPlayerDrone::UpdateAltitudeCheck()
{
	//���C�̊J�n�_�ƏI�_��ݒ�(�h���[���̍��W���獂�x�̏���̒���)
	FVector Start = GetActorLocation();
	FVector End = Start;
	End.Z -= m_HeightMax;
	//�q�b�g���ʂ��i�[����z��
	TArray<FHitResult> OutHits;
	//�g���[�X����Ώ�(���g�͑Ώۂ���O��)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	//���C���΂��AWorldStatic�̃R���W�����`�����l�������I�u�W�F�N�g�̃q�b�g������擾����
	bool isHit = GetWorld()->LineTraceMultiByObjectType(OutHits, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool OverHeightMax = true;

	m_HeightFromGround = m_HeightMax;

	//���C���q�b�g������A�N�^�[�̃^�O���m�F���AGround�̃^�O�����A�N�^�[������΍��x������z���Ă��Ȃ��̂Ńt���O���~�낷
	if (isHit)
	{
		for (const FHitResult& HitResult : OutHits)
		{
			if (AActor* pHitActor = HitResult.GetActor())
			{
				if (pHitActor->ActorHasTag(TEXT("Ground")))
				{
					OverHeightMax = false;
					//�n�ʂ���̍������v��
					m_HeightFromGround = HitResult.Distance;

					if (pHitActor->ActorHasTag(TEXT("LandScape")))
					{
						m_GroundMaterialName = TEXT("LandScape");
					}
					else
					{
						//�q�b�g�����A�N�^�[�̃}�e���A�������擾
						if (UPrimitiveComponent* pHitComp = HitResult.GetComponent())
						{
							if (UMaterialInterface* pMaterial = pHitComp->GetMaterial(0))
							{
								m_GroundMaterialName = pMaterial->GetName();
							}
						}
					}
					break;
				}
			}
		}
	}
}

//�����̃G�t�F�N�g�̕\���ؑ�
void ANetworkPlayerDrone::UpdateCloudOfDustEffect()
{
	if (!m_isControl) { return; }
	if (!m_pBodyMesh) { return; }

	//�n�ʂƂ̋������߂��Ȃ�����G�t�F�N�g��\��
	if (m_HeightFromGround <= m_ShowEffectDistance)
	{
		//�G�t�F�N�g��n�ʂ̍����ɒ�������
		FVector EmitterLocation = m_pBodyMesh->GetUpVector() * (-m_HeightFromGround);

		if (m_pCloudOfDustEmitter)
		{
			//�n�ʂ̃}�e���A�����Ɉ�v����G�t�F�N�g��ݒ�
			if (UNiagaraSystem* pNiagaraSystem = m_pDroneEffects.FindRef(m_GroundMaterialName))
			{
				//�ݒ肳��Ă���G�t�F�N�g���Ⴄ�ꍇ�͍����ւ���
				if (m_pCloudOfDustEmitter->GetAsset() != pNiagaraSystem)
				{
					m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pBodyMesh, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
				}
				//�����G�t�F�N�g�Ȃ�ʒu�����̂ݍs��
				else
				{
					m_pCloudOfDustEmitter->SetRelativeLocation(EmitterLocation);
					m_pCloudOfDustEmitter->Activate();
				}
			}
			//�G�t�F�N�g�𔭐������Ȃ��}�e���A���̏ꍇ�̓G�t�F�N�g���\���ɂ���
			else
			{
				if (m_pCloudOfDustEmitter->IsActive())
				{
					m_pCloudOfDustEmitter->Deactivate();
				}
			}
		}
		else
		{
			//�n�ʂ̃}�e���A�����ƈ�v����G�t�F�N�g��ݒ�
			if (UNiagaraSystem* pNiagaraSystem = m_pDroneEffects.FindRef(m_GroundMaterialName))
			{
				m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pBodyMesh, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
			}
		}
	}
	//�n�ʂƂ̋��������ꂽ�獻�����\���ɂ���
	else
	{
		if (m_pCloudOfDustEmitter)
		{
			if (m_pCloudOfDustEmitter->IsActive())
			{
				m_pCloudOfDustEmitter->Deactivate();
			}
		}
	}
}

//�h���[���̓����蔻��ɃI�u�W�F�N�g���I�[�o�[���b�v�������Ă΂��C�x���g�֐���o�^
void ANetworkPlayerDrone::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�^�O��Ring�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Ring")))
		{
			//�����t���O�𗧂Ă�
			m_bIsPassedRing = true;

			//�����v���J�E���^�[�����Z�b�g
			m_SincePassageCount = 0.f;
		}
	}
}

//�h���[���̓����蔻��ɃI�u�W�F�N�g���q�b�g�������Ă΂��C�x���g�֐���o�^
void ANetworkPlayerDrone::OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		//�h���[���̌������擾
		FQuat Quat = FRotator(0.f, GetActorRotation().Yaw + 90.f, 0.f).Quaternion();

		//�Փ˂����A�N�^�[���d�Ԃ̏ꍇ�͊O���ɂ͂������悤�ɂ���
		if (OtherActor->ActorHasTag(TEXT("Train")))
		{
			FVector LocalReflectVector = Quat.Inverse().RotateVector(Hit.Normal);
			//���˃x�N�g����i�s�����ɐݒ�
			m_AxisAccel = FVector4(LocalReflectVector, m_AxisAccel.W);
		}
		//�d�ԈȊO�͒��˕Ԃ�̏������s��
		else
		{
			//���͎����擾
			FVector AxisAccle = m_AxisAccel;
			//���[���h���W�ɕϊ�
			FVector WorldDir = Quat.RotateVector(AxisAccle);
			//���˃x�N�g�������߂�
			FVector ReflectVector = WorldDir - Hit.Normal * (2.f * (WorldDir | Hit.Normal));
			//���߂����˃x�N�g������͎��̍��W�ɕϊ�
			FVector LocalReflectVector = Quat.Inverse().RotateVector(ReflectVector);
			//���˃x�N�g����i�s�����ɐݒ�
			m_AxisAccel = FVector4(LocalReflectVector * m_Attenuation, m_AxisAccel.W);
		}
	}
}

//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
void ANetworkPlayerDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//���}�b�s���O
	InputComponent->BindAxis(TEXT("Throttle"), this, &ANetworkPlayerDrone::Input_Throttle);
	InputComponent->BindAxis(TEXT("Elevator"), this, &ANetworkPlayerDrone::Input_Elevator);
	InputComponent->BindAxis(TEXT("Aileron"), this, &ANetworkPlayerDrone::Input_Aileron);
	InputComponent->BindAxis(TEXT("Ladder"), this, &ANetworkPlayerDrone::Input_Ladder);
}

//�y���̓o�C���h�z�X���b�g��(�㉺)�̓��͂��������ꍇ�Ăяo�����֐�
void ANetworkPlayerDrone::Input_Throttle(float _axisValue)
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
void ANetworkPlayerDrone::Input_Elevator(float _axisValue)
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
void ANetworkPlayerDrone::Input_Aileron(float _axisValue)
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
void ANetworkPlayerDrone::Input_Ladder(float _axisValue)
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