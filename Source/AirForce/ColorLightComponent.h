//------------------------------------------------------------------------
// ファイル名		:ColorLightComponent.h
// 概要				:マテリアルのカラー情報を管理するコンポーネント
// 作成日			:2021/06/04
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/06/04
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorLightComponent.generated.h"

//カラーステート情報
UENUM(BlueprintType)
enum class ECOLOR_STATE : uint8
{
	RED							UMETA(DisplayName = "RED"),
	ORANGE					UMETA(DisplayName = "ORANGE"),
	YELLOW					UMETA(DisplayName = "YELLOW"),
	GREEN						UMETA(DisplayName = "GREEN"),
	BLUE							UMETA(DisplayName = "BLUE"),
	INDIGO						UMETA(DisplayName = "INDIGO"),
	PURPLE						UMETA(DisplayName = "PURPLE"),
	ENUM_SIZE				UMETA(Hidden),
};

//カラーステート管理構造体
USTRUCT(BlueprintType)
struct FCOLOR_STATE
{
	GENERATED_USTRUCT_BODY()

	//コンストラクタ
	FCOLOR_STATE()
		: COLOR_STATE(ECOLOR_STATE::RED)
		, VectorColor(FVector::ZeroVector)
	{}
	FCOLOR_STATE(const uint8 _colorType)
		: COLOR_STATE((ECOLOR_STATE)_colorType)
		, VectorColor(FVector::ZeroVector)
	{}
	FCOLOR_STATE(const ECOLOR_STATE _colorType)
		: COLOR_STATE(_colorType)
		, VectorColor(FVector::ZeroVector)
	{}

	//演算子オーバーロード
	FCOLOR_STATE& operator++() 
	{
		COLOR_STATE = ECOLOR_STATE((uint8)COLOR_STATE + 1); 
		return *this;
	}
	FCOLOR_STATE& operator--()
	{
		COLOR_STATE = ECOLOR_STATE((uint8)COLOR_STATE - 1);
		return *this;
	}
	FCOLOR_STATE operator+(uint8 n) { return FCOLOR_STATE((uint8)COLOR_STATE + n); }
	FCOLOR_STATE operator%(uint8 n) { return FCOLOR_STATE((uint8)COLOR_STATE % n); }
	FCOLOR_STATE operator%(ECOLOR_STATE n) { return FCOLOR_STATE((uint8)COLOR_STATE % (uint8)n); }

public:
	ECOLOR_STATE COLOR_STATE;				//カラーステート
	FVector VectorColor;								//ベクターカラー情報
};

//カラー情報
#define LINEAR_COLOR_ORANGE FLinearColor(0.94921875f, 0.609375f, 0.0703125f)		//オレンジ
#define LINEAR_COLOR_PURPLE FLinearColor(0.66015625f, 0.02734375f, 0.890625f)		//紫
#define LINEAR_COLOR_INDIGO FLinearColor(0, 0.296875f, 0.44140625f)						//藍色

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AIRFORCE_API UColorLightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UColorLightComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

private:
	//色情報の更新処理
	void UpdateColor(const float& DeltaTime);
	//色を変化させるカウントの更新処理
	void UpdateColorChangeCnt(const float& DeltaTime);
	//カラーの切り替えカウントの進行度を取得
	float GetColorChangeRate()const { return FMath::Clamp(m_ColorChangeCnt / m_ColorChangeTime, 0.f, 1.f); }
public:
	//メッシュのマテリアルパラメーターの初期設定をする関数
	void InitializeMaterialParameter(UStaticMeshComponent* _staticMeshComponent, const bool _bGenerateSineWave = true, const float _maximumSineWave = 30.f, const float minimumSineWave = 5.f, const float _sinWavePeriod = 0.3f);
	void InitializeMaterialParameter(USkeletalMeshComponent* _skeletalMeshComponent, const bool _bGenerateSineWave = true, const float _maximumSineWave = 30.f, const float minimumSineWave = 5.f, const float _sinWavePeriod = 0.3f);

	//このコンポーネントを毎フレーム更新するかどうか
	void Activate(const bool _bActive) { PrimaryComponentTick.bCanEverTick = _bActive; }
	//色の設定
	void SetColor(const FLinearColor _color) { m_ColorState.VectorColor = FVector(_color); }
	//現在のカラー情報を取得
	FLinearColor GetLinearColor()const { return FLinearColor(m_ColorState.VectorColor); }
	//現在のベクターカラー情報を取得する関数
	FVector GetVectorColor()const { return m_ColorState.VectorColor; }

private:
	UPROPERTY(VisibleAnywhere, Category = "ColorManager")
		//カラー情報
		FCOLOR_STATE m_ColorState;
	UPROPERTY(VisibleAnywhere, Category = "Rainbow", meta = (EditCondition = "bRainbowLoop"))
		//次の色に切り替わるまでの時間カウント
		float m_ColorChangeCnt;
	UPROPERTY(EditAnywhere, Category = "Rainbow", meta = (EditCondition = "bRainbowLoop"))
		//次の色に切り替わるまでの時間
		float m_ColorChangeTime;
};
