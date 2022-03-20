
#pragma once

//ゲームのコンフィグ設定を取得できるクラス
class CConfigParameter
{
public:
	//読みとるコンフィグのパラメーター初期化
	static void InitConfigParameter();
	//固定フレームレートの値
	static float FixedFrameRate;
};