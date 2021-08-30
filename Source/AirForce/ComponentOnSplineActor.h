//--------------------------------------------------------------------------------------------
// �t�@�C����		:ComponentOnSplineActor.h
// �T�v				:�X�v���C����ɃR���|�[�l���g��������������A�N�^�[�N���X
// �쐬��			:2021/08/30
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------
//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComponentOnSplineActor.generated.h"

UENUM(BlueprintType)
namespace EComponent
{
	enum Type
	{
		Niagara				UMETA(DisplayName = "Niagara"),
		NUM					UMETA(Hidden)
	};
}

//�O���錾
class USplineComponent;
class UNiagaraSystem;
class UNiagaraComponent;

//�f�o�b�O�pdefine
//#define DEBUG_TRANSFORM				//�g�����X�t�H�[���̃��O�o��
#define DEBUG_COMPONENTCOUNT			//�ێ����Ă���R���|�[�l���g�̐����o��

UCLASS()
class AIRFORCE_API AComponentOnSplineActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AComponentOnSplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//�X�v���C����ɐ�������Niagara�G�t�F�N�g���̍X�V
	void SpawnNiagaraOnSpline();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
		USplineComponent* m_pSpline;										//�I�u�W�F�N�g��z�u����ʒu�������X�v���C��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
		TEnumAsByte<EComponent::Type> m_ComponentType;						//�z�u����R���|�[�l���g�̎��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		UNiagaraSystem* m_pNiagaraEffect;									//�\������G�t�F�N�g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		TArray<UNiagaraComponent*> m_pNiagaraEffectComponents;				//�X�v���C����ɔz�u����G�t�F�N�g�R���|�[�l���g
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		int m_ComponentCount;												//�z�u����R���|�[�l���g�̐�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InstanceSetting")
		FRotator m_ComponentRelativeRotation;								//�z�u����R���|�[�l���g�̑��Ίp�x
};
