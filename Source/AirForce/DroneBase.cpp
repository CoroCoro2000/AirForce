//------------------------------------------------------------------------
// �t�@�C����		:DroneBase.cpp
// �T�v				:�h���[���̃x�[�X�N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/04/19 �v���C���[�ƃG�l�~�[�̋��ʍ��̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameUtility.h"

//�R���X�g���N�^
ADroneBase::ADroneBase()
	: m_GameMode(EGAMEMODE::GAMEMODE_TPS)
	, m_DroneMode(EDRONEMODE::DRONEMODE_AUTOMATICK)
	, m_pBodyMesh(NULL)
	, m_pDroneCollision(NULL)
	, m_Wings{}
	, m_RPSMax(10.f)
	, m_WingAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_MoveDirectionFlag()
	, m_StateFlag()
	, m_TiltLimit(45.f)
	, m_Speed(7.f)
	, m_SpeedPerSecondMax(50.f)
	, m_AxisAccel(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_Acceleration(0.f)
	, m_Deceleration(0.96f)
	, m_Turning(0.6f)
	, m_DroneWeight(0.3f)
	, m_Velocity(FVector::ZeroVector)
	, m_CentrifugalForce(FVector::ZeroVector)
	, m_AngularVelocity(FVector::ZeroVector)
	, m_GravityScale(0.98f)
	, m_Gravity(FVector::ZeroVector)
	, m_DescentTime(0.f)
	, m_pWingRotationSE(NULL)
	, m_RingAcquisition(0)
	, m_HeightMax(400.f)
	, m_HeightFromGround(0.f)
	, m_DistanceToSlope(0.f)
	, m_isControl(false)
	, m_isFloating(true)
	, m_AxisValuePerFrame(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_SaveVelocity(FVector::ZeroVector)
	, m_SaveQuat(FQuat::Identity)
	, m_pWindEffect(NULL)
	, m_WindRotationSpeed(5.f)

{
	//���g��Tick()�𖈃t���[���Ăяo�����ǂ���
	PrimaryActorTick.bCanEverTick = true;

	//�h���[���̓����蔻�萶��
	m_pDroneCollision = CreateDefaultSubobject<USphereComponent>(TEXT("DroneCollision"));
	if (m_pDroneCollision)
	{
		RootComponent = m_pDroneCollision;
		m_pDroneCollision->SetSphereRadius(13.f);
	}

	//�{�f�B�̃��b�V���A�Z�b�g��T��
	ConstructorHelpers::FObjectFinder<UStaticMesh> pBodyMesh(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Body/CGAXR_BODY.CGAXR_BODY'"));
	//�{�f�B���b�V������
	m_pBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	//�{�f�B���b�V���̌����A�����ɐ���������
	if (m_pBodyMesh && pBodyMesh.Succeeded())
	{
		//���b�V���̃Z�b�g�A�b�v
		m_pBodyMesh->SetupAttachment(m_pDroneCollision);
		m_pBodyMesh->SetStaticMesh(pBodyMesh.Object);
	}

	//�H�̃��b�V���A�Z�b�g��T��
	//�E�˂��ƍ��˂��̉H���擾����
	ConstructorHelpers::FObjectFinder<UStaticMesh> pRightTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> pLeftTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST'"));
	
	//�H�̃��b�V�������ɐ���������H�̐�������
	if (!pRightTwistWing.Succeeded() || !pLeftTwistWing.Succeeded()) { return; }
	for (int index = 0; index < EWING::NUM; ++index)
	{
		//�E���̉H�𒲂ׂ�
		const bool isRightTrun = (index == 0 || index == 3) ? true : false;

		const FName WingName = isRightTrun ?
			(index + 1 < 3) ? "LF_Wing" : "RB_Wing" :
			(index + 1 > 2) ? "LB_Wing" : "RF_Wing";
		FRotator InitRotaion = FRotator::ZeroRotator;
		InitRotaion.Yaw = (index < 2) ?
			(index == 0) ? -45.f : 45.f :
			(index == 2) ? 45.f : -45.f;

		//�z��̒ǉ�(���ʔԍ��A�H�̃��b�V��)
		m_Wings[index] = FWing(FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName)));

		if (m_Wings[index].GetWingMesh())
		{
			//�H�̃��b�V����ݒ�
			m_Wings[index].GetWingMesh()->SetStaticMesh((isRightTrun ? pLeftTwistWing.Object : pRightTwistWing.Object));
			//�{�f�B�ɃA�^�b�`����
			m_Wings[index].GetWingMesh()->SetupAttachment(m_pBodyMesh);
			//�H�̃��b�V���R���W�����𖳌��ɂ���
			m_Wings[index].GetWingMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//�\�P�b�g�̈ʒu�ɉH���A�^�b�`
			m_Wings[index].GetWingMesh()->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, WingName);
			m_Wings[index].GetWingMesh()->SetRelativeRotation(InitRotaion);
		}
	}
}

//�Q�[���J�n����1�x��������
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pDroneCollision)
	{
		//�I�[�o�[���b�v�A�q�b�g���̃C�x���g�֐����o�C���h
		m_pDroneCollision->OnComponentBeginOverlap.AddDynamic(this, &ADroneBase::OnDroneCollisionOverlapBegin);
		m_pDroneCollision->OnComponentHit.AddDynamic(this, &ADroneBase::OnDroneCollisionHit);
	}

	//����*�d�͉����x���d�͂ɐݒ�
	m_Gravity = FVector(0.f, 0.f, m_DroneWeight * m_GravityScale);
}

//���t���[������
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�X�e�[�g�X�V����
	//UpdateState();
	// 
	//�H�̉����x�X�V����
	UpdateWingAccle(DeltaTime);

	//�H�̉�]�X�V����
	UpdateWingRotation(DeltaTime);

	//��]����
	UpdateRotation(DeltaTime);

	//���x�X�V����
	UpdateSpeed(DeltaTime);

}

//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//�X�e�[�g�X�V����
void ADroneBase::UpdateState()
{

}

//�H�̉����x�X�V����
void ADroneBase::UpdateWingAccle(const float& DeltaTime)
{
}

//�H�̉�]�X�V����
void ADroneBase::UpdateWingRotation(const float& DeltaTime)
{
	//2���̓��͗ʂ���������
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValuePerFrame.W, m_AxisValuePerFrame.Z).Size() +
		FVector2D(m_AxisValuePerFrame.X, m_AxisValuePerFrame.Y).Size()) / 2,
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

//��]����
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }

	//�H�̉�]�ʂ���h���[���̊p���x�̍ő�l��ݒ�
	m_AngularVelocity = FVector(
		(m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState),
		(m_Wings[EWING::LEFT_BACKWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_FORWARD].AccelState),
		(m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState));
	m_AngularVelocity.Z = FMath::Abs(m_AngularVelocity.Z) * m_AxisAccel.W;

	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();

	BodyRotation.Pitch += m_AngularVelocity.Y;
	BodyRotation.Roll += m_AngularVelocity.X;

	//	�I�[�g�}�`�b�N�ő��삷��Ƃ�
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float deg = 25.f;

		if (m_AngularVelocity.Y != 0.f)
		{
			//�O��Ƀh���[�����X�������Ȃ��悤�ɕ␳
			if (BodyRotation.Pitch > deg)
			{
				BodyRotation.Pitch = deg;
			}
			else if (BodyRotation.Pitch < -deg)
			{
				BodyRotation.Pitch = -deg;
			}
		}
		else
		{
			if (FMath::Abs(CGameUtility::SetDecimalTruncation(BodyRotation.Pitch, 3)) != 0.f)
			{
				BodyRotation.Pitch *= m_Deceleration;
			}
		}

		if (m_AngularVelocity.X != 0.f)
		{

			if (BodyRotation.Roll > deg)
			{
				BodyRotation.Roll = deg;
			}
			else if (BodyRotation.Roll < -deg)
			{
				BodyRotation.Roll = -deg;
			}
		}
		else
		{
			if (FMath::Abs(CGameUtility::SetDecimalTruncation(BodyRotation.Roll, 3)) != 0.f)
			{
				BodyRotation.Roll *= m_Deceleration;
			}
			else
			{
				BodyRotation.Roll = 0.f;
			}
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
	//FQuat qAngularVelocity = FQuat::MakeFromEuler(m_AngularVelocity);
	//m_pBodyMesh->AddLocalRotation(qAngularVelocity * MOVE_CORRECTION, true);

	FRotator NewRotation = BodyRotation;
	NewRotation.Yaw += m_AngularVelocity.Z;
	m_pBodyMesh->SetRelativeRotation(NewRotation.Quaternion() * MOVE_CORRECTION, true);
	m_SaveQuat = m_pBodyMesh->GetRelativeRotation().Quaternion();
}

//���x�X�V����
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	//�I�[�g�}�`�b�N�ő��삷��Ƃ�
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
		FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

		m_Velocity = FVector::ZeroVector;

		m_Velocity += BodyQuat.GetRightVector() * m_Speed * m_AxisAccel.X * (IsReverseInput(m_AxisAccel.X, m_AxisValuePerFrame.X) ? m_Turning : 1.f);
		m_Velocity += BodyQuat.GetForwardVector() * m_Speed * -m_AxisAccel.Y * (IsReverseInput(m_AxisAccel.Y, m_AxisValuePerFrame.Y) ? m_Turning : 1.f);
		m_Velocity += BodyQuat.GetUpVector() * m_Speed * m_AxisAccel.Z * (IsReverseInput(m_AxisAccel.Z, m_AxisValuePerFrame.Z) ? m_Turning : 1.f);

		//����ŃN�����v
		m_Velocity = CGameUtility::SetDecimalTruncation(m_Velocity, 3);
		//���x����𒴂��Ă����玩���I�ɍ��x��������
		if (IsOverHeightMax())
		{
			m_Velocity.Z = -3.f;
		}

	}

	//�}�j���A���ő��삷��Ƃ�
	else
	{
		const FVector Direction = m_pBodyMesh->GetUpVector();
		//���͂̑傫���𑪂�
		float Buoyancy = 0.f;
		for (const FWing& wing : m_Wings)
		{
			Buoyancy += wing.AccelState;
		}
		Buoyancy /= (float)EWING::NUM;

#ifdef DEGUG_ACCEL
		UE_LOG(LogTemp, Warning, TEXT("Buoyancy:%f"), Buoyancy);
#endif

		//���͂��z�o�����O��Ԃ��傫���Ƃ�
		if (Buoyancy > 0.f)
		{
			if (m_Acceleration < 5.f)
			{
				m_Acceleration += Buoyancy * DeltaTime;
			}
		}
		//���͂��z�o�����O��Ԃ�菬������
		else if (Buoyancy < 0.f)
		{
			if (m_Acceleration > -2.f)
			{
				m_Acceleration += Buoyancy * DeltaTime;
			}
		}
		//���͂��d�͂ƒނ荇����(�z�o�����O���)
		else if (Buoyancy == 0.f)
		{
			m_Acceleration *= 59.f * DeltaTime;
		}

		//���i�͂̐ݒ�
		FVector Propulsion = Direction * (m_Acceleration + m_Gravity.Z);

		//�X�������鎞
		if (Direction.Z < 1.f)
		{
			float Centrifugal = 1.f - Direction.Z;
			Propulsion.X += Direction.X * Centrifugal;
			Propulsion.Y += Direction.Y * Centrifugal;
		}

#ifdef DEGUG_ACCEL
		UE_LOG(LogTemp, Warning, TEXT("Propulsion:%s"), *Propulsion.ToString());
#endif
		////�d�͂𔲂����ړ��ʂ�ێ�����
		//m_Velocity = Propulsion;
		////�d�͂����Z
		//Propulsion.Z += UpdateGravity(DeltaTime);

		////�ړ�����
		//AddActorWorldOffset(Propulsion * MOVE_CORRECTION, true);

#ifdef DEGUG_ACCEL
		UE_LOG(LogTemp, Warning, TEXT("Move:%s"), *Propulsion.ToString());
#endif
	}

}

//�d�͍X�V����
float ADroneBase::UpdateGravity(const float& DeltaTime)
{
	float newGravity = m_Gravity.Z;
	const float UpForce = m_pBodyMesh->GetUpVector().Z;
	//������̗͂��Ȃ���
	if (UpForce < 0.f)
	{
		//�d�͉����x���v�Z
		newGravity = (m_DescentTime * m_DescentTime * (-m_GravityScale) * 0.5f) + (m_DescentTime * m_Velocity.Z);

		//�󒆂ɂ���Ȃ�
		if (m_isFloating)
		{
			//�������Ԃ𑝉�
			m_DescentTime += DeltaTime;
		}
	}
	else
	{
		m_DescentTime = 0.f;
		newGravity *= -1.f;
	}

#ifdef DEBUG_GRAVITY
	UE_LOG(LogTemp, Warning, TEXT("newGravity%f"), newGravity);
#endif // DEBUG_GRAVITY

	return newGravity;
}

//���̃G�t�F�N�g�X�V����
void ADroneBase::UpdateWindEffect(const float& DeltaTime)
{

	if (m_pWindEffect)
	{
		//�G�t�F�N�g�ƃh���[���̍��W���擾
		FVector EffectLocation = m_pWindEffect->GetComponentLocation();
		FVector  DroneLocation = m_pBodyMesh->GetComponentLocation();
		//�G�t�F�N�g���i�s�����֌����悤�ɂ���
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - EffectLocation).Rotator();
		//�ړ��ʂ̑傫������G�t�F�N�g�̕s�����x��ݒ�
		float AccelValue = FMath::Clamp(FVector2D(m_Velocity.X, m_Velocity.Y).Size() / SPEED_MAX, 0.f, 1.f);
		float WindOpacity = (m_AxisAccel.Y < 0.f) ? AccelValue * 0.7f : 0.f;
		float WindMask = FMath::Lerp(50.f, 40.f, AccelValue);
		float effectScale = FMath::Lerp(2.f, 1.f, AccelValue);
		float effectLocationX = FMath::Lerp(-40.f, 0.f, AccelValue);

		m_pWindEffect->SetRelativeScale3D(FVector(effectScale));
		m_pWindEffect->SetWorldRotation(LookAtRotation.Quaternion());
		m_pWindEffect->SetRelativeLocation(FVector(effectLocationX, 0.f, 0.f));
		//�G�t�F�N�g�̕s�����x��ύX
		m_pWindEffect->SetVariableFloat(TEXT("User.Mask"), WindOpacity);
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

//���x�̏�����𒴂��Ă��邩�m�F
bool ADroneBase::IsOverHeightMax()
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

	//���C���q�b�g������A�N�^�[�̃^�O���m�F���AGround�̃^�O�����A�N�^�[������΍��x������z���Ă��Ȃ��̂Ńt���O���~�낷
	if (isHit)
	{
		for (const FHitResult& HitResult : OutHits)
		{
			if (HitResult.GetActor())
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Ground")))
				{
					OverHeightMax = false;
					//�n�ʂ���̍������v��
					m_HeightFromGround = FVector::Dist(GetActorLocation(), HitResult.Location);
					break;
				}
			}
		}
	}
#ifdef DEBUG_IsOverHeightMax
	//������z�����物�F�A�z���Ă��Ȃ��Ȃ��
	FColor LineColor = OverHeightMax ? FColor::Yellow : FColor::Blue;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.f);
#endif // DEBUG_IsOverHeightMax

	return OverHeightMax;
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ADroneBase::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�^�O��Player�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			return;
		}

		//�^�O��Ring�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Ring")))
		{
			m_RingAcquisition++;
		}
	}
#ifdef DEBUG_CollisionOverlap_Begin
	UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
#endif // DEBUG_OVERLAP_BEGIN
}

//�h���[���̓����蔻��ɃI�u�W�F�N�g���q�b�g�������Ă΂��C�x���g�֐���o�^
void ADroneBase::OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		m_isFloating = false;

		if (FVector(m_AxisAccel).GetAbsMax() > 0.1f)
		{

			FVector progressVector = m_AxisAccel;

			//�q�b�g�����A�N�^�[�̖@���x�N�g�����擾
			FVector HitActorNormal = Hit.Normal;

			//�i�s�x�N�g���Ɩ@���x�N�g���̓��ς����߂�
			float dot = progressVector | HitActorNormal;

			//���˃x�N�g�������߂�
			FVector reflectVector = progressVector - dot * 2.f * HitActorNormal;

			//���˃x�N�g����i�s�����ɐݒ�
			m_AxisAccel = reflectVector * 0.5f;
		}



		m_isFloating = true;
	}
}