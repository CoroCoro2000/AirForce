//------------------------------------------------------------------------
// �t�@�C����		:Ring.cpp
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//�R���X�g���N�^
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pFollowingEffectDronePairs()
	, m_pEffect(NULL)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_SineCurveValue(0.f)
	, m_RingScale(1.f)
	, m_HSV(30.f, 40.f, 30.f)
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
		m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnComponentBeginOverlap);
	}

	//�����O�̏����X�P�[����ێ�
	m_RingScale = GetActorScale3D().GetMax();
}

//���t���[���Ăяo�����֐�
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�T�C���J�[�u�̒l���X�V
	UpdateSineCurve(DeltaTime);

	//�����O�̃T�C�Y�X�V
	UpdateScale(DeltaTime);

	//�����O�̃}�e���A���X�V
	UpdateMaterial(DeltaTime);

	//�G�t�F�N�g�̍X�V
	UpdateEffect(DeltaTime);
}

//�T�C���J�[�u�̒l���X�V
void ARing::UpdateSineCurve(const float& DeltaTime)
{
	if (const UWorld* pWorld = GetWorld())
	{
		//�T�C���g�̕���ݒ�
		const float WaveWidth = m_SineWidth * pWorld->GetTimeSeconds();
		//�T�C���g�̑傫����0����1�ɐ��K������
		m_SineCurveValue = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
	}
}

//�����O�̃T�C�Y�X�V
void ARing::UpdateScale(const float& DeltaTime)
{
	//�ʉ߃t���O�������Ă���Ȃ烊���O���g�傷��
	if (m_bIsPassed)
	{

	}
	//�ʉ߃t���O�������Ă��Ȃ��Ƃ���Sin�g�̊Ԋu�Ŋg��k�����J��Ԃ�
	else
	{
		//�T�C���g�̒l�Ń����O�̑傫����ς���
		float ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, m_SineCurveValue);
		//�V�����X�P�[����K�p
		SetActorScale3D(FVector(m_RingScale * ScaleMultiplier));
	}
}

//�����O�̃}�e���A���X�V
void ARing::UpdateMaterial(const float& DeltaTime)
{
	if (!m_pRingMesh) { return; }

	if (m_bIsPassed)
	{

	}
	//�ʉ߃t���O�������Ă��Ȃ��Ԃ̓��b�V���̊g�k�ɍ��킹�ă����O�̐F��ς���
	else
	{
		m_HSV.R = FMath::Lerp(50.f, 30.f, m_SineCurveValue);
		m_HSV.B = FMath::Lerp(30.f, 50.f, m_SineCurveValue);
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
	}
}

//�����O�̃G�t�F�N�g�X�V
void ARing::UpdateEffect(const float& DeltaTime)
{
	if (m_pFollowingEffectDronePairs.Num() <= 0) { return; }

	//�G�t�F�N�g�������Ă�����z�񂩂�폜���邽�߁A�t����for������
	for (int32 index = m_pFollowingEffectDronePairs.Num() - 1; index >= 0; --index)
	{
		if (m_pFollowingEffectDronePairs.IsValidIndex(index))
		{
			if (const FFollowingEffectDronePair* EffectDronePair = &m_pFollowingEffectDronePairs[index])
			{
				//�G�t�F�N�g���h���[���ɒǏ]������
				if (IsValid(EffectDronePair->pFollowingEffect))
				{
					if (EffectDronePair->pDrone)
					{
						FVector DroneLocation = EffectDronePair->pDrone->GetActorLocation();
						EffectDronePair->pFollowingEffect->SetVariableVec3(TEXT("User.Aim_position"), DroneLocation);
					}
				}
				//�G�t�F�N�g����������z�񂩂�폜����
				else
				{
					m_pFollowingEffectDronePairs.RemoveAt(index);
				}
			}
		}
	}
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ARing::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//�^�O��Player�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			if (m_pEffect)
			{
				//�q�b�g�����h���[���ƃG�t�F�N�g��z��Ɋi�[����
				m_pFollowingEffectDronePairs.Add(
					FFollowingEffectDronePair(
						Cast<ADroneBase>(OtherActor),
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, m_pEffect, GetActorLocation())));

				//�}�e���A���̉�]���x���グ��
				float ScrollSpeed = 1.f;
				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);

				//SE�̍Đ�
				UGameplayStatics::PlaySound2D(GetWorld(), m_RingHitSE);
			}
		}
	}
}