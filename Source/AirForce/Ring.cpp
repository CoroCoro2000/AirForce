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
#include "NiagaraComponentPool.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//struct FFollowingDroneAndEffect----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
FFollowingDroneAndEffect::FFollowingDroneAndEffect()
	: m_pDrone(nullptr)
	, m_pNiagaraEffect(nullptr)
	, m_bIsEffectSpawned(false)
{

}
FFollowingDroneAndEffect::FFollowingDroneAndEffect(ADroneBase* pDrone, UNiagaraComponent* pNiagaraEffect)
	: m_pDrone(MakeWeakObjectPtr<ADroneBase>(pDrone))
	, m_pNiagaraEffect(MakeWeakObjectPtr<UNiagaraComponent>(pNiagaraEffect))
	, m_bIsEffectSpawned(m_pNiagaraEffect.IsValid())
{

}
//�f�X�g���N�^
FFollowingDroneAndEffect::~FFollowingDroneAndEffect()
{

}
//�G�t�F�N�g�𐶐�
void FFollowingDroneAndEffect::SpawnEffectAtLocation(const UObject* WorldContextObject, UNiagaraSystem* SystemTemplate, FVector SpawnLocation, FRotator SpawnRotation, FVector Scale, bool bAutoDestroy, bool bAutoActivate, ENCPoolMethod PoolingMethod)
{
	m_pNiagaraEffect = MakeWeakObjectPtr<UNiagaraComponent>(UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SystemTemplate, SpawnLocation, SpawnRotation, Scale, bAutoDestroy, bAutoActivate, PoolingMethod));
	m_bIsEffectSpawned = true;
}

//ARing----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//�R���X�g���N�^
ARing::ARing()
	: m_pRingMesh(NULL)
	, m_pEffect(NULL)
	, m_bIsPassed(false)
	, m_PassedTime(0.f)
	, m_ResetTime(1.f)
	, m_SineWidth(10.f)
	, m_SineScaleMin(0.8f)
	, m_SineScaleMax(1.05f)
	, m_SineCurveValue(0.f)
	, m_RingScale(1.f)
	, m_RingMaxScale(m_RingScale * 1.8f)
	, m_HSV(30.f, 40.f, 30.f)
	, m_pRingHitSE(NULL)
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

	//�ݒ肳�ꂽFPS�̊Ԋu��Tick���X�V����
	const float currentTime = GetWorld()->GetTimeSeconds();
	//�z�肳���1�t���[���ɂ����鎞��
	const float TimePerFrame = 1.f / m_TickFPS;
	//�O����s����Ă���̌o�ߎ���
	const float deltaTime = currentTime - m_LastTickTime;

	//�����\�ȃt���[���ł���΍X�V
	if (deltaTime > TimePerFrame && IsProcessableFrame(currentTime))
	{
		m_LastTickTime = currentTime;

		//�T�C���J�[�u�̒l���X�V
		UpdateSineCurve(currentTime);

		//�����O�̃T�C�Y�X�V
		UpdateScale(deltaTime);

		//�����O�̃}�e���A���X�V
		UpdateMaterial();
	}

	//�G�t�F�N�g�̍X�V
	UpdateEffect();
}

//�T�C���J�[�u�̒l���X�V
void ARing::UpdateSineCurve(const float& CurrentTime)
{
	//�T�C���g�̕���ݒ�
	const float WaveWidth = m_SineWidth * CurrentTime;
	//�T�C���g�̑傫����0����1�ɐ��K������
	m_SineCurveValue = FMath::Sin(WaveWidth) * 0.5f + 0.5f;
}

//�����O�̃T�C�Y�X�V
void ARing::UpdateScale(const float& DeltaTime)
{
	float NewScale = m_RingScale;

	//�ʉ߃t���O�������Ă���Ȃ烊���O���g�傷��
	if (m_bIsPassed)
	{
		if (m_PassedTime < m_ResetTime)
		{
			m_PassedTime += DeltaTime;
		}
		else
		{
			m_bIsPassed = false;

			//�}�e���A���̉�]���x��������
			float ScrollSpeed = 0.5f;
			m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);
		}

		NewScale = FMath::InterpExpoOut(m_RingScale, m_RingMaxScale, FMath::Clamp(m_PassedTime / m_ResetTime, 0.f, 1.f));
	}
	//�ʉ߃t���O�������Ă��Ȃ��Ƃ���Sin�g�̊Ԋu�Ŋg��k�����J��Ԃ�
	else
	{
		//�T�C���g�̒l�Ń����O�̑傫����ς���
		float ScaleMultiplier = FMath::Lerp(m_SineScaleMin, m_SineScaleMax, m_SineCurveValue);
		NewScale *= ScaleMultiplier;
	}
	float Speed = FMath::Clamp(m_bIsPassed ? DeltaTime * 10.f : DeltaTime * 8.f, 0.f, 1.f);
	SetActorScale3D(FMath::Lerp(GetActorScale3D(), FVector(NewScale), Speed));
}

//�����O�̃}�e���A���X�V
void ARing::UpdateMaterial()
{
	if (!m_pRingMesh) { return; }

	if (m_bIsPassed)
	{
		//�}�e���A���̉�]���x���グ��
		float ScrollSpeed = 1.5f;
		m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("ColorScrollSpeed"), ScrollSpeed);
	}

	m_HSV.R = FMath::Lerp(50.f, 30.f, m_SineCurveValue);
	m_HSV.B = FMath::Lerp(30.f, 50.f, m_SineCurveValue);
	m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("BlendColor"), FVector(m_HSV.HSVToLinearRGB()));
}

//�����O�̃G�t�F�N�g�X�V
void ARing::UpdateEffect()
{
	if (m_pFollowingDroneAndEffect.Num() <= 0) { return; }

	//�G�t�F�N�g�������Ă�����z�񂩂�폜���邽�߁A�t����for������
	for (int32 index = m_pFollowingDroneAndEffect.Num() - 1; index >= 0; --index)
	{
		if (m_pFollowingDroneAndEffect.IsValidIndex(index))
		{
			FFollowingDroneAndEffect& DroneAndEffect = m_pFollowingDroneAndEffect[index];

			//�����ς݂̃G�t�F�N�g�������ɂȂ��Ă�����
			if (DroneAndEffect.IsEffectSpawned() && (!DroneAndEffect.m_pNiagaraEffect.IsValid()))
			{
				//�z�񂩂��菜��
				m_pFollowingDroneAndEffect.RemoveAt(index);
			}
			//�G�t�F�N�g���X�|�[������Ă��Ȃ��ꍇ�̓����O�̊g�傪�I����Ă�����X�|�[��������
			else if (!DroneAndEffect.IsEffectSpawned())
			{
				FVector scaleSubtract = FVector(m_RingMaxScale) - GetActorScale3D();
				if (scaleSubtract.IsNearlyZero(0.3f))
				{
					if (m_pEffect)
					{
						DroneAndEffect.SpawnEffectAtLocation(this, m_pEffect, GetActorLocation(), GetActorRotation(), GetActorScale3D());
						if (DroneAndEffect.m_pNiagaraEffect.IsValid())
						{
							int32 randomColor = FMath::RandRange(0, 2);
							FLinearColor particleColor = (randomColor == 0) ? FLinearColor::Red : (randomColor == 1) ? FLinearColor::Green : FLinearColor::Blue;
							particleColor *= 30.f;
							DroneAndEffect.m_pNiagaraEffect->SetVariableLinearColor(TEXT("User.Color"), particleColor);
						}
					}
				}
			}
		}
	}
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ARing::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//�����O�̍X�V�t���[����15FPS��菭�Ȃ��ꍇ�̓I�[�o�[���b�v�������s��Ȃ�
	if (m_TickFPS < 15.f) { return; }

	if (OtherActor && OtherActor != this)
	{
		//�^�O��Player�������ꍇ
		if (OtherActor->ActorHasTag(TEXT("Drone")))
		{
			if (ADroneBase* pDrone = Cast<ADroneBase>(OtherActor))
			{
				//�ʉߍς݂̃h���[�����m�F
				bool isPassed = false;
				for (const FFollowingDroneAndEffect& FollowingDroneAndEffect : m_pFollowingDroneAndEffect)
				{
					if (pDrone == FollowingDroneAndEffect.m_pDrone)
					{
						isPassed = true;
						break;
					}
				}

				if (!isPassed)
				{
					m_bIsPassed = true;
					m_PassedTime = 0.f;

					//�q�b�g�����h���[����z��Ɋi�[����
					m_pFollowingDroneAndEffect.Add(FFollowingDroneAndEffect(pDrone));

					//�q�b�g�����h���[�������g�̑��삷��h���[���̎��̂�SE���Đ�
					if (pDrone->GetisControl() && pDrone->IsPlayerControlled())
					{
						UGameplayStatics::PlaySound2D(GetWorld(), m_pRingHitSE);
					}
				}
			}
		}
	}
}