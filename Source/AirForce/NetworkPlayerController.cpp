
#include "NetworkPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

ANetworkPlayerController::ANetworkPlayerController()
    : OnLoginCompleted()
    , OnCreateSessionCompleted()
{

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

bool ANetworkPlayerController::CreateSession(const int32 _connection,const FString _searchKeyword, const FName _sessionName, const FCreateSessionCompleted& _createSessionCompleted)
{
    if (IOnlineSubsystem* const pOnlineSubsystem = Online::GetSubsystem(GetWorld()))
    {
        IOnlineSessionPtr Sessions = pOnlineSubsystem->GetSessionInterface();
        if (Sessions.IsValid())
        {
            TSharedPtr<FOnlineSessionSettings> pSessionSettings = MakeShareable(new FOnlineSessionSettings());
            //制限なしで参加できる人数
            pSessionSettings->NumPublicConnections = _connection;

            pSessionSettings->NumPrivateConnections = 0;
            //セッションの検索を許可する
            pSessionSettings->bShouldAdvertise = true;
            //Session開始後でも参加を許可する
            pSessionSettings->bAllowJoinInProgress = true;
            //招待を許可する
            pSessionSettings->bAllowInvites = true;
            //プレゼンスにロビー情報をのせる
            pSessionSettings->bUsesPresence = true;
            //SessionIDを知っているユーザーであれば参加可能にする
            pSessionSettings->bAllowJoinViaPresence = true;
            //EOSロビーサービスを利用するように設定
            pSessionSettings->bUseLobbiesIfAvailable = true;
            //ロビーによる音声通話の設定
            pSessionSettings->bUseLobbiesVoiceChatIfAvailable = false;

            //検索ワードとしてSearchKeywordを設定
            pSessionSettings->Set(SEARCH_KEYWORDS, _searchKeyword, EOnlineDataAdvertisementType::ViaOnlineService);

            //セッション作成終了時デリゲートの登録
            OnCreateSessionCompleted = _createSessionCompleted;
            Sessions->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ANetworkPlayerController::OnCreateSessionCompleted_Internal));

            TSharedPtr<const FUniqueNetId> pUniqueNetIdptr = this->GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
            //セッションの作成
            bool bResult = Sessions->CreateSession(*pUniqueNetIdptr, _sessionName, *pSessionSettings);

            if (bResult) 
            {
                return true;
            }
        }
    }

    _createSessionCompleted.ExecuteIfBound(NAME_None, false);
    this->OnCreateSessionCompleted.Clear();
    return false;
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

void ANetworkPlayerController::OnCreateSessionCompleted_Internal(FName _sessionName, bool _bWasSuccessful)
{
    if (_bWasSuccessful)
    {

    }
    else
    {

    }

    this->OnCreateSessionCompleted.ExecuteIfBound(_sessionName, _bWasSuccessful);
    this->OnCreateSessionCompleted.Clear();
}
