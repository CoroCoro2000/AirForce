//------------------------------------------------------------------------
// �t�@�C����		:ANetworkGameState.h
// �T�v				:�T�[�o�[�@�́@�N���C�A���g�Ԃ̂������Ǘ�����Q�[���X�e�[�g
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NetworkGameState.generated.h"

class APlayerDrone;

/**
 * 
 */
UCLASS()
class AIRFORCE_API ANetworkGameState : public AGameState
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	ANetworkGameState();

protected:
	//�Q�[���J�n���Ɏ��s�����֐�
	virtual void BeginPlay()override;
	//���t���[�����s�����֐�
	virtual void Tick(float DeltaTime)override;
	//���v���P�[�g����v���p�e�B���X�V����
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//���[�J���ő��삵�Ă���v���C���[���擾
	UFUNCTION(BlueprintCallable)
		APlayerDrone* GetLocalPlayerDrone()const;

	UFUNCTION()
		void OnRep_CurrentDrone();

protected:
	UPROPERTY(EditAnywhere)
		uint8 m_PlayerIndex;													//���[�J���v���C���[�̎��ʔԍ�
	UPROPERTY(EditAnywhere, Replicated, ReplicatedUsing = OnRep_CurrentDrone)
		TArray<APlayerDrone*> m_pPlayerDrones;					//�Z�b�V�����ɎQ�����Ă���v���C���[�̍��W
};
