//----------------------------------------------------------------------------------------------
// �t�@�C����		:WidgetBase.cpp
// �T�v				:UUserWidget���p������Widget�̃x�[�X�N���X
// �쐬��			:2021/08/23
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//----------------------------------------------------------------------------------------------

#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"

//�������̏��������鏈��
void UWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

//�Q�[���J�n���ɌĂ΂�鏈��
void UWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();

}

//�E�B�W�F�b�g���쐬���ꂽ�Ƃ��ɌĂяo����鏈��
void UWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

}

//�E�B�W�F�b�g���폜����Ƃ��Ăяo����鏈��
void UWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

}

//���t���[���X�V���鏈��
void UWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}
