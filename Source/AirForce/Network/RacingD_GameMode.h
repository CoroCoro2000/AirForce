/*
* マルチ対戦時のサーバーで制御する処理をまとめたゲームモードクラス
*
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RacingD_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API ARacingD_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    /**
     * @brief コンストラクタ
    */
    ARacingD_GameMode();

};
