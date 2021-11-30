//------------------------------------------------------------------------
// �t�@�C����		:Ring.h
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ring.generated.h"

//�O���錾
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ADroneBase;

USTRUCT(BlueprintType)
struct FFollowingEffectDronePair
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = FollowingEffectDronePair)
		ADroneBase* pDrone;
	UPROPERTY(EditAnywhere, Category = FollowingEffectDronePair)
		UNiagaraComponent* pFollowingEffect;

	//�R���X�g���N�^
	FFollowingEffectDronePair()
		: pDrone(nullptr)
		, pFollowingEffect(nullptr)
	{}
	FFollowingEffectDronePair(ADroneBase* Drone, UNiagaraComponent* FollowingEffect)
		: pDrone(Drone)
		, pFollowingEffect(FollowingEffect)
	{}
};

UCLASS()
class AIRFORCE_API ARing : public AActor
{
	GENERATED_BODY()
	
public:	
	//�R���X�g���N�^
	ARing();

protected:
	//�Q�[���J�n���܂��͂��̃N���X�̃I�u�W�F�N�g���X�|�[�����ꂽ���P�x�����Ăяo�����֐�
	virtual void BeginPlay() override;

public:	
	//���t���[���Ăяo�����֐�
	virtual void Tick(float DeltaTime) override;

protected:
	//�I�[�o�[���b�v���ɌĂ΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	//�ʉ߂���Ă��邩�̃t���O�擾
	FORCEINLINE bool GetIsPassed()const { return m_bIsPassed; }
	//���b�V���擾
	UStaticMeshComponent* GetMesh()const { return m_pRingMesh; }

private:
	//�T�C���J�[�u�̒l���X�V
	void UpdateSineCurve(const float& DeltaTime);
	//�����O�̃T�C�Y�X�V
	void UpdateScale(const float& DeltaTime);
	//�����O�̃}�e���A���X�V
	void UpdateMaterial(const float& DeltaTime);
	//�����O�̃G�t�F�N�g�X�V
	void UpdateEffect(const float& DeltaTime);

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;														//�����O�̃��b�V��
	UPROPERTY(EditAnywhere)
		TArray<struct FFollowingEffectDronePair> m_pFollowingEffectDronePairs;		//�ʉ߂����h���[���ƒǏ]�G�t�F�N�g�̃y�A
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* m_pEffect;																		//�ʉߎ��ɏo���G�t�F�N�g
	UPROPERTY(VisibleAnywhere)
		bool m_bIsPassed;																						//���̃����O���ʉ߂��ꂽ������
	UPROPERTY(EditAnywhere)
		float m_SineWidth;																						//�T�C���g�̊Ԋu
	UPROPERTY(EditAnywhere)
		float m_SineScaleMin;																					//�T�C���g�̍ŏ��l
	UPROPERTY(EditAnywhere)
		float m_SineScaleMax;																				//�T�C���g�̍ő�l
	UPROPERTY(EditAnywhere)
		float m_SineCurveValue;																				//�T�C���g�̒l
	UPROPERTY(VisibleAnywhere)
		float m_RingScale;																						//�����O�̃X�P�[��
	UPROPERTY(EditAnywhere)
		FLinearColor m_HSV;																					//�����O�̐F
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_RingHitSE;																		//�����O�Փ�SE
};
