//----------------------------------------------------------------------------------------------
// �t�@�C����		:WidgetBase.h
// �T�v				:UUserWidget���p������Widget�̃x�[�X�N���X
// �쐬��			:2021/08/23
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//----------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
    //�������̏��������鏈��
    virtual void NativeOnInitialized() override;
    //�Q�[���J�n���ɌĂ΂�鏈��
    virtual void NativePreConstruct() override;
    //�E�B�W�F�b�g���쐬���ꂽ�Ƃ��ɌĂяo����鏈��
    virtual void NativeConstruct() override;
    //�E�B�W�F�b�g���폜����Ƃ��Ăяo����鏈��
    virtual void NativeDestruct() override;
    //���t���[�����s����鏈��
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    //�E�B�W�F�b�g���r���[�|�[�g��ɑ��݂��邩�m�F����֐�
    bool WidgetIsValid()const { return GetWorld()->GetGameViewport() ? true : false; }

protected:

};
