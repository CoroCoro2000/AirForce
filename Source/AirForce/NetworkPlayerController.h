//------------------------------------------------------------------------
// �t�@�C����		:ANetworkPlayerController.h
// �T�v				:�N���C�A���g��Pawn��HUD�̑�����s���R���g���[���[�@�T�[�o�[�A�e�N���C�A���g�ɑ��݂���
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

class APlayerDrone;

UCLASS()
class AIRFORCE_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//�R���X�g���N�^
	ANetworkPlayerController();

protected:
	//�Q�[���J�n���Ɏ��s�����֐�
	virtual void BeginPlay()override;
	//���t���[�����s�����֐�
	virtual void Tick(float DeltaTime)override;
	//���v���P�[�g��o�^
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	//�T�[�o�[�p�@Transform�K�p
	UFUNCTION(Server, Unreliable)
		void Server_ApplyTransform();
	//�N���C�A���g�p�@Transform��n��
	UFUNCTION(Client, Unreliable)
		void Client_SetTransform();

	//�v���C���[��Transform����
	void SynchronizePlayerTransform();
	//�v���C���[���̍X�V
	UFUNCTION()
		void ActivateUpdate();

public:
	//�Q�[���R���g���[���[�擾
	static ANetworkPlayerController* Get();

private:
	UPROPERTY(EditAnywhere)
		float m_SynchronousInterval;			//��������Ԋu(�b�P��)
	UPROPERTY(EditAnywhere, Replicated)
		bool m_bCanUpdate;						//�X�V�\�t���O
};
