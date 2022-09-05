
#include "ConfigParameter.h"
#include "Misc/ConfigCacheIni.h"

//static変数の実体を生成
float CConfigParameter::FixedFrameRate;


//読みとるコンフィグのパラメーター初期化
void CConfigParameter::InitConfigParameter()
{
	//フレームレートの値をConfig設定から読み出す
	float _FrameRate = 0.f;
	if(GConfig->GetFloat(TEXT("/Script/Engine.Engine"),TEXT("FixedFrameRate"),_FrameRate,GEngineIni))
	{
		CConfigParameter::FixedFrameRate = _FrameRate;

		UE_LOG(LogTemp,Warning,TEXT("Config::FixedFrameRate[%f]"),CConfigParameter::FixedFrameRate);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Config Read Failed!"));
	}
}
