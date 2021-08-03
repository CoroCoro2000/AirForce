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
template <typename WrappedRetValType, typename... ParamTypes>
class TBaseDelegate;
template<class FuncType>
class TFunction;
class FName;
class AActor;
struct FTimerHandle;

class CGameUtility
{
public:
	//*���x�����S�Ăɑ΂��Č������s�����߁ATick�Ŏg���ꍇ�͒��ӂ��邱�ƁB
	//�^�O����A�N�^�[���擾����֐�(��1�����F���̊֐����Ăяo���A�N�^�[�A��2�����F��������A�N�^�[�̃^�O��)
	static AActor* GetActorFromTag(AActor* _pOwnerActor, const FName& _tag);

	//*�e���v���[�g�֐��͕����R���p�C�����o���Ȃ����߁A�w�b�_�[�ɏ���
	//�C�ӎ��Ԍ�ɓo�^�����������Ăяo���֐�(��1�����F���[���h�A��2�����F�x�����鎞��(�b�Ŏw��)�A��3�����F�����̃��[�v�����邩�ǂ����A��4�����F�C�ӎ��Ԍ�Ɏ��s���鏈��(�����_��)�A��5�����F���s���鏈���ɕK�v�Ȉ���(�ϒ�))
	template<typename FunctorType, typename... VarTypes>
	static inline void Delay(UWorld* _pWorld, const float& _duration, const bool& _bLoop, const FunctorType&& _func, const VarTypes... _varTypes)
	{
		//NULL�`�F�b�N
		if (!_pWorld) { return; }

		//�f���Q�[�g���s���^�C���n���h�����쐬
		FTimerHandle timeHandle;
		//���s���郉���_�����f���Q�[�h�Ƀo�C���h
		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda((TFunction<void(VarTypes...)>&&)_func, _varTypes...);

		//�w�肳�ꂽ�b����Ɏ��s����悤�ɐݒ�
		_pWorld->GetTimerManager().SetTimer(timeHandle, timerDelegate, _duration, _bLoop);
	}

	//�C�ӎ��Ԍ�ɓo�^�����������Ăяo���֐�(��1�����F���[���h�A��2�����F�x�����鎞��(�b�Ŏw��)�A��3�����F�����̃��[�v�����邩�ǂ����A��4�����F��������֐��������o�[�Ɏ��N���X�̃|�C���^�A��5�����F���s����֐����A��6�����F���s���鏈���ɕK�v�Ȉ���(�ϒ�))
	//template<typename UserClass, typename... VarTypes>
	//static inline void Delay(UWorld* _pWorld, const float& _duration, const bool& _bLoop, UObject* _userObject, typename TMemFunPtrType<true, UObject, void(VarTypes...)>::Type _func, const VarTypes... _varTypes)
	//{
	//	//NULL�`�F�b�N
	//	if (!_pWorld || !_userObject || !_func) { return; }

	//	//�f���Q�[�g���s���^�C���n���h�����쐬
	//	FTimerHandle timeHandle;
	//	FTimerDelegate timerDelegate;
	//	timerDelegate.BindUObject(_userObject, _func, _varTypes...);

	//	//�w�肳�ꂽ�b����Ɏ��s����悤�ɐݒ�
	//	_pWorld->GetWorld()->GetTimerManager().SetTimer(timeHandle, timerDelegate, _duration, _bLoop);
	//}

};