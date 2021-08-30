//--------------------------------------------------------------------------------------------
// �t�@�C����		:ComponentOnSplineActor.cpp
// �T�v				:�X�v���C����ɃR���|�[�l���g��������������A�N�^�[�N���X
// �쐬��			:2021/08/30
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------
//�C���N���[�h
#include "ComponentOnSplineActor.h"
#include "Components/SplineComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AComponentOnSplineActor::AComponentOnSplineActor()
	: m_pSpline(NULL)
	, m_ComponentType(EComponent::Niagara)
	, m_pNiagaraEffect(NULL)
	, m_ComponentCount(1)
	, m_ComponentRelativeRotation(FRotator::ZeroRotator)
{
	//�����̓G�f�B�^��ł������s���Ȃ��ׁATick�͖����ɂ���
	PrimaryActorTick.bCanEverTick = false;

	//�X�v���C������
	m_pSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	if (m_pSpline)
	{
		RootComponent = m_pSpline;
	}
}

// Called when the game starts or when spawned
void AComponentOnSplineActor::BeginPlay()
{
	Super::BeginPlay();

	if (m_ComponentType == EComponent::Niagara)
	{
		//�X�v���C�����Niagara�G�t�F�N�g�𐶐�
		SpawnNiagaraOnSpline();
	}
}

// Called every frame
void AComponentOnSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�X�v���C����ɐ�������Niagara�G�t�F�N�g���̍X�V
void AComponentOnSplineActor::SpawnNiagaraOnSpline()
{
	if (!m_pSpline) { return; }
	if (!m_pNiagaraEffect) { return; }
	if (m_ComponentCount <= 0) { return; }

	//�X�v���C���̒������擾
	const float splineLength = m_pSpline->GetSplineLength();

	for (int index = 0; index < m_ComponentCount; ++index)
	{
		//�X�v���C����̂ǂ̈ʒu�ɃG�t�F�N�g�𐶐����邩���߂�
		const float rate = (float)index / (float)m_ComponentCount;
		const float distance = rate * splineLength;
		const FVector initLocation = m_pSpline->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
		FRotator initRotation = m_pSpline->GetRotationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World) + m_ComponentRelativeRotation;

		//�G�t�F�N�g�R���|�[�l���g����
		m_pNiagaraEffectComponents.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, m_pNiagaraEffect, initLocation, initRotation));
	}
}