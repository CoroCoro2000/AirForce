//----------------------------------------------------------------------------------------------
// ファイル名		:WidgetBase.h
// 概要				:UUserWidgetを継承するWidgetのベースクラス
// 作成日			:2021/08/23
// 作成者			:19CU0105 池村凌太
// 更新内容			:
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
    //生成時の初期化する処理
    virtual void NativeOnInitialized() override;
    //ゲーム開始時に呼ばれる処理
    virtual void NativePreConstruct() override;
    //ウィジェットが作成されたときに呼び出される処理
    virtual void NativeConstruct() override;
    //ウィジェットを削除するとき呼び出される処理
    virtual void NativeDestruct() override;
    //毎フレーム実行される処理
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    //ウィジェットがビューポート上に存在するか確認する関数
    bool WidgetIsValid()const { return GetWorld()->GetGameViewport() ? true : false; }

protected:

};
