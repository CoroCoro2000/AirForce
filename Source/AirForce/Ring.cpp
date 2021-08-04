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
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DroneBase.h"
#include "PlayerDrone.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"

//�R���X�g���N�^
ARing::ARing()
	: m_pColorLightComp(NULL)
	, m_pRingMesh(NULL)
	, m_RingNumber(0)
	, m_RingDrawUpNumber(0)
	, m_pNiagaraEffectComp(NULL)
	, m_bIsPassed(false)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(3.f)
	, m_pScaleCurve(NULL)
{
	//�J���[�R���|�[�l���g����
	m_pColorLightComp = CreateDefaultSubobject<UColorLightComponent>(TEXT("m_pColorLightComp"));

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
	m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnOverlapBegin);

	//�h���[�����������A����ێ�����
	AActor* FindDrone = CGameUtility::GetActorFromTag(this, TEXT("Drone"));
	if (FindDrone)
	{
		m_pDrone = Cast<APlayerDrone>(FindDrone);
	}

	m_pColorLightComp->InitializeMaterialParameter(m_pRingMesh, true);
	m_pColorLightComp->Activate(true);

	m_pNiagaraEffectComp->SetRelativeLocation(FVector::ZeroVector);
	m_pNiagaraEffectComp->SetRelativeRotation(FRotator::ZeroRotator);
	m_pNiagaraEffectComp->SetRelativeScale3D(FVector::OneVector);
}

//���t���[���Ăяo�����֐�
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pColorLightComp && m_pRingMesh)
	{
		//�����O�̐F���X�V
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("EmissiveColor"), m_pColorLightComp->GetVectorColor());
	}
	m_pRingMesh->SetGenerateOverlapEvents(isDraw());
	m_pRingMesh->SetHiddenInGame(!isDraw());
	//�����O���ʉ߂��ꂽ�Ȃ�
	if (m_bIsPassed)
	{
		//�G�t�F�N�g�̐F�����b�V���Ɠ����F�ɂ���
		m_pNiagaraEffectComp->SetNiagaraVariableLinearColor(TEXT("User.Color"), m_pColorLightComp->GetVectorColor());
		if (m_MakeInvisibleTime > m_MakeInvisibleCnt)
		{
			//���S�Ɍ����Ȃ��Ȃ�܂Ŏ��Ԃ��J�E���g
			m_MakeInvisibleCnt += DeltaTime;
			if (m_pRingMesh && m_pNiagaraEffectComp && m_pScaleCurve && m_pDrone)
			{
				//���b�V���ƃG�t�F�N�g�̕s�����x�������Ă���
				const float MeshCountRate = FMath::Clamp(m_MakeInvisibleCnt / (m_MakeInvisibleTime * 0.5f), 0.f, 1.f);
				const float EffectCountRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
				const float MeshOpacity = FMath::Lerp(1.f, 0.f, MeshCountRate);
				const float EffectOpacity = FMath::Lerp(1.f, 0.f, EffectCountRate);
				const FVector RingLocation = FMath::Lerp(GetActorLocation(), m_pDrone->GetActorLocation(), EffectCountRate);
				const float Scale = m_pScaleCurve->GetFloatValue(m_MakeInvisibleCnt);

				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), MeshOpacity);
				m_pNiagaraEffectComp->SetNiagaraVariableFloat(TEXT("User.Opacity"), EffectOpacity);

				//�����O�����������Ă���

				if (Scale > 0.f)
				{
					SetActorScale3D(FVector(Scale));
					SetActorLocation(RingLocation);
				}
				
				if (EffectOpacity <= 0.f)
				{
					//�����O�������Ȃ��Ȃ�����j������
					this->Destroy();
				}
			}
		}
	}
}

bool ARing::isDraw()
{
	if (!m_pDrone)
	{
		return false;
	}

	return  (m_RingNumber - m_pDrone->GetRingAcquisition() <= m_RingDrawUpNumber ? true : false);
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ARing::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
				m_bIsPassed = true;
				//�G�t�F�N�g�̍Đ�
				m_pNiagaraEffectComp->Activate();
				//�����O�̓����蔻���؂�
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

