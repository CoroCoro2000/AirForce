﻿
#include "NetworkPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

const FName ANetworkPlayerController::SESSION_NAME = TEXT("SessionName");

ANetworkPlayerController::ANetworkPlayerController()
    : m_pSearchSettings(nullptr)
    , OnLoginCompleted()
    , OnCreateSessionCompleted()
    , OnFindSessionCompleted()
    , OnJoinSessionCompleted()
    , OnKillSessionCompleted()
{

}

void ANetworkPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (m_pSearchSettings.IsValid())
    {
        m_pSearchSettings.Reset();
    }
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

bool ANetworkPlayerController::CreateSession(const int32 _connection,const FString _searchKeyword, const FCreateSessionCompleted& _createSessionCompleted)
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
            bool bResult = Sessions->CreateSession(*pUniqueNetIdptr, ANetworkPlayerController::SESSION_NAME, *pSessionSettings);

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

bool ANetworkPlayerController::FindSession(const FString _searchKeyword, const int32 _maxSearchResults, const FFindSessionCompleted& _findSessionCompleted)
{
    if (IOnlineSubsystem* const pOnlineSubsystem = Online::GetSubsystem(GetWorld()))
    {
        IOnlineSessionPtr pSessions = pOnlineSubsystem->GetSessionInterface();
        if (pSessions.IsValid())
        {
            m_pSearchSettings = MakeShareable(new FOnlineSessionSearch());

            m_pSearchSettings->MaxSearchResults = _maxSearchResults;
            m_pSearchSettings->PingBucketSize = 50;
            m_pSearchSettings->bIsLanQuery = false;

            m_pSearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
            m_pSearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
            if (!_searchKeyword.IsEmpty())
            {
                m_pSearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, _searchKeyword, EOnlineComparisonOp::Near);
            }

            //検索完了時のデリゲートを登録
            OnFindSessionCompleted = _findSessionCompleted;
            pSessions->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ANetworkPlayerController::OnFindSessionsCompleted_Internal));

            TSharedRef<FOnlineSessionSearch> SearchSettingsRef = m_pSearchSettings.ToSharedRef();
            TSharedPtr<const FUniqueNetId> UniqueNetIdptr = this->GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
            if (UniqueNetIdptr != nullptr)
            {
                if (pSessions->FindSessions(*UniqueNetIdptr, SearchSettingsRef))
                {
                    return true;
                }
            }
        }
    }

    _findSessionCompleted.ExecuteIfBound(TArray<FBlueprintSessionResult>(), false);
    this->OnFindSessionCompleted.Clear();
    return false;
}

bool ANetworkPlayerController::JoinSession(const FBlueprintSessionResult& _searchResult, const FJoinSessionCompleted& _joinSessionCompleted)
{
    if (IOnlineSubsystem* const pOnlineSubsystem = Online::GetSubsystem(GetWorld()))
    {
        IOnlineSessionPtr pSessions = pOnlineSubsystem->GetSessionInterface();
        if (pSessions.IsValid())
        {
            if (_searchResult.OnlineResult.IsValid()) 
            {
                pSessions->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ANetworkPlayerController::OnJoinSessionCompleted_Internal));

                TSharedPtr<const FUniqueNetId> UniqueNetIdptr = GetLocalPlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
                this->OnJoinSessionCompleted = _joinSessionCompleted;
                if (pSessions->JoinSession(*UniqueNetIdptr, ANetworkPlayerController::SESSION_NAME, _searchResult.OnlineResult))
                {
                    return true;
                }
            }
        }
    }

    _joinSessionCompleted.ExecuteIfBound(ANetworkPlayerController::SESSION_NAME, EJoinSessionState::UnknownError);
    this->OnJoinSessionCompleted.Clear();
    return false;
}

bool ANetworkPlayerController::KillSession(const FKillSessionCompleted& _killSessionCompleted)
{
    if (IOnlineSubsystem* const pOnlineSubsystem = Online::GetSubsystem(GetWorld()))
    {
        IOnlineSessionPtr pSessions = pOnlineSubsystem->GetSessionInterface();
        if (pSessions.IsValid())
        {
            this->OnKillSessionCompleted = _killSessionCompleted;
            if (pSessions->DestroySession(ANetworkPlayerController::SESSION_NAME, FOnDestroySessionCompleteDelegate::CreateUObject(this, &ANetworkPlayerController::OnKillSessionCompleted_Internal)))
            {
                return true;
            }
        }
    }

    _killSessionCompleted.ExecuteIfBound(NAME_None, false);
    OnKillSessionCompleted.Clear();
    return false;
}

EServerRole ANetworkPlayerController::GetServerRole_Implementation() const
{
    return IRacingD_Session::Execute_GetServerRole(PlayerState);
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
            if (pLocalPlayer != nullptr)
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

void ANetworkPlayerController::OnFindSessionsCompleted_Internal(bool _bWasSuccessful)
{
    if (_bWasSuccessful)
    {
        //検索に引っかかったセッションがある
        if (m_pSearchSettings->SearchResults.Num() > 0)
        {
            TArray<FBlueprintSessionResult> results;
            for (const auto& result : m_pSearchSettings->SearchResults)
            {
                FBlueprintSessionResult val;
                val.OnlineResult = result;
                results.Add(val);
            }
            this->OnFindSessionCompleted.ExecuteIfBound(results, true);
            this->OnFindSessionCompleted.Clear();
            return;
        }
        //セッション1つもない
        else
        {

        }
    }
    //検索に失敗
    else
    {

    }

    this->OnFindSessionCompleted.ExecuteIfBound(TArray<FBlueprintSessionResult>(), false);
    this->OnFindSessionCompleted.Clear();
}

void ANetworkPlayerController::OnJoinSessionCompleted_Internal(FName _sessionName, EOnJoinSessionCompleteResult::Type _joinState)
{
    this->OnJoinSessionCompleted.ExecuteIfBound(_sessionName, static_cast<EJoinSessionState>(_joinState));
    this->OnJoinSessionCompleted.Clear();
}

void ANetworkPlayerController::OnKillSessionCompleted_Internal(FName _sessionName, bool _bWasSuccessful)
{
    if (_bWasSuccessful)
    {

    }
    else
    {

    }

    this->OnKillSessionCompleted.ExecuteIfBound(_sessionName, _bWasSuccessful);
}
