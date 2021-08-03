//------------------------------------------------------------------------
// �t�@�C����		:DroneBullet.h
// �T�v				:�h���[���̒e�̃N���X
// �쐬��			:2021/05/17
// �쐬��			:19CU0104 �r�c�Ĉ�Y
// �X�V���e			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DroneBullet.generated.h"

UCLASS()
class AIRFORCE_API ADroneBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	//�R���X�g���N�^
	ADroneBullet();

protected:
	//�Q�[���J�n����1�x��������
	virtual void BeginPlay() override;

	//	�I�[�o�[���b�v�ڐG���n�߂����ɌĂ΂��C�x���g�C�x���g�֐���o�^
	UFUNCTION(BlueprintCallable, Category = "Overlap")
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	//���t���[������
	virtual void Tick(float DeltaTime) override;

	//	�e�̓����蔻��̎擾
	UBoxComponent* GetBulletBoxComp() const { return m_pBulletBoxComp; };

	//	�h���[���̑��x�����Z
	void AddDroneSpeed(const float speed) { m_BulletSpeed += speed;}
private:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pBulletMesh;		//�e�̃��b�V��
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UBoxComponent* m_pBulletBoxComp;			//�e�̓����蔻��

	UPROPERTY(EditAnywhere, Category = "Speed")
		float m_BulletSpeed;							//	�e�̃X�s�[�h

	UPROPERTY(VisibleAnywhere, Category = "Time")
		float m_DeleteTimeCount;							//	�e�̍폜����

	UPROPERTY(EditAnywhere, Category = "Time")
		float m_DeleteTimeCountMax;					//	�e�̍폜����

public:
	UPROPERTY(VisibleAnywhere)
		FVector Start;										//�e�̔��ˊJ�n���W
	UPROPERTY(VisibleAnywhere)
		FVector End;											//�e�̒��e���W
};