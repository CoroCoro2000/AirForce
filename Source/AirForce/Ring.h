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
class UColorLightComponent;
class UNiagaraComponent;
class APlayerDrone;
class UCurveFloat;

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
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void SetActivate(const bool& _isActive);
	void SetDroneLocation(const FVector& _droneLocation) { m_PassedDroneLocation = _droneLocation; }
	void SetRingNumber(const int& _ringNumber) { m_RingNumber = _ringNumber; }
	int GetRingNumber() { return m_RingNumber; }
	bool IsPassed()const { return m_bIsPassed; }
	bool IsDestroy()const { return m_bDestroy; }

private:
	//�����O�̐F�̍X�V����
	void UpdateColor(const float& DeltaTime);
	//�����O�̃g�����X�t�H�[���X�V
	void UpdateTransform(const float& DeltaTime);

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRingMesh;				//�����O�̃��b�V��
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* m_pNiagaraEffectComp;		//�����O��ʉ߂����ۂɏo���G�t�F�N�g
	UPROPERTY(EditAnywhere)
		UColorLightComponent* m_pColorLightComp;		//�����O�̐F���Ǘ�����R���|�[�l���g
	UPROPERTY(EditAnywhere)
		int m_RingNumber;											//�����O�̔ԍ�
	UPROPERTY(EditAnywhere)
		bool m_bIsPassed;												//���̃����O���ʉ߂��ꂽ������
	UPROPERTY(EditAnywhere)
		bool m_bDestroy;												//�����O���������ǂ����̃t���O
	UPROPERTY(EditAnywhere)
		float m_MakeInvisibleCnt;									//�����O�������ۂ̃J�E���^�[
	UPROPERTY(EditAnywhere)
		float m_MakeInvisibleTime;									//�����O��������܂ł̎���
	UPROPERTY(EditAnywhere)
		float m_InvisibleCntRate;									//�����O�������J�E���^�[�̐i�s�x
	UPROPERTY(EditAnywhere)
		UCurveFloat* m_pScaleCurve;								//�����O�̑傫�������߂�J�[�u
	UPROPERTY(EditAnywhere)
		FVector m_PassedDroneLocation;						//�����O�����������h���[���̍��W
};
