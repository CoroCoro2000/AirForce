//-----------------------------------------------------------------------------------------------------------------
// �t�@�C����		:RingManager.cpp
// �T�v				:�����O���Ǘ�����A�N�^�[�N���X
// �쐬��			:2021/08/20
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/08/20	�r������		:	�Q�[���J�n���ɂ��ׂẴ����O�ɔԍ������蓖�Ă鏈�����쐬
//						:2021/08/20	�r������		:	�����O��������ꂽ��z�񂩂�폜���鏈�����쐬
//-----------------------------------------------------------------------------------------------------------------

//�C���N���[�h
#include "RingManager.h"
#include "GameManager.h"
#include "Ring.h"
#include "DroneBase.h"
#include "GameUtility.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

//�R���X�g���N�^
ARingManager::ARingManager()
	: m_pDrone(NULL)
	, m_pGameManager(NULL)
	, m_ColorState(ECOLOR_STATE::RED)
	, m_RingColor(FLinearColor::Transparent)
	, m_FresnelColor(FLinearColor::Transparent)
	, m_TargetColor(FLinearColor::Transparent)
	, m_ColorTransitionSpeed(3.f)
	, m_DelayTempo(2)
{
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n����1�x�����Ă΂�鏈��
void ARingManager::BeginPlay()
{
	Super::BeginPlay();
	
	//�h���[���ƃQ�[���}�l�[�W���[�������A�ێ�����
	TSubclassOf<AActor> findClass = AActor::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), findClass, actors);

	if ((int)actors.Num() > 0)
	{
		for (AActor* pActor : actors)
		{
			if (pActor->ActorHasTag(TEXT("GameManager")))
			{
				m_pGameManager = Cast<AGameManager>(pActor);
			}
			if (pActor->ActorHasTag(TEXT("Drone")))
			{
				m_pDrone = Cast<ADroneBase>(pActor);
			}
		}
	}
}

//���t���[���Ă΂�鏈��
void ARingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//�J���[�X�e�[�g�X�V
	UpdateColorState();

	//�J���[�X�V
	UpdateColor(DeltaTime);

	/*if (m_pGameManager && m_pChildRings[0])
	{
		m_pGameManager->SetIsGoal(m_pChildRings[0]->GetIsPassed());
	}*/
	if (m_pChildRings[0]->GetIsPassed())
	{
		m_pGameManager->SetIsGoal(true);
	}
}

//�J���[�X�e�[�g�X�V
void ARingManager::UpdateColorState()
{
	switch (m_ColorState)
	{
	case ECOLOR_STATE::RED:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Red - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::ORANGE : ECOLOR_STATE::RED);
		m_TargetColor = FLinearColor::Red;
		break;
	}
	case ECOLOR_STATE::ORANGE:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(LINEARCOLOR_ORANGE - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::YELLOW : ECOLOR_STATE::ORANGE);
		m_TargetColor = LINEARCOLOR_ORANGE;
		break;
	}
	case ECOLOR_STATE::YELLOW:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Yellow - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::GREEN : ECOLOR_STATE::YELLOW);
		m_TargetColor = FLinearColor::Yellow;
		break;
	}
	case ECOLOR_STATE::GREEN:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Green - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::BLUE : ECOLOR_STATE::GREEN);
		m_TargetColor = FLinearColor::Green;
		break;
	}
	case ECOLOR_STATE::BLUE:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(FLinearColor::Blue - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::INDIGO : ECOLOR_STATE::BLUE);
		m_TargetColor = FLinearColor::Blue;
		break;
	}
	case ECOLOR_STATE::INDIGO:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(LINEARCOLOR_INDIGO - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::PURPLE : ECOLOR_STATE::INDIGO);
		m_TargetColor = LINEARCOLOR_INDIGO;
		break;
	}
	case ECOLOR_STATE::PURPLE:
	{
		float proximityRate = CGameUtility::SetDecimalTruncation(FVector(LINEARCOLOR_PURPLE - m_RingColor).Size(), 3);
		m_ColorState = (proximityRate == 0.f ? ECOLOR_STATE::RED : ECOLOR_STATE::PURPLE);
		m_TargetColor = LINEARCOLOR_PURPLE;
		break;
	}
	default:
		break;
	}
}

//�����O�̐F�X�V
void ARingManager::UpdateColor(const float& DeltaTime)
{
	if ((int)m_pChildRings.Num() <= 0) { return; }

	//���݂̐F����^�[�Q�b�g�Ɍ����ĐF��ς��Ă���
	m_RingColor = FLinearColor::LerpUsingHSV(m_RingColor, m_TargetColor, DeltaTime * m_ColorTransitionSpeed);
	//�t���l���̐F��3�e���|��̐F�ɂ���
	int32 FresnelState = (m_ColorState + m_DelayTempo) % ECOLOR_STATE::NUM;
	m_FresnelColor = FLinearColor::LerpUsingHSV(m_FresnelColor, GetTargetColor(FresnelState), DeltaTime * m_ColorTransitionSpeed);
	//�}�l�[�W���[���Ǘ����Ă��邷�ׂẴ����O�̃}�e���A���Ɍ��݂̐F��K�p����
	for (ARing* pRing : m_pChildRings)
	{
		if (pRing)
		{
			if (pRing->GetMesh())
			{
				pRing->GetMesh()->SetVectorParameterValueOnMaterials(TEXT("RingColor"), FVector(m_RingColor));
				pRing->GetMesh()->SetVectorParameterValueOnMaterials(TEXT("FresnelColor"), FVector(m_FresnelColor));
			}

			//�����O���ʉ߂���Ă�����G�t�F�N�g�̐F���ω�������
			if (pRing->GetIsPassed())
			{
				if (pRing->GetEffectComponent())
				{
					pRing->GetEffectComponent()->SetVariableLinearColor(TEXT("User.Color"), m_RingColor);
				}
			}
		}
	}
}

//�X�e�[�g����J���[�^�[�Q�b�g���擾
FLinearColor ARingManager::GetTargetColor(const int32& _colorIndex)
{
	FLinearColor TargetColor;
	switch (_colorIndex)
	{
	case ECOLOR_STATE::RED:
		TargetColor = FLinearColor::Red;
		break;
	case ECOLOR_STATE::ORANGE:
		TargetColor = LINEARCOLOR_ORANGE;
		break;
	case ECOLOR_STATE::YELLOW:
		TargetColor = FLinearColor::Yellow;
		break;
	case ECOLOR_STATE::GREEN:
		TargetColor = FLinearColor::Green;
		break;
	case ECOLOR_STATE::BLUE:
		m_TargetColor = FLinearColor::Blue;
		break;
	case ECOLOR_STATE::INDIGO:
		TargetColor = LINEARCOLOR_INDIGO;
		break;
	case ECOLOR_STATE::PURPLE:
		TargetColor = LINEARCOLOR_PURPLE;
		break;
	default:
		break;
	}
	return TargetColor;
}