//------------------------------------------------------------------------
// �t�@�C����		:Ring.cpp
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "Utility/GameUtility.h"
#include "Components/StaticMeshComponent.h"
#include "ColorLightComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DroneBase.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"

//�R���X�g���N�^
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pNiagaraEffectComp(NULL)
	, m_pColorLightComp(NULL)
	, m_RingNumber(0)
	, m_bIsPassBegin(false)
	, m_bIsPassed(false)
	, m_bDestroy(false)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.5f)
	, m_InvisibleCntRate(0.f)
	, m_pScaleCurve(NULL)
	, m_PassedDroneLocation(FVector::ZeroVector)
{
	//���b�V������
	m_pRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRingMesh"));
	if (m_pRingMesh)
	{
		RootComponent = m_pRingMesh;
	}

	//�i�C�A�K���̃G�t�F�N�g�R���|�[�l���g����
	m_pNiagaraEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("m_pNiagaraEffectComp"));
	if (m_pNiagaraEffectComp)
	{
		m_pNiagaraEffectComp->SetupAttachment(m_pRingMesh);
	}

	//�J���[�R���|�[�l���g����
	m_pColorLightComp = CreateDefaultSubobject<UColorLightComponent>(TEXT("m_pColorLightComp"));

	//�X�P�[���̑傫���̒l�����o���J�[�u�I�u�W�F�N�g����
	ConstructorHelpers::FObjectFinder<UCurveFloat> pRingScaleCurve(TEXT("CurveFloat'/Game/Effect/Ring/Curve/RingScale_Curve.RingScale_Curve'"));
	if (pRingScaleCurve.Succeeded())
	{
		m_pScaleCurve = pRingScaleCurve.Object;
	}

	//���t���[��Tick���Ăяo�����ǂ����̃t���O
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n���܂��͂��̃N���X�̃I�u�W�F�N�g���X�|�[�����ꂽ���P�x�����Ăяo�����֐�
void ARing::BeginPlay()
{
	Super::BeginPlay();

	if (!m_pRingMesh || !m_pColorLightComp || !m_pNiagaraEffectComp) { return; }

	//�I�[�o�[���b�v�J�n���ɌĂ΂��C�x���g�֐���o�^
	m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnComponentOverlapBegin);
	//�J���[�R���|�[�l���g�̏����ݒ�
	m_pColorLightComp->InitializeMaterialParameter(m_pRingMesh, true);
	m_pColorLightComp->Activate(true);

	//�G�t�F�N�g�R���|�[�l���g������
	m_pNiagaraEffectComp->SetRelativeLocation(FVector::ZeroVector);
	m_pNiagaraEffectComp->SetRelativeRotation(FRotator::ZeroRotator);
	m_pNiagaraEffectComp->SetRelativeScale3D(FVector::OneVector);
}

//���t���[���Ăяo�����֐�
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�J���[�X�V
	UpdateColor(DeltaTime);

	//�g�����X�t�H�[���X�V
	UpdateTransform(DeltaTime);
}

//�����O�̐F�̍X�V����
void ARing::UpdateColor(const float& DeltaTime)
{
	if (!m_pColorLightComp || !m_pRingMesh || !m_pNiagaraEffectComp) { return; }
	
	//�����O�̐F���X�V
	m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("EmissiveColor"), m_pColorLightComp->GetVectorColor());
	
	//�����O���v���C���[���������Ă�����
	if (m_bIsPassed)
	{
		//�G�t�F�N�g�̐F�����b�V���Ɠ����F�ɂ���
		m_pNiagaraEffectComp->SetNiagaraVariableLinearColor(TEXT("User.Color"), m_pColorLightComp->GetVectorColor());

		//���S�Ɍ����Ȃ��Ȃ�܂Ŏ��Ԃ��J�E���g
		if (m_MakeInvisibleTime > m_MakeInvisibleCnt)
		{
			m_MakeInvisibleCnt += DeltaTime;
			m_InvisibleCntRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
			const float MeshOpacity = FMath::Lerp(1.f, 0.f, m_InvisibleCntRate);

			//���b�V���̕s�����x��������
			m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), MeshOpacity);
		}
	}
}

//�����O�̃g�����X�t�H�[���X�V
void ARing::UpdateTransform(const float& DeltaTime)
{
	if (!m_bIsPassed) { return; }
	if (!m_pRingMesh || !m_pScaleCurve) { return; }

	//�J�E���g�̐i�s�x�ō��W�Ƒ傫�������߂�
	const FVector RingLocation = FMath::Lerp(GetActorLocation(), m_PassedDroneLocation, m_InvisibleCntRate);
	const float Scale = m_pScaleCurve->GetFloatValue(m_MakeInvisibleCnt);

	//�����O���k�߂Ȃ���v���C���[��ǂ��悤�Ɉړ�����
	if (Scale > 0.f)
	{
		SetActorScale3D(FVector(Scale));
		SetActorLocation(RingLocation);
	}
	else
	{
		m_bDestroy = true;
	}

	if (m_bDestroy)
	{
		//�����O�������Ȃ��Ȃ����烊���O������
		this->Destroy();
	}
}

//�����O���A�N�e�B�u������
void ARing::SetActivate(const bool& _isActive)
{
	if (!m_pRingMesh) { return; }

	//���b�V���̃I�[�o�[���b�v�C�x���g�Ɖ�����ON/OFF��؂�ւ���
	m_pRingMesh->SetGenerateOverlapEvents(_isActive);
	m_pRingMesh->SetHiddenInGame(!_isActive);

	//�����Ă��Ȃ��Ԃ͍X�V�̕K�v���Ȃ��̂�Tick��؂�
	PrimaryActorTick.bCanEverTick = _isActive;
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ARing::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//�^�O��Player�������ꍇ
	if (OtherActor->ActorHasTag(TEXT("Drone")))
	{
		//���̃����O���܂��ʉ߂���Ă��Ȃ��ꍇ
		if (!m_bIsPassed)
		{
			if (m_pNiagaraEffectComp && m_pRingMesh)
			{
				//�ʉ߂��ꂽ��ԂɕύX
				m_bIsPassBegin = true;
				m_bIsPassed = true;
				//�G�t�F�N�g�̍Đ�
				m_pNiagaraEffectComp->Activate();
				//�����O�̓����蔻���؂�
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}