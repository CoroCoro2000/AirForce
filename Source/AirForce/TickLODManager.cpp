//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:TickLODManager.cpp
// �T�v				:�A�N�^�[��TickLOD���x�����Ǘ�����}�l�[�W���[�N���X
// �쐬��			:2022/03/09
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/03/09		:�v���C���[���痣�ꂽ�ʒu�̃A�N�^�[��FPS�������鏈���̒ǉ�
//------------------------------------------------------------------------------------------------------------------------------------------------

#include "TickLODManager.h"
#include "TickLODActor.h"
#include "PlayerDrone.h"
#include "GameUtility.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATickLODManager::ATickLODManager()
	: m_TickLODSettings()
	, m_pPlayer(nullptr)
	, m_pTickLODActors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1�b��1��Tick�X�V���s��
	PrimaryActorTick.TickInterval = 1.f;

	//�A�N�^�[��TickLOD�ݒ�
	m_TickLODSettings.Add(FTickLODSetting());
}

// Called when the game starts or when spawned
void ATickLODManager::BeginPlay()
{
	Super::BeginPlay();
	
	//LOD�A�N�^�[�̏�����
	InitializeActorArray();
}

// Called every frame
void ATickLODManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//LOD���X�V
	UpdateLOD(DeltaTime);
}

//LOD�̃A�N�^�[�z��̏�����
void ATickLODManager::InitializeActorArray()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), OutActors);

	for (AActor* pActor : OutActors)
	{
		if (pActor)
		{
			//���x�����ATickLODActor���p�����Ă���A�N�^�[��S�Ĕz��Ɋi�[
			if (ATickLODActor* pTickLODActor = Cast<ATickLODActor>(pActor))
			{
				//�A�N�^�[�ɔԍ���ݒ�
				pTickLODActor->SetNumber(m_pTickLODActors.Num());
				m_pTickLODActors.Add(pTickLODActor);
			}

			if (!m_pPlayer)
			{
				if (pActor->ActorHasTag("Player"))
				{
					m_pPlayer = Cast<APlayerDrone>(pActor);
				}
			}
		}
	}
}

//LOD�̍X�V
void ATickLODManager::UpdateLOD(const float& DeltaTime)
{
	if (!m_pPlayer) { return; }

	FVector PlayerLocation = m_pPlayer->GetActorLocation();
	const float FPS = 1.f / GetWorld()->GetDeltaSeconds();

	for (ATickLODActor* pTickLODActor : m_pTickLODActors)
	{
		if (pTickLODActor)
		{
			//�v���C���[�ƃA�N�^�[�̋���
			const float Distance = FVector::Dist(PlayerLocation, pTickLODActor->GetActorLocation());

			//LOD�ݒ�̋������������Ɋm�F���A�A�N�^�[�̈ʒu�ɂ�����LOD�ݒ�����蓖�Ă�
			for (const FTickLODSetting& TickLODSetting : m_TickLODSettings)
			{
				if (TickLODSetting.Distance <= Distance)
				{
					pTickLODActor->SetTickFPS(CGameUtility::SetDecimalTruncation(FPS * TickLODSetting.FrameRate,3));
					
					break;
				}
			}
		}
	}
}

//�z�u���Ɏ��s�����֐�
void ATickLODManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (m_TickLODSettings.Num() <= 1) { return; }

	//LOD�ݒ�̔z����������������Ƀ\�[�g����
	int32 i = 0, j = 0;
	int32 Max = m_TickLODSettings.Num();
	FTickLODSetting tmpTickLODSetting;
	for (i = 0; i < Max; ++i)
	{
		for (j = i + 1; j < Max; ++j)
		{
			if (m_TickLODSettings.IsValidIndex(i) && m_TickLODSettings.IsValidIndex(j))
			{
				//�������r���A���ёւ���
				if (m_TickLODSettings[i].Distance < m_TickLODSettings[j].Distance)
				{
					tmpTickLODSetting = m_TickLODSettings[i];
					m_TickLODSettings[i] = m_TickLODSettings[j];
					m_TickLODSettings[j] = tmpTickLODSetting;
				}
			}
		}
	}
}

