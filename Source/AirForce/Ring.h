//------------------------------------------------------------------------
// �t�@�C����		:Ring.h
// �T�v				:�R�[�X�̐i�s������������O�̃N���X
// �쐬��			:2021/06/04
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/06/04
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "TickLODActor.h"
#include "NiagaraComponentPool.h"
#include "Ring.generated.h"

//�O���錾
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class ADroneBase;
class USoundBase;
class UCurveLinearColor;

//�R�t�����ꂽ�h���[���ƃG�t�F�N�g���i�[����\����
USTRUCT(BlueprintType)
struct FFollowingDroneAndEffect
{
	GENERATED_BODY()
public:
	//�R���X�g���N�^
	FFollowingDroneAndEffect();
	FFollowingDroneAndEffect(ADroneBase* pDrone, UNiagaraComponent* pNiagaraEffect = (UNiagaraComponent*)nullptr);
	//�f�X�g���N�^
	~FFollowingDroneAndEffect();
	//�h���[���擾
	ADroneBase* GetDrone()const { return m_pDrone.Get(); }
	//�G�t�F�N�g�擾
	UNiagaraComponent* GetEffect()const { return m_pNiagaraEffect.Get(); }
	//�G�t�F�N�g�������ς݂��ǂ���
	bool IsEffectSpawned() const { return m_bIsEffectSpawned; }
	//�G�t�F�N�g�𐶐�
	void SpawnEffectAtLocation(const UObject* WorldContextObject, UNiagaraSystem* SystemTemplate, FVector SpawnLocation, FRotator SpawnRotation = FRotator::ZeroRotator, FVector Scale = FVector(1.F), bool bAutoDestroy = true, bool bAutoActivate = true, ENCPoolMethod PoolingMethod = ENCPoolMethod::None);

public:
	UPROPERTY(VisibleAnywhere)
		TWeakObjectPtr<ADroneBase> m_pDrone;
	UPROPERTY(VisibleAnywhere)
		TWeakObjectPtr<UNiagaraComponent> m_pNiagaraEffect;
	UPROPERTY(VisibleAnywhere)
		bool m_bIsEffectSpawned;
};

UCLASS()
class AIRFORCE_API ARing : public ATickLODActor
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
	bool GetIsPassed()const { return m_bIsPassed; }
	//���b�V���擾
	UStaticMeshComponent* GetMesh()const { return m_pRingMesh; }

private:
	//�T�C���J�[�u�̒l���X�V
	void UpdateSineCurve(const float& CurrentTime);
	//�����O�̃T�C�Y�X�V
	void UpdateScale(const float& DeltaTime);
	//�����O�̃}�e���A���X�V
	void UpdateMaterial();
	//�����O�̃G�t�F�N�g�X�V
	void UpdateEffect();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;														//�����O�̃��b�V��
	UPROPERTY(EditAnywhere)
		TArray<struct FFollowingDroneAndEffect> m_pFollowingDroneAndEffect;			//�ʉ߂����h���[��
	UPROPERTY(EditAnywhere)
		UNiagaraSystem* m_pEffect;																		//�ʉߎ��ɏo���G�t�F�N�g
	UPROPERTY(VisibleAnywhere)
		bool m_bIsPassed;																						//���̃����O���ʉ߂��ꂽ������
	UPROPERTY(VisibleAnywhere)
		float m_PassedTime;																					//�ʉߌ�̎��Ԃ��v��
	UPROPERTY(VisibleAnywhere)
		float m_ResetTime;																					//�ʉߌ�̎��Ԃ��v��
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
		float m_RingMaxScale;																				//�����O�̍ő�X�P�[��
	UPROPERTY(EditAnywhere)
		FLinearColor m_HSV;																					//�����O�̐F
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_pRingHitSE;																		//�����O�Փ�SE
};
