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
#include "GameUtility.h"

//�R���X�g���N�^
ADroneBase::ADroneBase()
	: m_pBodyMesh(NULL)
	, m_pDroneCollision(NULL)
	, m_RPSMax(10.f)
	, m_WingAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_CurrentLocation(FVector::ZeroVector)
	, m_PrevCurrentLocation(FVector::ZeroVector)
	, m_OldRotation(FRotator::ZeroRotator)
	, m_SpeedPerSecondMax(50.f)
	, m_AxisAccel(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_Acceleration(0.f)
	, m_DroneWeight(0.3f)
	, m_Velocity(FVector::ZeroVector)
	, Centrifugalforce(FVector::ZeroVector)
	, m_AngularVelocity(FVector::ZeroVector)
	, Gravity(FVector::ZeroVector)
	, m_GravityScale(0.98f)
	, m_DescentTime(0.f)
	, m_CenterOfGravity(FVector::ZeroVector)
	, m_isControl(false)
	, m_isFloating(true)
	, m_RingAcquisition(0)
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
	Gravity = FVector(0.f, 0.f, m_DroneWeight * m_GravityScale);
}

//���t���[������
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�X�e�[�g�X�V����
	UpdateState();

	//�d�S�ړ�����
	UpdateCenterOfGravity(DeltaTime);

	//��]����
	//UpdateRotation(DeltaTime);

	//���x�X�V����
	UpdateSpeed(DeltaTime);

	//�ړ�����
	UpdateMove(DeltaTime);

}

//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//�H�̉����x�X�V����
void ADroneBase::UpdateWingAccle()
{
}

//�X�e�[�g�X�V����
void ADroneBase::UpdateState()
{

}


//�d�S�ړ�����
void ADroneBase::UpdateCenterOfGravity(const float& DeltaTime)
{

}

//��]����
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
}

//���x�X�V����
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

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
	FVector Propulsion = Direction * (m_Acceleration + Gravity.Z);

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
	//�d�͂𔲂����ړ��ʂ�ێ�����
	m_Velocity = Propulsion;
	//�d�͂����Z
	Propulsion.Z += UpdateGravity(DeltaTime);

	m_Speed = Propulsion.Size();

	//�ړ�����
	AddActorWorldOffset(Propulsion * MOVE_CORRECTION, true);

#ifdef DEGUG_ACCEL
	UE_LOG(LogTemp, Warning, TEXT("Move:%s"), *Propulsion.ToString());
#endif

}

//�ړ�����
void ADroneBase::UpdateMove(const float& DeltaTime)
{

}

//�H�̉�]�X�V����
void ADroneBase::UpdateWingRotation(const float& DeltaTime)
{
}

//�d�͍X�V����
float ADroneBase::UpdateGravity(const float& DeltaTime)
{
	float newGravity = Gravity.Z;
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

//���̉e����^����͈͂̍X�V
void ADroneBase::UpdateWindRangeLineTrace(const float& DeltaTime)
{
	//UWorld::LineTraceMultiByChannel;
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ADroneBase::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�^�O��Player�������ꍇ
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

		FVector progressVector = m_AxisAccel;

		//�q�b�g�����A�N�^�[�̖@���x�N�g�����擾
		FVector HitActorNormal = Hit.Normal;

		//�i�s�x�N�g���Ɩ@���x�N�g���̓��ς����߂�
		float dot = progressVector | HitActorNormal;

		//���˃x�N�g�������߂�
		FVector reflectVector = progressVector - dot * 2.f * HitActorNormal;

		//���˃x�N�g����i�s�����ɐݒ�
		m_AxisAccel = reflectVector * 0.5f;
		m_isFloating = true;
	}
}