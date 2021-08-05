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
#include "DroneBase.h"
#include "Ring.generated.h"

//�O���錾
class UStaticMeshComponent;
class UColorLightComponent;
class UNiagaraComponent;
class APlayerDrone;
class UCurveFloat;

#define DEBUG_HIT

UCLASS(HideCategories = (Input, Rendering, Replication, LOD, Cooking))
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
	bool isDraw();
	int GetRingNumber() { return m_RingNumber; }
protected:
	//�I�[�o�[���b�v���ɌĂ΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere)
		//�����O�̐F���Ǘ�����R���|�[�l���g
		UColorLightComponent* m_pColorLightComp;
	UPROPERTY(EditAnywhere)
		//�����O�̃��b�V��
		UStaticMeshComponent* m_pRingMesh;
	UPROPERTY(EditAnywhere, Category = "Ring")
		int m_RingNumber;							//�����O�̔ԍ����蓖��
	UPROPERTY(EditAnywhere, Category = "Ring")
		int m_RingDrawUpNumber;						//�v���C���[����̃����O�܂ŕ`�悷�邩���߂�i���o�[
	UPROPERTY(EditAnywhere, Category = "Ring")
		UNiagaraComponent* m_pNiagaraEffectComp;	//�����O��ʉ߂����ۂɏo���G�t�F�N�g
	UPROPERTY(EditAnywhere, Category = "Ring")
		bool m_bIsPassed;							//���̃����O���ʉ߂��ꂽ������

	UPROPERTY(EditAnywhere, Category = "Ring")
		APlayerDrone* m_pDrone;

	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_MakeInvisibleCnt;
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_MakeInvisibleTime;
	UPROPERTY(EditAnywhere, Category = "Ring")
		UCurveFloat* m_pScaleCurve;
};
