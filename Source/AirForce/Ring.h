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
class ADroneBase;

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
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	//�ʉ߂���Ă��邩�̃t���O�擾
	FORCEINLINE bool GetIsPassed()const { return m_bIsPassed; }
	//���b�V���擾
	FORCEINLINE UStaticMeshComponent* GetMesh()const { return m_pRingMesh; }
	//�i�C�A�K���R���|�[�l���g�擾
	FORCEINLINE UNiagaraComponent* GetEffectComponent()const { return m_pNiagaraEffectComp; }

private:
	//�����O�̃T�C�Y�X�V
	void UpdateScale(const float& DeltaTime);

	//�����O�̃}�e���A���X�V
	void UpdateMaterial(const float& DeltaTime);

	//�����O�̃G�t�F�N�g�X�V
	void UpdateEffect(const float& DeltaTime);

	//�����O�̏�����
	void Reset();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;				//�����O�̃��b�V��
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* m_pNiagaraEffectComp;		//�����O��ʉ߂����ۂɏo���G�t�F�N�g
	UPROPERTY(VisibleAnywhere)
		bool m_bIsPassed;												//���̃����O���ʉ߂��ꂽ������
	UPROPERTY(VisibleAnywhere)
		float m_MakeInvisibleCnt;									//�����O��������܂ł̃J�E���^�[
	UPROPERTY(EditAnywhere)
		float m_MakeInvisibleTime;									//�����O��������܂ł̎���
	UPROPERTY(EditAnywhere)
		float m_SineWidth;												//�T�C���g�̊Ԋu
	UPROPERTY(EditAnywhere)
		float m_SineScaleMin;											//�T�C���g�̍ŏ��l
	UPROPERTY(EditAnywhere)
		float m_SineScaleMax;										//�T�C���g�̍ő�l
	UPROPERTY(EditAnywhere)
		float m_PassedSceleMax;									//�����O�ʉߌ�̑傫��
	UPROPERTY(VisibleAnywhere)
		ADroneBase* m_pPassedDrone;							//���̃����O��ʉ߂����h���[��
	UPROPERTY(EditAnywhere)
		FLinearColor m_HSV;											//�����O�̐F
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_RingHIttSE;						//�����O�Փ�SE
	UPROPERTY(VisibleAnywhere)
		FTransform m_InitialTransform;							//�����O�̏����g�����X�t�H�[��
};
