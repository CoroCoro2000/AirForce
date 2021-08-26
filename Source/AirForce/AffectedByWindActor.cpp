//--------------------------------------------------------------------------------------------
// �t�@�C����		:AAffectedByWindActor.cpp
// �T�v				:�h���[���̕��̉e�����󂯂�A�N�^�[�N���X
// �쐬��			:2021/08/26
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#include "AffectedByWindActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAffectedByWindActor::AAffectedByWindActor()
	: m_pMesh(NULL)
	, m_WindSpeed(1.f)
	, m_TempWindSpeed(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//���b�V������
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//���b�V���̐����ɐ���������A�f�t�H���g�̕��̋�����ݒ�
	if (m_pMesh)
	{
		RootComponent = m_pMesh;
		m_pMesh->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), m_WindSpeed);
	}
}

// Called when the game starts or when spawned
void AAffectedByWindActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAffectedByWindActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//�󂯂镗�̋�����ݒ肷��֐�
void AAffectedByWindActor::SetWindSpeed(const float& _windSpeed)
{
	m_WindSpeed = _windSpeed;

	//���̋����̍X�V����
	if (m_pMesh)
	{
		m_pMesh->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), m_WindSpeed);
		m_TempWindSpeed = m_WindSpeed;
	}

	UE_LOG(LogTemp, Warning, TEXT("m_WindSpeed    :%f"), m_WindSpeed);
	UE_LOG(LogTemp, Warning, TEXT("m_TempWindSpeed:%f"), m_TempWindSpeed);
}