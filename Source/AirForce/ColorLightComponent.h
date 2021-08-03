//------------------------------------------------------------------------
// �t�@�C����		:ColorLightComponent.h
// �T�v				:�}�e���A���̃J���[�����Ǘ�����R���|�[�l���g
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorLightComponent.generated.h"

//�J���[�X�e�[�g���
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

//�J���[�X�e�[�g�Ǘ��\����
USTRUCT(BlueprintType)
struct FCOLOR_STATE
{
	GENERATED_USTRUCT_BODY()

	//�R���X�g���N�^
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

	//���Z�q�I�[�o�[���[�h
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
	ECOLOR_STATE COLOR_STATE;				//�J���[�X�e�[�g
	FVector VectorColor;								//�x�N�^�[�J���[���
};

//�J���[���
#define LINEAR_COLOR_ORANGE FLinearColor(0.94921875f, 0.609375f, 0.0703125f)		//�I�����W
#define LINEAR_COLOR_PURPLE FLinearColor(0.66015625f, 0.02734375f, 0.890625f)		//��
#define LINEAR_COLOR_INDIGO FLinearColor(0, 0.296875f, 0.44140625f)						//���F

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
	//�F���̍X�V����
	void UpdateColor(const float& DeltaTime);
	//�F��ω�������J�E���g�̍X�V����
	void UpdateColorChangeCnt(const float& DeltaTime);
	//�J���[�̐؂�ւ��J�E���g�̐i�s�x���擾
	float GetColorChangeRate()const { return FMath::Clamp(m_ColorChangeCnt / m_ColorChangeTime, 0.f, 1.f); }
public:
	//���b�V���̃}�e���A���p�����[�^�[�̏����ݒ������֐�
	void InitializeMaterialParameter(UStaticMeshComponent* _staticMeshComponent, const bool _bGenerateSineWave = true, const float _maximumSineWave = 30.f, const float minimumSineWave = 5.f, const float _sinWavePeriod = 0.3f);
	void InitializeMaterialParameter(USkeletalMeshComponent* _skeletalMeshComponent, const bool _bGenerateSineWave = true, const float _maximumSineWave = 30.f, const float minimumSineWave = 5.f, const float _sinWavePeriod = 0.3f);

	//���̃R���|�[�l���g�𖈃t���[���X�V���邩�ǂ���
	void Activate(const bool _bActive) { PrimaryComponentTick.bCanEverTick = _bActive; }
	//�F�̐ݒ�
	void SetColor(const FLinearColor _color) { m_ColorState.VectorColor = FVector(_color); }
	//���݂̃J���[�����擾
	FLinearColor GetLinearColor()const { return FLinearColor(m_ColorState.VectorColor); }
	//���݂̃x�N�^�[�J���[�����擾����֐�
	FVector GetVectorColor()const { return m_ColorState.VectorColor; }

private:
	UPROPERTY(VisibleAnywhere, Category = "ColorManager")
		//�J���[���
		FCOLOR_STATE m_ColorState;
	UPROPERTY(VisibleAnywhere, Category = "Rainbow", meta = (EditCondition = "bRainbowLoop"))
		//���̐F�ɐ؂�ւ��܂ł̎��ԃJ�E���g
		float m_ColorChangeCnt;
	UPROPERTY(EditAnywhere, Category = "Rainbow", meta = (EditCondition = "bRainbowLoop"))
		//���̐F�ɐ؂�ւ��܂ł̎���
		float m_ColorChangeTime;
};
