//--------------------------------------------------------------------------------------------
// �t�@�C����		:AlignedProceduralMesh.cpp
// �T�v				:�K���I�ɕ��ԃ��b�V����������������A�N�^�[�N���X
// �쐬��			:2021/08/07
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#include "AlignedProceduralMesh.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameUtility.h"

// Sets default values
AAlignedProceduralMesh::AAlignedProceduralMesh()
	: m_pMeshes(NULL)
	, m_ArrangementType(EARRANGEMENT::LINEAR)
	, m_MeshCount(1)
	, m_Distance(10.f)
	, m_MeshRelativeRotation(FRotator::ZeroRotator)
	, m_RandomizeStatus()
	, m_GridStatus()
	, m_bFix(false)
{
	//�����̓G�f�B�^��ł������s���Ȃ��ׁATick�͖����ɂ���
	PrimaryActorTick.bCanEverTick = false;

	//���b�V���C���X�^���X����
	m_pMeshes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Meshes"));
	if (m_pMeshes)
	{
		RootComponent = m_pMeshes;
		m_pMeshes->SetMobility(EComponentMobility::Static);
	}
}

// Called when the game starts or when spawned
void AAlignedProceduralMesh::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAlignedProceduralMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//������ɐ������鏈��
void AAlignedProceduralMesh::CreateLinear()
{
	//�Œ肳��Ă��Ȃ���Ύw�肵����������������
	if (!m_bFix)
	{
		//�g�����X�t�H�[���̃��Z�b�g
		m_TempInstanceTransform.Empty();

		for (int index = 0; index < m_MeshCount; ++index)
		{
			FRotator initRotation = m_MeshRelativeRotation;
			FVector initLocation = FVector::ZeroVector;
			initLocation.Y = (float)index * m_Distance;
			float initScale = 1.f;

			//���b�V���Ԃ̋����������_�������邩�ǂ���
			if (m_RandomizeStatus.bRandomizeDistance)
			{
				float RandomDistance = FMath::FRandRange(m_RandomizeStatus.RandomDistanceMin, m_RandomizeStatus.RandomDistanceMax);
				initLocation.Y *= CGameUtility::SetDecimalTruncation(RandomDistance, 3);
			}
			//���b�V���̃X�P�[���������_�������邩�ǂ���
			if (m_RandomizeStatus.bRandomizeScale)
			{
				float RandomScale = FMath::FRandRange(m_RandomizeStatus.RandomScaleMin, m_RandomizeStatus.RandomScaleMax);
				initScale *= CGameUtility::SetDecimalTruncation(RandomScale, 3);
			}
			//���b�V���̉�]�������_�������邩�ǂ���
			if (m_RandomizeStatus.bRandomizeRotaion)
			{
				float RandomRotation = FMath::FRandRange(-m_RandomizeStatus.RandomRotaionYawAngle, m_RandomizeStatus.RandomRotaionYawAngle);
				initRotation.Yaw += CGameUtility::SetDecimalTruncation(RandomRotation, 3);
			}

			//���b�V���̃g�����X�t�H�[����ݒ�
			FTransform initTransform = FTransform(initRotation, initLocation, FVector(initScale));

			//���b�V���̒ǉ�
			m_pMeshes->AddInstance(initTransform);
			//���b�V���̃g�����X�t�H�[������ۑ�
			m_TempInstanceTransform.Add(initTransform);
		}
	}
	//�Œ肳��Ă���Ƃ��͕ύX�O�̏�ԂŐ�������
	else
	{
		for (FTransform transform : m_TempInstanceTransform)
		{
			m_pMeshes->AddInstance(transform);
		}
	}
}

//�i�q��ɐ������鏈��
void AAlignedProceduralMesh::CreateGrid()
{
	FVector initLocation = FVector::ZeroVector;
	for (int x = 0; x < m_GridStatus.MeshCountX; ++x)
	{
		initLocation.X = (float)x * m_GridStatus.Distance.X;
		for (int y = 0; y < m_GridStatus.MeshCountY; ++y)
		{
			initLocation.Y = (float)y * m_GridStatus.Distance.Y;
			for (int z = 0; z < m_GridStatus.MeshCountZ; ++z)
			{
				initLocation.Z = (float)z * m_GridStatus.Distance.Z;

				FTransform initTransform = FTransform(m_MeshRelativeRotation, initLocation);
				m_pMeshes->AddInstance(initTransform);
			}
		}
	}
}

//���b�V�����̍X�V
void AAlignedProceduralMesh::UpdateMesh()
{
	if (!m_pMeshes) { return; }
	if (!m_pMeshes->GetStaticMesh()) { return; }
	//�X�V�����O�̃��b�V���������Z�b�g
	if ((int)m_pMeshes->GetInstanceCount() > 0) { m_pMeshes->ClearInstances(); }

	switch (m_ArrangementType)
	{
	case EARRANGEMENT::LINEAR:
		CreateLinear();
		break;
	case EARRANGEMENT::GRID:
		CreateGrid();
		break;
	default:
		break;
	}

#ifdef DEBUG_INSTANCECOUNT
	UE_LOG(LogTemp, Warning, TEXT("MeshInctanceCount:%i"), m_pMeshes->GetInstanceCount());
#endif // DEBUG_INCTANCECOUNT
}

//�G�f�B�^��Ŕz�u���A�܂��͓����̒l���ύX���ꂽ���ɌĂяo�����֐�
void AAlignedProceduralMesh::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//���b�V���𐶐�����
	UpdateMesh();
}