//----------------------------------------------------------------------------------------------
// �t�@�C����		:GameUtility.h
// �T�v				:�ǂ�����ł��Ăяo����֐��̏������܂Ƃ߂����[�e�B���e�B�N���X
// �쐬��			:2021/07/26
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//----------------------------------------------------------------------------------------------
//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "TimerManager.h"

//�O���錾
class AActor;

class CGameUtility
{
public:
	//������n�ʖ����؂�̂�
	static float SetDecimalTruncation(float value, const int& n);

	//*���x�����S�Ăɑ΂��Č������s�����߁ATick�Ŏg���ꍇ�͒��ӂ��邱�ƁB
	//�^�O����A�N�^�[���擾����֐�(��1�����F���̊֐����Ăяo���A�N�^�[�A��2�����F��������A�N�^�[�̃^�O��)
	static AActor* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag);
};