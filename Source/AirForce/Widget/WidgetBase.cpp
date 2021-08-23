//----------------------------------------------------------------------------------------------
// ファイル名		:WidgetBase.cpp
// 概要				:UUserWidgetを継承するWidgetのベースクラス
// 作成日			:2021/08/23
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//----------------------------------------------------------------------------------------------

#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"

//生成時の初期化する処理
void UWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

//ゲーム開始時に呼ばれる処理
void UWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();

}

//ウィジェットが作成されたときに呼び出される処理
void UWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

}

//ウィジェットを削除するとき呼び出される処理
void UWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

}

//毎フレーム更新する処理
void UWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}
