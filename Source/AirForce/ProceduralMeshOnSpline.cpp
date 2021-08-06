//--------------------------------------------------------------------------------------------
// �t�@�C����		:ProceduralMeshOnSpline.cpp
// �T�v				:�X�v���C����Ɏw�萔�̃��b�V����������������A�N�^�[�N���X
// �쐬��			:2021/08/06
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------
//�C���N���[�h
#include "ProceduralMeshOnSpline.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AProceduralMeshOnSpline::AProceduralMeshOnSpline()
	: m_pSpline(NULL)
	, m_pMeshes(NULL)
	, m_MeshCount(1)
	, m_MeshRelativeRotation(FRotator::ZeroRotator)
	, m_bLockRotationPitch(false)
	, m_bLockRotationYaw(false)
	, m_bLockRotationRoll(false)
{
	//�����̓G�f�B�^��ł������s����Ȃ��ׁATick�͐؂�
	PrimaryActorTick.bCanEverTick = false;

	//�X�v���C������
	m_pSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	if (m_pSpline)
	{
		RootComponent = m_pSpline;
	}

	//���b�V���R���|�[�l���g����
	m_pMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Meshes");
	if (m_pMeshes && m_pSpline)
	{
		m_pMeshes->SetupAttachment(m_pSpline);
	}
}

// Called when the game starts or when spawned
void AProceduralMeshOnSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralMeshOnSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�X�v���C����Ƀ��b�V���𐶐����鏈��
void AProceduralMeshOnSpline::CreateMeshOnSpline()
{
	//NULL�`�F�b�N
	if (!m_pSpline || !m_pMeshes) { return; }

	if ((int)m_pMeshes->GetInstanceCount() > 0)
	{
		//�ύX�O�ɕ`�悳��Ă������ׂẴC���X�^���X���폜
		m_pMeshes->ClearInstances();
	}

	//�X�v���C���̒������擾
	const float splineLength = m_pSpline->GetSplineLength();

	//�z�u���郁�b�V���̐����������������s��
	for (int index = 0; index < m_MeshCount; ++index)
	{
		//�X�v���C����̂ǂ̈ʒu�ɂɃ��b�V���𐶐����邩���߂�
		const float rate = (float)index / (float)m_MeshCount;
		const float distance = rate * splineLength;
		const FVector MeshLocation = m_pSpline->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);
		FRotator MeshRotation = m_pSpline->GetRotationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::Local);

		//��]�������b�N����Ă���ꍇ�A�X�v���C���ɉ����ĉ�]���Ȃ��悤�ɂ���
		MeshRotation.Pitch *= (m_bLockRotationPitch) ? 0.f : 1.f;
		MeshRotation.Yaw *= (m_bLockRotationYaw) ? 0.f : 1.f;
		MeshRotation.Roll *= (m_bLockRotationRoll) ? 0.f : 1.f;

		//�z�u���郁�b�V���̃g�����X�t�H�[����ݒ�
		const FTransform MeshTransform = FTransform(MeshRotation + m_MeshRelativeRotation, MeshLocation);

		//���b�V���C���X�^���X�̒ǉ��A�z�u
		m_pMeshes->AddInstance(MeshTransform);

#ifdef DEBUG_TRANSFORM
		UE_LOG(LogTemp, Warning, TEXT("MeshTransform:%s"), *MeshTransform.ToString());
#endif // DEBUG_TRANSFORM
	}
#ifdef DEBUG_INSTANCECOUNT
	UE_LOG(LogTemp, Warning, TEXT("MeshInctanceCount:%i"), m_pMeshes->GetInstanceCount());
#endif // DEBUG_INCTANCECOUNT
}

//�G�f�B�^��Ŕz�u���A�܂��͓����̒l���ύX���ꂽ���ɌĂяo�����֐�
void AProceduralMeshOnSpline::OnConstruction(const FTransform& Transform)
{
	//�X�v���C����Ƀ��b�V���𐶐�
	CreateMeshOnSpline();
}