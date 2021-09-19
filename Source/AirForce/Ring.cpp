//------------------------------------------------------------------------
// �t�@�C����		:Ring.cpp
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "GameUtility.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "UObject/ConstructorHelpers.h"

//�R���X�g���N�^
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pNiagaraEffectComp(NULL)
	, m_RingScale(1.f)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.5f)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_PassedSceleMax(3.f)
	, m_EaseExp(2.f)
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

	//�����O�̑傫����ێ�
	m_RingScale = GetActorScale().GetAbsMax();
	SetActorScale3D(FVector(m_RingScale));
}

//���t���[���Ăяo�����֐�
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�����O�̃T�C�Y�X�V
	UpdateScale(DeltaTime);
}

//�����O�̃T�C�Y�X�V
void ARing::UpdateScale(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	float ScaleMultiplier = 0.f;
	//�����O���ʉ߂���Ă��Ȃ��Ԃ͈��̎����ő傫����ς���
	if (!m_bIsPassed)
	{
		//�T�C���g�̕���ݒ�
		const float WaveWidth = m_SineWidth * GetWorld()->GetTimeSeconds();
		//�T�C���g�̑傫����0����1�ɐ��K������
		const float SinWave = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
		//�T�C���g�̒l�Ń����O�̑傫����ς���
		ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, SinWave);
	}
	//�����O���ʉ߂��ꂽ��傫������
	else
	{
		//�w��̎��Ԃ��z����܂Ŏ��Ԃ��v��
		if (m_MakeInvisibleCnt < m_MakeInvisibleTime)
		{
			m_MakeInvisibleCnt += DeltaTime;
		}
		//�o�ߗ��ő傫����ς���
		const float elapsedRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
		m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), elapsedRate);
		ScaleMultiplier = FMath::InterpEaseOut(m_RingScale, m_PassedSceleMax, elapsedRate, 2.f);
	}
	//�V�����X�P�[����K�p
	SetActorScale3D(FVector(m_RingScale * ScaleMultiplier));
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
				m_bIsPassed = true;
				//�G�t�F�N�g�̍Đ�
				m_pNiagaraEffectComp->Activate();
				//�����O�̓����蔻���؂�
				m_pRingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}