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
	, m_DroneWeight(0.15f)
	, m_Velocity(FVector::ZeroVector)
	, Centrifugalforce(FVector::ZeroVector)
	, m_AngularVelocity(FVector::ZeroVector)
	, Gravity(FVector::ZeroVector)
	, m_GravityScale(9.8f)
	, m_DescentTime(0.f)
	, m_CenterOfGravity(FVector::ZeroVector)
	, m_isControl(false)
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
		m_pWings.Add(new FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName)));

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

//�f�X�g���N�^
ADroneBase::~ADroneBase()
{
	for (FWing* pWing : m_pWings)
	{
		//�̈�̊J��
		if (pWing)
		{
			delete pWing;
		}
	}
}

//�Q�[���J�n����1�x��������
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	
	m_pDroneBoxComp->OnComponentBeginOverlap.AddDynamic(this,&ADroneBase::OnOverlapBegin);

	//����*�d�͉����x���d�͂ɐݒ�
	Gravity = FVector(0.f, 0.f, m_DroneWeight * m_GravityScale * -1.f);
}

//���̃I�u�W�F�N�g���j�������Ƃ��ɌĂяo�����֐�
void ADroneBase::BeginDestory()
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
	//���݂�1�t���[���O�̈ړ��ʂ���P�t���[���Ԃ̈ړ��ʂ��v�Z
	m_PrevCurrentLocation = m_CurrentLocation;
	m_CurrentLocation = GetActorLocation();
}

//�ړ�����
void ADroneBase::UpdateMove(const float& DeltaTime)
{
	//�h���[���̌X���������x�N�g���ɕϊ�
	const FVector unitDirection = GetActorRotation().Vector();

	//UE_LOG(LogTemp, Warning, TEXT("GetWingNormalizeAccele%f"), GetWingNormalizeAccele());
}

//�H�̉�]�X�V����
void ADroneBase::UpdateWingRotation(const float& DeltaTime)
{
}
//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ADroneBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	  //�^�O��Player�������ꍇ
	if (OtherActor->ActorHasTag(TEXT("Ring")))
	{
		m_RingAcquisition++;
	}
}