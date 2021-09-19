//------------------------------------------------------------------------
// ファイル名		:ColorLightComponent.cpp
// 概要				:マテリアルのカラー情報を管理するコンポーネント
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
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

//毎フレーム処理される関数
void UColorLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//色を変化させるカウントの更新処理
	//UpdateColorChangeCnt(DeltaTime);

	////色情報の更新
	//UpdateColor(DeltaTime);
}

//色情報の更新処理
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

//色を変化させるカウントの更新処理
//void UColorLightComponent::UpdateColorChangeCnt(const float& DeltaTime)
//{
//	//７色を順番にループする
//	if (m_ColorChangeTime > m_ColorChangeCnt)
//	{
//		m_ColorChangeCnt += DeltaTime;
//	}
//	else
//	{
//		//色変えカウンタリセット、次の色のステップに移行する
//		m_ColorChangeCnt = 0.f;
//		m_ColorState = (m_ColorState + 1) % ECOLOR_STATE::NUM;
//	}
//}

//メッシュのマテリアルパラメーターの初期設定をする関数
void UColorLightComponent::InitializeMaterialParameter(UStaticMeshComponent* _staticMeshComponent, const bool _bGenerateSineWave, const float _maximumSineWave, const float minimumSineWave, const float _sinWavePeriod)
{
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("GenerateSineWave"), _bGenerateSineWave);
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MaximumSineWave"), _maximumSineWave);
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MinimumSineWave"), minimumSineWave);
	_staticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("SinWavePeriod"), _sinWavePeriod);
}

//メッシュのマテリアルパラメーターの初期設定をする関数
void UColorLightComponent::InitializeMaterialParameter(USkeletalMeshComponent* _skeletalMeshComponent, const bool _bGenerateSineWave, const float _maximumSineWave, const float minimumSineWave, const float _sinWavePeriod)
{
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("GenerateSineWave"), _bGenerateSineWave);
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MaximumSineWave"), _maximumSineWave);
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("MinimumSineWave"), minimumSineWave);
	_skeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("SinWavePeriod"), _sinWavePeriod);
}