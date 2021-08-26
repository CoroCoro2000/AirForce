//--------------------------------------------------------------------------------------------
// �t�@�C����		:AlignedProceduralMesh.cpp
// �T�v				:�K���I�ɕ��ԃ��b�V����������������A�N�^�[�N���X
// �쐬��			:2021/08/07
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#include "AlignedProceduralMesh.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

// Sets default values
AAlignedProceduralMesh::AAlignedProceduralMesh()
	: m_pMeshes(NULL)
	, m_ArrangementType(EARRANGEMENT::LINEAR)
	, m_MeshCount(1)
	, m_Distance(10.f)
	, m_MeshRelativeRotation(FRotator::ZeroRotator)
	, m_RandomizeStatus()
	, m_SpiralStatus()
	, m_GridStatus()
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
	//�w�肵�����������b�V���𐶐�����
	for (int index = 0; index < m_MeshCount; ++index)
	{
		FRotator initRotation = m_MeshRelativeRotation;
		FVector initLocation = FVector::ZeroVector;
		initLocation.Y = (float)index * m_Distance;
		float initScale = 1.f;

		//���b�V���Ԃ̋����������_�������邩�ǂ���
		if (m_RandomizeStatus.bRandomizeDistance)
		{
			initLocation.Y *= FMath::FRandRange(m_RandomizeStatus.RandomDistanceMin, m_RandomizeStatus.RandomDistanceMax);
		}
		//���b�V���̃X�P�[���������_�������邩�ǂ���
		if (m_RandomizeStatus.bRandomizeScale)
		{
			initScale *= FMath::FRandRange(m_RandomizeStatus.RandomScaleMin, m_RandomizeStatus.RandomScaleMax);
		}
		//���b�V���̉�]�������_�������邩�ǂ���
		if (m_RandomizeStatus.bRandomizeRotaion)
		{
			initRotation.Yaw += FMath::FRandRange(-m_RandomizeStatus.RandomRotaionYawAngle, m_RandomizeStatus.RandomRotaionYawAngle);
		}

		//���b�V���̃g�����X�t�H�[����ݒ�
		const FTransform initTransform = FTransform(initRotation, initLocation, FVector(initScale));

		//���b�V���C���X�^���X��ǉ�
		m_pMeshes->AddInstance(initTransform);
	}
}

//�Ȑ���ɐ������鏈��
void AAlignedProceduralMesh::CreateCurved()
{

}

//�~��ɐ������鏈��
void AAlignedProceduralMesh::CreateCircular()
{

}

//������ɐ������鏈��
void AAlignedProceduralMesh::CreateSpiral()
{
	for (int index = 0; index < m_MeshCount; ++index)
	{
		FVector initLocation = (float)index * m_SpiralStatus.Direction;
		FRotator initRotation = (float)index * m_SpiralStatus.Rotation;

		FTransform initTransform = FTransform(initRotation.RotateVector(initLocation));

		m_pMeshes->AddInstance(initTransform);
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
	if (m_MeshCount == 0) { return; }
	if (!m_pMeshes) { return; }
	if (!m_pMeshes->GetStaticMesh()) { return; }
	//�X�V�����O�̃��b�V���������Z�b�g
	if ((int)m_pMeshes->GetInstanceCount() > 0) { m_pMeshes->ClearInstances(); }

	switch (m_ArrangementType)
	{
	case EARRANGEMENT::LINEAR:
		CreateLinear();
		break;
	case EARRANGEMENT::CURVE:
		CreateCurved();
		break;
	case EARRANGEMENT::CIRCLE:
		CreateCircular();
		break;
	case EARRANGEMENT::SPIRAL:
		CreateSpiral();
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
	//���b�V���𐶐�����
	UpdateMesh();
}