//------------------------------------------------------------------------
// �t�@�C����		:DroneBase.cpp
// �T�v				:�h���[���̃x�[�X�N���X
// �쐬��			:2021/04/19
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/04/19 �v���C���[�ƃG�l�~�[�̋��ʍ��̒ǉ�
//------------------------------------------------------------------------

//�C���N���[�h
#include "DroneBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameUtility.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif // WITH_EDITOR

//�R���X�g���N�^
ADroneBase::ADroneBase()
	: m_pBodyMesh(NULL)
	, m_pDroneCollision(NULL)
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
	, m_AxisValuePerFrame(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_LocalAxis(FVector::ZeroVector)
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
	, m_pCloudOfDustEmitter(NULL)
	, m_ShowEffectDistance(50.f)
	, m_GroundMaterialName(TEXT(""))
{
	//���g��Tick()�𖈃t���[���Ăяo�����ǂ���
	PrimaryActorTick.bCanEverTick = true;

	//���b�V���A�Z�b�g�̃Z�b�g�A�b�v
	MeshAssetSetup();
	//�R���W���������ݒ�
	InitializeCollision();
	//���b�V���̏����ݒ�
	(GetLocalRole() == ENetRole::ROLE_Authority) ? InitializeMesh() : Client_InitializeMesh_Implementation();
	
	//�^�O��ǉ�
	Tags.Add(TEXT("Drone"));

	//�v���C���[�����R���g���[����
	Role = ROLE_Authority;
	//�����Ώۃt���O
	bReplicates = true;
	//���L�������N���C�A���g�݂̂ɓ�������
	bOnlyRelevantToOwner = false;
}

//�f�X�g���N�^
ADroneBase::~ADroneBase()
{
	for (int32 index = m_pWings.Num() - 1; index >= 0; --index)
	{
		if (m_pWings.IsValidIndex(index))
		{
			m_pWings[index].Reset();
		}
	}
}

//�Q�[���J�n����1�x��������
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();

	//���C�g�̏����ݒ�
	(GetLocalRole() == ENetRole::ROLE_Authority) ? InitializeLight() : Client_InitializeLight_Implementation();

	if (m_pDroneCollision)
	{
		//�I�[�o�[���b�v�A�q�b�g���̃C�x���g�֐����o�C���h
		m_pDroneCollision->OnComponentBeginOverlap.AddDynamic(this, &ADroneBase::OnDroneCollisionOverlapBegin);
		m_pDroneCollision->OnComponentHit.AddDynamic(this, &ADroneBase::OnDroneCollisionHit);
	}
}

//���v���P�[�g��o�^
void ADroneBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADroneBase, m_isControl);
}

//���t���[������
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�y���̓o�C���h�z�R���g���[���[���͐ݒ�
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//���b�V���A�Z�b�g�̃Z�b�g�A�b�v
void ADroneBase::MeshAssetSetup()
{
	m_BodyMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/NewDrone/Drone.Drone"))).LoadSynchronous();

	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST"))).LoadSynchronous());
	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST"))).LoadSynchronous());
}

//�R���W�����̏����ݒ�
void ADroneBase::InitializeCollision()
{
	//�h���[���̓����蔻�萶��
	m_pDroneCollision = CreateDefaultSubobject<USphereComponent>(TEXT("DroneCollision"));
	if (m_pDroneCollision)
	{
		RootComponent = m_pDroneCollision;
		m_pDroneCollision->SetSphereRadius(8.f);
	}
}

//���b�V���̏����ݒ�
void ADroneBase::InitializeMesh()
{
	if (!m_pDroneCollision) { return; }

	//�{�f�B���b�V������
	m_pBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	if (m_pBodyMesh && m_BodyMesh)
	{
		//���b�V���̃A�^�b�`
		m_pBodyMesh->AttachToComponent(m_pDroneCollision, FAttachmentTransformRules::KeepRelativeTransform);
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

//�N���C�A���g�p�@InitializeMesh
void ADroneBase::Client_InitializeMesh_Implementation()
{
	InitializeMesh();
}

//���C�g�̏����ݒ�
void ADroneBase::InitializeLight()
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

//�N���C�A���g�p ���C�g�̏����ݒ�
void ADroneBase::Client_InitializeLight_Implementation()
{
	InitializeLight();
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

//��]����
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
	//NULL�`�F�b�N
	if (!m_pBodyMesh) { return; }

	//�H�̉�]�ʂ���h���[���̊p���x�̍ő�l��ݒ�
	FVector AngularVelocity;
	AngularVelocity = FVector(
		(m_pWings[EWING::LEFT_FORWARD]->AccelState + m_pWings[EWING::LEFT_BACKWARD]->AccelState) - (m_pWings[EWING::RIGHT_FORWARD]->AccelState + m_pWings[EWING::RIGHT_BACKWARD]->AccelState),
		(m_pWings[EWING::LEFT_BACKWARD]->AccelState + m_pWings[EWING::RIGHT_BACKWARD]->AccelState) - (m_pWings[EWING::LEFT_FORWARD]->AccelState + m_pWings[EWING::RIGHT_FORWARD]->AccelState),
		(m_pWings[EWING::RIGHT_FORWARD]->AccelState + m_pWings[EWING::LEFT_BACKWARD]->AccelState) - (m_pWings[EWING::LEFT_FORWARD]->AccelState + m_pWings[EWING::RIGHT_BACKWARD]->AccelState));
	AngularVelocity.Z = FMath::Abs(AngularVelocity.Z) * m_AxisAccel.W;

	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();

	float deg = 25.f;
	float RotationSpeed = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
	BodyRotation.Pitch = FMath::Lerp(BodyRotation.Pitch, m_AxisValuePerFrame.Y * deg, RotationSpeed);
	float HorizontalAxis = (FMath::Abs(m_AxisValuePerFrame.X) > FMath::Abs(m_AxisValuePerFrame.W) ? m_AxisValuePerFrame.X : m_AxisValuePerFrame.W);
	BodyRotation.Roll = FMath::Lerp(BodyRotation.Roll, HorizontalAxis * deg, RotationSpeed);

	FRotator NewRotation = BodyRotation;
	NewRotation.Yaw += AngularVelocity.Z * 0.7f;
	m_pBodyMesh->SetRelativeRotation(NewRotation.Quaternion() * MOVE_CORRECTION, true);
}

//���x�X�V����
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
	FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

	m_LocalAxis = FVector::ZeroVector;
	FVector NormalizeValue = FVector(m_AxisAccel).GetAbs().GetSafeNormal();
	m_LocalAxis += BodyQuat.GetRightVector() * m_AxisAccel.X;
	m_LocalAxis += BodyQuat.GetForwardVector() * -m_AxisAccel.Y;
	m_LocalAxis += BodyQuat.GetUpVector() * m_AxisAccel.Z;

	//�h���[���ɂ�����͂̐ݒ�
	m_Velocity = CGameUtility::SetDecimalTruncation(m_LocalAxis * m_Speed, 3);

	//���x����𒴂��Ă����玩���I�ɍ��x��������
	if (m_HeightFromGround >= m_HeightMax)
	{
		m_Velocity.Z = -3.f;
	}
}

//���̃G�t�F�N�g�X�V����
void ADroneBase::UpdateWindEffect(const float& DeltaTime)
{
	if (!m_pWindEmitter || !m_pBodyMesh) { return; }

	//�G�t�F�N�g�ƃh���[���̍��W���擾
	FVector EffectLocation = m_pWindEmitter->GetComponentLocation();
	FVector  DroneLocation = m_pBodyMesh->GetComponentLocation();
	//�G�t�F�N�g���i�s�����֌����悤�ɂ���
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - EffectLocation).Rotator();
	//�ړ��ʂ̑傫������G�t�F�N�g�̕s�����x��ݒ�
	float AxisValue = FVector2D(m_AxisValuePerFrame.X, m_AxisValuePerFrame.Y).GetSafeNormal().Size();
	//���������v�Z
	float AccelRate = FMath::Clamp(m_AxisAccel.Size3() / m_WingAccelMax, 0.f, 1.f);
	float Opacity = (m_AxisValuePerFrame.Y < 0.f) ? AxisValue * (m_bIsPassedRing ? 1.f : 0.8f) : 0.f;
	m_WindOpacity = FMath::Lerp(m_WindOpacity, Opacity, DeltaTime * 5.f);
	float WindNoise = (AxisValue != 0.f ? (m_bIsPassedRing ? 8.f : 18.f): 40.f);
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
void ADroneBase::UpdateAltitudeCheck()
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

					if(pHitActor->ActorHasTag(TEXT("LandScape")))
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
void ADroneBase::UpdateCloudOfDustEffect()
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
					m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pDroneCollision, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
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
				m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pDroneCollision, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
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

//�{�f�B���b�V���̉�]�ݒ�
void ADroneBase::SetBodyMeshRotation(const FRotator& NewRotator)
{
	if (!m_pBodyMesh) { return; }

	 m_pBodyMesh->SetRelativeRotation(NewRotator); 
}

//�{�f�B���b�V���̉�]�ݒ�
void ADroneBase::SetBodyMeshRotation(const FQuat& NewRotator)
{
	if (!m_pBodyMesh) { return; }

	m_pBodyMesh->SetRelativeRotation(NewRotator);
}

//�{�f�B���b�V���̉�]�ʎ擾
FRotator ADroneBase::GetBodyMeshRotation()const
{
	if (!m_pBodyMesh) { return FRotator::ZeroRotator; }

	return m_pBodyMesh->GetComponentRotation(); 
}

//�{�f�B���b�V���̉�]�ʎ擾
FRotator ADroneBase::GetBodyMeshRelativeRotation()const
{
	if (!m_pBodyMesh) { return FRotator::ZeroRotator; }

	return m_pBodyMesh->GetRelativeRotation();
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ADroneBase::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
#ifdef DEBUG_CollisionOverlap_Begin
	UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
#endif // DEBUG_OVERLAP_BEGIN
}

//�h���[���̓����蔻��ɃI�u�W�F�N�g���q�b�g�������Ă΂��C�x���g�֐���o�^
void ADroneBase::OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (m_pBodyMesh && OtherActor && OtherActor != this)
	{
		//�h���[���̌������擾
		FQuat Quat = FRotator(0.f, m_pBodyMesh->GetComponentRotation().Yaw + 90.f, 0.f).Quaternion();

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