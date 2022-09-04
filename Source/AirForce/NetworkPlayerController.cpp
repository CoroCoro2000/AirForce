
#include "NetworkPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

ANetworkPlayerController::ANetworkPlayerController()
    : OnLoginCompleted()
{

}

ANetworkPlayerController* ANetworkPlayerController::GetNetworkPlayerController(const UObject* WorldContextObject)
{
    return Cast<ANetworkPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}

void ANetworkPlayerController::LoginEOS(const FLoginCompleted& _loginCompleted)
{
    if (IOnlineSubsystem* pOnlineSubsystem = Online::GetSubsystem(GetWorld()))
    {
        IOnlineIdentityPtr pIdentity = pOnlineSubsystem->GetIdentityInterface();
        if (pIdentity.IsValid())
        {
            //ローカルプレイヤーの取得
            if (ULocalPlayer* pLocalPlayer = this->GetLocalPlayer())
            {
                //コントローラIDの取得
                int32 ControllerId = pLocalPlayer->GetControllerId();
                if (pIdentity->GetLoginStatus(ControllerId) != ELoginStatus::LoggedIn)
                {
                    UE_LOG_ONLINE(Warning, TEXT("ComandLine: %s"), FCommandLine::Get());

                    //OnlineSubsystemのLoginCompleteデリゲートを登録
                    pIdentity->AddOnLoginCompleteDelegate_Handle(ControllerId, FOnLoginCompleteDelegate::CreateUObject(this, &ANetworkPlayerController::OnLoginCompleted_Internal));

                    //ログインの情報を作成
                    FOnlineAccountCredentials acountCredentials;
                    acountCredentials.Id = TEXT("localhost:8080");
                    acountCredentials.Token = TEXT("test");
                    acountCredentials.Type = TEXT("developer");

                    this->OnLoginCompleted = _loginCompleted;
                    pIdentity->Login(ControllerId, acountCredentials);
                    //pIdentity->AutoLogin(ControllerId);
                    return;
                }
                //ログイン済みの場合
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Already logged into EOS."));
                    _loginCompleted.ExecuteIfBound(true);
                    return;
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Online::GetSubsystem Failed."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invaild Identity"));
    }
    
    //ログイン失敗した時のデリゲート処理
    _loginCompleted.ExecuteIfBound(false);
}

void ANetworkPlayerController::OnLoginCompleted_Internal(int32 _localUserNum, bool _bWasSuccessful, const FUniqueNetId& _userId, const FString& _error)
{
    //ログイン成功
    if (_bWasSuccessful)
    {
        IOnlineIdentityPtr pIdentity = Online::GetIdentityInterface();
        if (pIdentity.IsValid())
        {
            //ローカルプレイヤーを取得
            ULocalPlayer* pLocalPlayer = Cast<ULocalPlayer>(this->GetLocalPlayer());
            if (pLocalPlayer != NULL)
            {
                int ControllerId = pLocalPlayer->GetControllerId();
                //現在のユニークIDの取得
                FUniqueNetIdRepl uniqueId = this->PlayerState->GetUniqueId();
                //通信用UniqueNetIDを設定
                uniqueId.SetUniqueNetId(FUniqueNetIdWrapper(_userId).GetUniqueNetId());
                //UniqueIDを更新
                this->PlayerState->SetUniqueId(uniqueId);
            }
        }
    }
    //ログイン失敗
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *_error);
    }

    //デリゲート処理
    OnLoginCompleted.ExecuteIfBound(_bWasSuccessful);
    OnLoginCompleted.Clear();
}
