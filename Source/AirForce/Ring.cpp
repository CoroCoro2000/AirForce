//------------------------------------------------------------------------
// �t�@�C����		:Ring.cpp
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------


#include "Ring.h"
#include "Components/StaticMeshComponent.h"
#include "ColorLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DroneBase.h"

//�R���X�g���N�^
ARing::ARing()
	: m_pColorLightComp(NULL)
	, m_pRingMesh(NULL)
	, m_RingNumber(0)
	, m_RingDrawUpNumber(0)
	, m_pNiagaraEffectComp(NULL)
	, m_bIsPassed(false)
	, m_MakeInvisibleCnt(0.f)
	, m_MakeInvisibleTime(1.8f)
{
	//�J���[�R���|�[�l���g����
	m_pColorLightComp = CreateDefaultSubobject<UColorLightComponent>(TEXT("m_pColorLightComp"));

	//���b�V������
	m_pRingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRingMesh"));
	if (m_pRingMesh)
	{
		RootComponent = m_pRingMesh;
	}

	//�i�C�A�K���̃G�t�F�N�g�R���|�[�l���g����
	m_pNiagaraEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("m_pNiagaraEffectComp"));

	//���t���[��Tick���Ăяo�����ǂ����̃t���O
	PrimaryActorTick.bCanEverTick = true;
}

//�Q�[���J�n���܂��͂��̃N���X�̃I�u�W�F�N�g���X�|�[�����ꂽ���P�x�����Ăяo�����֐�
void ARing::BeginPlay()
{
	Super::BeginPlay();

	//�I�[�o�[���b�v�J�n���ɌĂ΂��C�x���g�֐���o�^
	m_pRingMesh->OnComponentBeginOverlap.AddDynamic(this, &ARing::OnOverlapBegin);

	//	�h���[���̌���
	TSubclassOf<AActor> findClass;
	findClass = AActor::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), findClass, actors);

	//�����͈͂�Actor�������
	if ((int)actors.Num() > 0)
	{
		for (int i = 0; i < (int)actors.Num(); i++)
		{
			//���������A�N�^�[
			AActor* pActor = Cast<AActor>(actors[i]);

			//	�Q�[���}�l�W���[���擾
			if (pActor->ActorHasTag("Drone"))
			{
				m_Drone = Cast<ADroneBase>(pActor);
				break;
			}
		}
	}

	m_pColorLightComp->InitializeMaterialParameter(m_pRingMesh, true);
	m_pColorLightComp->Activate(true);
}

//���t���[���Ăяo�����֐�
void ARing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pColorLightComp && m_pRingMesh)
	{
		//�����O�̐F���X�V
		m_pRingMesh->SetVectorParameterValueOnMaterials(TEXT("EmissiveColor"), m_pColorLightComp->GetVectorColor());
	}
	m_pRingMesh->SetGenerateOverlapEvents(isDraw());
	m_pRingMesh->SetHiddenInGame(!isDraw());
	//�����O���ʉ߂��ꂽ�Ȃ�
	if (m_bIsPassed)
	{
		//�G�t�F�N�g�̐F�����b�V���Ɠ����F�ɂ���
		m_pNiagaraEffectComp->SetNiagaraVariableLinearColor(TEXT("User.MeshColor"), m_pColorLightComp->GetVectorColor());
		if (m_MakeInvisibleTime > m_MakeInvisibleCnt)
		{
			//���S�Ɍ����Ȃ��Ȃ�܂Ŏ��Ԃ��J�E���g
			m_MakeInvisibleCnt += DeltaTime;
			if (m_pRingMesh && m_pNiagaraEffectComp)
			{
				//���b�V���ƃG�t�F�N�g�̕s�����x�������Ă���
				const float CountRate = FMath::Clamp(m_MakeInvisibleCnt / m_MakeInvisibleTime, 0.f, 1.f);
				const float Opacity = FMath::Lerp(1.f, 0.f, CountRate);
				m_pRingMesh->SetScalarParameterValueOnMaterials(TEXT("Opacity"), Opacity);
				m_pNiagaraEffectComp->SetNiagaraVariableFloat(TEXT("User.MeshOpacity"), Opacity);

				if (Opacity <= 0.f)
				{
					//�����O�������Ȃ��Ȃ�����j������
					this->Destroy();
				}
			}
		}
	}
}

bool ARing::isDraw()
{
	if (!m_Drone)
		return false;

	return  (m_RingNumber - m_Drone->GetRingAcquisition() <= m_RingDrawUpNumber ? true : false);
}

//�I�[�o�[���b�v�J�n���ɌĂ΂�鏈��
void ARing::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//�^�O��Player�������ꍇ
	if (OtherActor->ActorHasTag(TEXT("Drone")))
	{
		//���̃����O���܂��ʉ߂���Ă��Ȃ��ꍇ
		if (!m_bIsPassed)
		{
			if (m_pNiagaraEffectComp)
			{
				//�ʉ߂��ꂽ��ԂɕύX
				m_bIsPassed = true;
				//�G�t�F�N�g�̍Đ�
				m_pNiagaraEffectComp->Activate();
			}
		}
	}
}

