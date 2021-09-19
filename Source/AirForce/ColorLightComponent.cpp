//------------------------------------------------------------------------
// �t�@�C����		:ColorLightComponent.cpp
// �T�v				:�}�e���A���̃J���[�����Ǘ�����R���|�[�l���g
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------


#include "ColorLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/Color.h"

// Sets default values for this component's properties
UColorLightComponent::UColorLightComponent()
	//: m_ColorState(ECOLOR_STATE::RED)
	: m_ColorChangeCnt(0.f)
	, m_ColorChangeTime(1.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UColorLightComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

//���t���[�����������֐�
void UColorLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//�F��ω�������J�E���g�̍X�V����
	//UpdateColorChangeCnt(DeltaTime);

	////�F���̍X�V
	//UpdateColor(DeltaTime);
}

//�F���̍X�V����
//void UColorLightComponent::UpdateColor(const float& DeltaTime)
//{
//	switch (m_ColorState.COLOR_STATE)
//	{
//	case ECOLOR_STATE::RED:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(LINEAR_COLOR_PURPLE, FLinearColor::Red, GetColorChangeRate()));
//		break;
//	case ECOLOR_STATE::ORANGE:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(FLinearColor::Red, LINEAR_COLOR_ORANGE, GetColorChangeRate()));
//		break;
//	case ECOLOR_STATE::YELLOW:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(LINEAR_COLOR_ORANGE, FLinearColor::Yellow, GetColorChangeRate()));
//		break;
//	case ECOLOR_STATE::GREEN:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Green, GetColorChangeRate()));
//		break;
//	case ECOLOR_STATE::BLUE:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(FLinearColor::Green, FLinearColor::Blue, GetColorChangeRate()));
//		break;
//	case ECOLOR_STATE::INDIGO:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(FLinearColor::Blue, LINEAR_COLOR_INDIGO, GetColorChangeRate()));
//		break;
//	case ECOLOR_STATE::PURPLE:
//		m_ColorState.VectorColor = FVector(FLinearColor::LerpUsingHSV(LINEAR_COLOR_INDIGO, LINEAR_COLOR_PURPLE, GetColorChangeRate()));
//		break;
//	default:
//		break;
//	}
//}

//�F��ω�������J�E���g�̍X�V����
//void UColorLightComponent::UpdateColorChangeCnt(const float& DeltaTime)
//{
//	//�V�F�����ԂɃ��[�v����
//	if (m_ColorChangeTime > m_ColorChangeCnt)
//	{
//		m_ColorChangeCnt += DeltaTime;
//	}
//	else
//	{
//		//�F�ς��J�E���^���Z�b�g�A���̐F�̃X�e�b�v�Ɉڍs����
//		m_ColorChangeCnt = 0.f;
//		m_ColorState = (m_ColorState + 1) % ECOLOR_STATE::NUM;
//	}
//}

//���b�V���̃}�e���A���p�����[�^�[�̏����ݒ������֐�
void UColorLightComponent::InitializeMaterialParameter(UStaticMeshComponent* _staticMeshComponent, const bool _bGenerateSineWave, const float _maximumSineWave, const float minimumSineWave, const float _sinWavePeriod)
{
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("GenerateSineWave"), _bGenerateSineWave);
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MaximumSineWave"), _maximumSineWave);
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MinimumSineWave"), minimumSineWave);
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("SinWavePeriod"), _sinWavePeriod);
}

//���b�V���̃}�e���A���p�����[�^�[�̏����ݒ������֐�
void UColorLightComponent::InitializeMaterialParameter(USkeletalMeshComponent* _skeletalMeshComponent, const bool _bGenerateSineWave, const float _maximumSineWave, const float minimumSineWave, const float _sinWavePeriod)
{
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("GenerateSineWave"), _bGenerateSineWave);
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MaximumSineWave"), _maximumSineWave);
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MinimumSineWave"), minimumSineWave);
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("SinWavePeriod"), _sinWavePeriod);
}