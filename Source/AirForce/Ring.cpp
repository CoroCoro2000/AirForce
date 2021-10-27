//------------------------------------------------------------------------
// �t�@�C����		:Ring.cpp
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "GameUtility.h"
#include "DroneBase.h"
#include "PlayerDrone.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintFunctionUtility.h"

//�R���X�g���N�^
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pNiagaraEffectComp(NULL)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.5f)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_PassedSceleMax(3.f)
	, m_pPassedDrone(NULL)
	, m_bIsUnRotation(false)
	, m_HSV(30.f, 40.f, 30.f)
	, m_InitialTransform(FTransform(FQuat::Identity, FVector::ZeroVector, FVector::OneVector))
	, m_RingHitSE(NULL)
{
	//���t���[��Tick���Ăяo�����ǂ����̃t���O
	PrimaryActorTick.bCanEverTick = true;

	//���b�V������
	m_pRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRingMesh"));
	if (m_pRingMesh)
	{
		RootComponent = m_pRingMesh;
	}

	//�i�C�A�K���̃G�t�F�N�g�R���|�[�l���g����
	m_pNiagaraEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("m_pNiagaraEffectComp"));
	if (m_pNiagaraEffectComp && m_pRingMesh)
	{
		m_pNiagaraEffectComp->SetupAttachment(m_pRingMesh);
	}

	//�^�O�̒ǉ�
	Tags.Add(TEXT("Ring"));
}

//�Q�[���J�n���܂��͂��̃N���X�̃I�u�W�F�N�g���X�|�[�����ꂽ���P�x�����Ăяo�����֐�
void ARing::BeginPlay()
{
	Super::BeginPlay();

	if (m_pRingMesh)
	{
		//�I�[�o�[���b�v�J�n���ɌĂ΂��C�x���g�֐���o�^
		m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnComponentOverlapBegin);
	}

	//�����̃g�����X�t�H�[����ێ�
	m_InitialTransform = GetActorTransform();

}

//���t���[���Ăяo�����֐�
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_bIsPassed)
	{
		//�w��̎��Ԃ��z����܂Ŏ��Ԃ��v��
		if (m_MakeInvisibleCnt < m_MakeInvisibleTime)
		{
			m_MakeInvisibleCnt += DeltaTime;
		}
		else
		{
			Reset();
		}
	}

	//�����O�̃T�C�Y�X�V
	UpdateScale(DeltaTime);

	//�����O�̃}�e���A���X�V
	UpdateMaterial(DeltaTime);

	//�G�t�F�N�g�̍X�V
	UpdateEffect(DeltaTime);
}

//�����O�̃T�C�Y�X�V
void ARing::UpdateScale(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	//�����O�̃X�P�[��
	FVector RingScale = m_InitialTransform.GetScale3D();

	//�����O���ʉ߂���Ă��Ȃ��Ԃ͈��̎����ő傫����ς���
	if (!m_bIsPassed)
	{
		//�T�C���g�̕���ݒ�
		const float WaveWidth = m_SineWidth * GetWorld()->GetTimeSeconds();
		//�T�C���g�̑傫����0����1�ɐ��K������
		const float SinWave = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
		//�T�C���g�̒l�Ń����O�̑傫����ς���
		float ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, SinWave);

		//�V�����X�P�[����K�p
		SetActorScale3D(RingScale * ScaleMultiplier);
	}
	//�����O���ʉ߂��ꂽ��傫������
	else
	{
		if (m_pPassedDrone)
		{
			const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);

			//�o�ߗ��ő傫����ς���
			FVector Scale = FMath::Lerp(RingScale, RingScale * 0.05f, elapsedRate);
			SetActorScale3D(Scale);

			FRotator TargetRotation = m_bIsUnRotation ?
				m_pPassedDrone->GetBodyMeshRotation() * -1.f :
				m_pPassedDrone->GetBodyMeshRotation();
			
			//���X�ɍ��W�Ɖ�]���v���C���[�ɍ��킹��
			SetActorLocationAndRotation(
				FMath::Lerp(GetActorLocation(), m_pPassedDrone->GetActorLocation(), elapsedRate),
				FMath::Lerp(GetActorRotation(), TargetRotation, elapsedRate));
		}
	}
}

//�����O�̃}�e���A���X�V
void ARing::UpdateMaterial(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	if (m_bIsPassed)
	{
		//�������Ă���̌o�ߗ������߂�
		const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
		float ColorScale = m_HSV.R;
		ColorScale = FMath::Lerp(m_HSV.R, m_HSV.R * 0.5f, elapsedRate);

		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(FLinearColor(ColorScale, m_HSV.G, m_HSV.B).HSVToLinearRGB()));
		m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), 1.f - elapsedRate);
	}
	else
	{
		//�T�C���g�̕���ݒ�
		const float WaveWidth = m_SineWidth * GetWorld()->GetTimeSeconds();
		//�T�C���g�̑傫����0����1�ɐ��K������
		const float SinWave = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
		//�T�C���g�̒l�Ń����O�̐F����ς���
		m_HSV.R = FMath::Lerp(50.f, 30.f, SinWave);
		m_HSV.B = FMath::Lerp(30.f, 50.f, SinWave);
		
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
	}
}

//�����O�̃G�t�F�N�g�X�V
void ARing::UpdateEffect(const float& DeltaTime)
{
	if (!m_pNiagaraEffectComp || !m_pPassedDrone) { return; }

	if (m_bIsPassed)
	{
		FLinearColor PaticleColor = m_HSV;
		PaticleColor.B = 2.f;
		PaticleColor = FLinearColor::LerpUsingHSV(PaticleColor, PaticleColor * 0.5f, DeltaTime * 0.5f);
		m_pNiagaraEffectComp->SetVariableLinearColor(TEXT("User.Color"), FVector(PaticleColor.HSVToLinearRGB()));
	}
}

//�����O���t��]���邩�̃t���O
bool ARing::IsUnRotation()
{
	if (!m_pPassedDrone) { return false; }

	//�h���[���̌����ɍ��킹�ă����O�̌�����ύX����
	FRotator RingRotation = GetActorRotation();
	FRotator DroneRotation = m_pPassedDrone->GetBodyMeshRotation();
	FVector DroneRotationVector = DroneRotation.Vector();

	FVector RotationVector = RingRotation.RotateVector(DroneRotationVector);
	FVector UnRotationVector = RingRotation.UnrotateVector(DroneRotationVector);

	//�t��]�̂ق����h���[���̉�]�ɋ߂����true��Ԃ�
	return FVector::Dist(RotationVector, DroneRotationVector) > FVector::Dist(UnRotationVector, DroneRotationVector);
}

//�����O�̏�����
void ARing::Reset()
{
	m_bIsPassed = false;
	m_MakeInvisibleCnt = 0.f;
	m_HSV = FLinearColor(30.f, 40.f, 30.f);
	float ScrollSpeed = 0.5f;
	m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);
	SetActorTransform(m_InitialTransform);
	m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), 1.f);
	m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	m_pPassedDrone = NULL;
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ARing::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent && OtherActor && OtherActor != this)
	{
		//�^�O��Player�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			//���̃����O���܂��ʉ߂���Ă��Ȃ��ꍇ
			if (!m_bIsPassed)
			{
				//�ʉ߂��ꂽ��ԂɕύX
				m_bIsPassed = true;
				m_bIsUnRotation = IsUnRotation();

				//�}�e���A���̉�]���x���グ��
				float ScrollSpeed = 1.f;
				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);

				m_pPassedDrone = Cast<ADroneBase>(OtherActor);
				//�G�t�F�N�g�̍Đ�
				m_pNiagaraEffectComp->Activate();
				//SE�̍Đ�
				UGameplayStatics::PlaySound2D(GetWorld(), m_RingHitSE);
				//�����O�̓����蔻���؂�
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}