//------------------------------------------------------------------------
// �t�@�C����		:ANetworkGameMode.cpp
// �T�v				:���O�C���A���O�A�E�g���̏������s���N���X�@�T�[�o�[�݂̂ɑ��݂���B
// �쐬��			:2022/02/28
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------


#include "NetworkGameMode.h"
#include "Engine/Engine.h"
#include "NetworkPlayerController.h"

//�R���X�g���N�^
ANetworkGameMode::ANetworkGameMode()
{
	//�v���C���[�����R���g���[����
	Role = ROLE_Authority;
	//�����Ώۃt���O
	bReplicates = false;
	//���L�������N���C�A���g�݂̂ɓ�������
	bOnlyRelevantToOwner = false;
}

//���O�C�����ɌĂ΂��֐�
void ANetworkGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		if (ANetworkPlayerController* pPlayerController = Cast<ANetworkPlayerController>(NewPlayer))
		{
			//���O�C�������v���C���[�̃R���g���[���[��ێ�
			m_pPlayerControllers.Add(pPlayerController);
		}
	}
}

//���O�A�E�g���ɌĂ΂��֐�
void ANetworkGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

}

	//�Q�[�����[�h�擾
ANetworkGameMode* ANetworkGameMode::Get()
{
	ANetworkGameMode* pGameMode = nullptr;
	if (GEngine)
	{
		if (UWorld* pWorld = GEngine->GetWorld())
		{
			pGameMode = pWorld->GetAuthGameMode<ANetworkGameMode>();
		}
	}
	return pGameMode;
}