
#include "RacingD_GameMode.h"
#include "NetworkPlayerController.h"
#include "RacingD_GameSession.h"
#include "RacingD_GameState.h"
#include "RacingD_PlayerState.h"

ARacingD_GameMode::ARacingD_GameMode()
    : m_pPlayerControllers()
{
    //デフォルトクラス設定
    PlayerControllerClass = ANetworkPlayerController::StaticClass();
    GameSessionClass = ARacingD_GameSession::StaticClass();
    GameStateClass = ARacingD_GameState::StaticClass();
    PlayerStateClass = ARacingD_PlayerState::StaticClass();
}

void ARacingD_GameMode::PostLogin(APlayerController* NewPlayer)
{
    if(ANetworkPlayerController* pNetworkPlayerController = Cast<ANetworkPlayerController>(NewPlayer))
    {
        if(ARacingD_PlayerState* pRacingD_PlayerState = Cast<ARacingD_PlayerState>(pNetworkPlayerController->PlayerState))
        {
            //プレイヤーにロールを付ける
            //最初に参加したプレイヤーがサーバーになる
            pRacingD_PlayerState->SetServerRole(m_pPlayerControllers.Num() > 0 ? EServerRole::CLIENT : EServerRole::SERVER);
        }

        //プレイヤーをリストに追加
        m_pPlayerControllers.Add(pNetworkPlayerController);
    }
    
    Super::PostLogin(NewPlayer);
}

void ARacingD_GameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
    //新しいPlayerControllerに入れ替え
    m_pPlayerControllers.Remove(Cast<ANetworkPlayerController>(OldPC));
    m_pPlayerControllers.Add(Cast<ANetworkPlayerController>(NewPC));
    
    Super::SwapPlayerControllers(OldPC, NewPC);
}

void ARacingD_GameMode::Logout(AController* Exiting)
{
    //離脱するプレイヤーをリストから削除
    m_pPlayerControllers.Remove(Cast<ANetworkPlayerController>(Exiting));

    Super::Logout(Exiting);
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
