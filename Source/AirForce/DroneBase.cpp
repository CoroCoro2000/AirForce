//------------------------------------------------------------------------
// �t�@�C����		:DroneBase.cppAcceleration
// �T�v				:�h���[���̃x�[�X�N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/04/19 �v���C���[�ƃG�l�~�[�̋��ʍ��̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Utility/GameUtility.h"

//�R���X�g���N�^
ADroneBase::ADroneBase()
	: m_pBodyMesh(NULL)
	, m_rpsMax(10.f)
	, m_WingAccele(0.f)
	, m_WingOldAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_pDroneBoxComp(NULL)
	, m_MoveDirectionFlag(0)
	, m_StateFlag(0)
	, m_CurrentLocation(FVector::ZeroVector)
	, m_PrevCurrentLocation(FVector::ZeroVector)
	, m_OldRotation(FRotator::ZeroRotator)
	, m_SpeedPerSecondMax(50.f)
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

	//�{�f�B�̃��b�V���A�Z�b�g��T��
	ConstructorHelpers::FObjectFinder<UStaticMesh> pBodyMesh(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Body/CGAXR_BODY.CGAXR_BODY'"));
	//�{�f�B���b�V������
	m_pBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	//�{�f�B���b�V���̌����A�����ɐ���������
	if (m_pBodyMesh && pBodyMesh.Succeeded())
	{
		//���b�V���̃Z�b�g�A�b�v
		RootComponent = m_pBodyMesh;
		m_pBodyMesh->SetStaticMesh(pBodyMesh.Object);
	}

	//�H�̃��b�V���A�Z�b�g��T��
	//�E�˂��ƍ��˂��̉H���擾����
	ConstructorHelpers::FObjectFinder<UStaticMesh> pRightTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> pLeftTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST'"));
	
	//�H�̃��b�V�������ɐ���������H�̐�������
	if (!pRightTwistWing.Succeeded() || !pLeftTwistWing.Succeeded()) { return; }
	for (int index = 0; index < WING_ARRAY_MAX; ++index)
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
		TSharedPtr<FWing> pWing = CGameUtility::CreateSharedPtr<FWing>(FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName)));
		m_pWings.Add(pWing);

		if (!m_pWings[index]) { return; }
		if (!m_pWings[index]->GetWingMesh()) { return; }

		//�H�̃��b�V����ݒ�
		m_pWings[index]->GetWingMesh()->SetStaticMesh((isRightTrun ? pLeftTwistWing.Object : pRightTwistWing.Object));
		//�{�f�B�̃��b�V���ɃA�^�b�`����
		m_pWings[index]->GetWingMesh()->SetupAttachment(m_pBodyMesh);
		//�\�P�b�g�̈ʒu�ɉH���A�^�b�`
		m_pWings[index]->GetWingMesh()->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, WingName);
		m_pWings[index]->GetWingMesh()->SetRelativeRotation(InitRotaion);
	}
}

//�Q�[���J�n����1�x��������
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	

	if (m_pDroneBoxComp)
	{
		//�I�[�o�[���b�v���̃C�x���g�֐����o�C���h
		m_pDroneBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ADroneBase::OnComponentOverlapBegin);
		m_pDroneBoxComp->OnComponentEndOverlap.AddDynamic(this, &ADroneBase::OnComponentOverlapEnd);
	}

	//����*�d�͉����x���d�͂ɐݒ�
	Gravity = FVector(0.f, 0.f, m_DroneWeight * m_GravityScale);
}

//���̃I�u�W�F�N�g���j�������Ƃ��ɌĂяo�����֐�
void ADroneBase::BeginDestory()
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
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�X�e�[�g�X�V����
	UpdateState();

	//�d�S�ړ�����
	UpdateCenterOfGravity(DeltaTime);

	//��]����
	UpdateRotation(DeltaTime);

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
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (pWing.IsValid())
		{
			Buoyancy += pWing->AccelState;
		}
	}
	Buoyancy /= (float)WING_ARRAY_MAX;

#ifdef DEGUG_ACCEL
	UE_LOG(LogTemp, Warning, TEXT("Buoyancy:%f"), Buoyancy);
#endif

	//���͂��z�o�����O��Ԃ��傫���Ƃ�
	if (Buoyancy > BUOYANCY_HOVERING)
	{
		if (m_Acceleration < 5.f)
		{
			m_Acceleration += Buoyancy * DeltaTime;
		}
	}
	//���͂��z�o�����O��Ԃ�菬������
	else if (Buoyancy < BUOYANCY_HOVERING)
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

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ADroneBase::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		//�^�O��Player�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Ring")))
		{
			m_RingAcquisition++;
		}
		
		//�����O�ȊO�ƃI�[�o�[���b�v������
		if (!OtherActor->ActorHasTag(TEXT("Ring")))
		{
			m_isFloating = false;
		}
	}
#ifdef DEBUG_OVERLAP_BEGIN
	UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
#endif // DEBUG_OVERLAP_BEGIN
}

//�I�[�o�[���b�v���Ă����A�N�^�[���痣�ꂽ�u�ԌĂ΂��C�x���g�֐�
void ADroneBase::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != this)
	{
		//�����O�ȊO�̃I�[�o�[���b�v���Ă����A�N�^�[���痣�ꂽ��
		if (!OtherActor->ActorHasTag(TEXT("Ring")))
		{
			m_isFloating = true;
		}
#ifdef DEBUG_OVERLAP_END
		UE_LOG(LogTemp, Warning, TEXT("OverlapEnd"));
#endif // DEBUG_OVERLAP_END
	}
}