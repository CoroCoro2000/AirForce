//------------------------------------------------------------------------
// �t�@�C����		:ANetworkGameState.h
// �T�v				:�T�[�o�[�@�́@�N���C�A���g�Ԃ̂������Ǘ�����@�T�[�o�[�ƃN���C�A���g�S�̂�1�����L����B
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

public:
	//�Q�[���X�e�[�g�擾
	static ANetworkGameState* Get();


private:
	UPROPERTY(EditAnywhere)
		TArray<APlayerDrone*> m_pPlayerDrones;
};
