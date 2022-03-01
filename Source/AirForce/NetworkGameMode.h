//------------------------------------------------------------------------
// �t�@�C����		:ANetworkGameMode.h
// �T�v				:���O�C���A���O�A�E�g���̏������s���N���X�@�T�[�o�[�݂̂ɑ��݂���B
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NetworkGameMode.generated.h"

class ANetworkPlayerController;

/**
 * 
 */
UCLASS()
class AIRFORCE_API ANetworkGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	ANetworkGameMode();

protected:
	//���O�C�����ɌĂ΂��֐�
	virtual void PostLogin(APlayerController* NewPlayer)override;
	//���O�A�E�g���ɌĂ΂��֐�
	virtual void Logout(AController* Exiting)override;

public:
	//�Q�[�����[�h�擾
	static ANetworkGameMode* Get();

private:
	UPROPERTY(EditAnywhere)
		TArray<ANetworkPlayerController*> m_pPlayerControllers;				//�Z�b�V�����ɎQ�����̃v���C���[�̃R���g���[���[
};
