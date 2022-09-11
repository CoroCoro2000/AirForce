
#include "RacingD_GameMode.h"
#include "NetworkPlayerController.h"
#include "RacingD_GameSession.h"
#include "RacingD_GameState.h"
#include "RacingD_PlayerState.h"

ARacingD_GameMode::ARacingD_GameMode()
{
    //デフォルトクラス設定
    PlayerControllerClass = ANetworkPlayerController::StaticClass();
    GameSessionClass = ARacingD_GameSession::StaticClass();
    GameStateClass = ARacingD_GameState::StaticClass();
    PlayerStateClass = ARacingD_PlayerState::StaticClass();
}

bool ARacingD_GameMode::KickPlayer(APlayerController* _kickedPlayer, const FText& _kickReason)
{
    return GameSession->KickPlayer(_kickedPlayer, _kickReason);
}

bool ARacingD_GameMode::ServerTravel(const FString& _levelName, const bool _bAbsolute, const bool _bShouldSkipGameNotify, const FString _option)
{
    const FString url = _option.IsEmpty() ? _levelName : _levelName + TEXT("?") + _option;
    return GetWorld()->ServerTravel(url, _bAbsolute, _bShouldSkipGameNotify);
}
