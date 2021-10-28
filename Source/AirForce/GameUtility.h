//----------------------------------------------------------------------------------------------
// �t�@�C����		:GameUtility.h
// �T�v				:�ǂ�����ł��Ăяo����֐��̏������܂Ƃ߂����[�e�B���e�B�N���X
// �쐬��			:2021/07/26
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//----------------------------------------------------------------------------------------------
//�C���N���[�h�K�[�h
#pragma once

//�O���錾
class AActor;

class CGameUtility
{
public:
	//������n�ʖ����؂�̂�
	static float SetDecimalTruncation(float value, int n);
	//������n�ʖ����؂�̂�
	static FVector SetDecimalTruncation(FVector value, int n);
	//FPS���ς���Ă��ړ��ʂ����ɂ���{����Ԃ��֐�
	FORCEINLINE static float MoveCorrection(const float& DeltaTime) { return  60.f / (1.f / DeltaTime); }
	//*���x�����S�Ăɑ΂��Č������s�����߁ATick�Ŏg���ꍇ�͒��ӂ��邱�ƁB
	//�^�O����A�N�^�[���擾����֐�(��1�����F���̊֐����Ăяo���A�N�^�[�A��2�����F��������A�N�^�[�̃^�O��)
	static AActor* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag);
};