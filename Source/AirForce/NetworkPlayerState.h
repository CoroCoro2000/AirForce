//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:ANetworkPlayerState.h
// �T�v				:�N���C�A���g��PlayerController����Transform���󂯎��A�ێ�����v���C���[�X�e�[�g�N���X�@�T�[�o�[�ƃN���C�A���g�����ꂼ��1�����L���A�����Ɋ֌W�Ȃ��ǂݎ�邱�Ƃ��ł���B
// �쐬��			:2022/03/01
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetworkPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API ANetworkPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	//�R���X�g���N�^
	ANetworkPlayerState();

protected:
	//�Q�[���J�n���Ɏ��s
	virtual void BeginPlay()override;
	//���t���[�����s
	virtual void Tick(float DeltaTime)override;
	//���v���P�[�g��o�^
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

public:
	//�@�N���C�A���g�p�@�g�����X�t�H�[����ݒ�
	void SetPlayerTransform(const FTransform& NewTransform) { m_PlayerTransform = NewTransform; }
	//�@�T�[�o�[�p�@�g�����X�t�H�[�����擾
	FTransform GetPlayerTransform()const { return m_PlayerTransform; }

private:
	UPROPERTY(EditAnywhere, Replicated)
		FTransform m_PlayerTransform;								//�v���C���[��Transform���i�[
};
