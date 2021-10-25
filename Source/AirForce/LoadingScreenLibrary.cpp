// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenLibrary.h"
#include "RacingD_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SWidget.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Framework/Application/SlateApplication.h"
#include "MoviePlayer.h"
#include "UnrealEngine.h"
#include "Engine/Engine.h"
#include "Engine/LevelStreaming.h"
#include "Engine/GameViewportClient.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

//�R���X�g���N�^
FLoadingScreenSystem::FLoadingScreenSystem(URacingD_GameInstance* InGameInstance)
	: m_pGameInstance(InGameInstance)
	, m_pLoadingScreenWidget()
	, m_LastTickTime(0.0)
	, m_PackageName(TEXT(""))
	, m_Progress(0.f)
{
	//���[�h���ɌĂяo���֐���o�^
	FCoreDelegates::OnAsyncLoadingFlushUpdate.AddRaw(this, &FLoadingScreenSystem::OnAsyncLoadingFlushUpdate);
}

//�f�X�g���N�^
FLoadingScreenSystem::~FLoadingScreenSystem()
{

}

//���t���[�����s�����֐�
void FLoadingScreenSystem::Tick(float DeltaTime)
{

}

//���[�h�̐i�����擾����֐�
float FLoadingScreenSystem::GetLoadingProgress()
{
	float Sum = 0.f;
	int32 PackageNum = 1;
	UPackage* PersistentLevelPackage = FindObjectFast<UPackage>(NULL, m_PackageName, true, true);
	if (PersistentLevelPackage && (PersistentLevelPackage->GetLoadTime() > 0))
	{
		Sum += 100.0f;
	}
	else
	{
		const float LevelLoadPercentage = GetAsyncLoadPercentage(m_PackageName);
		if (LevelLoadPercentage >= 0.0f)
		{
			Sum += LevelLoadPercentage;
		}
	}

	if (PersistentLevelPackage)
	{
		UWorld* World = UWorld::FindWorldInPackage(PersistentLevelPackage);
		TArray<FName>	PackageNames;
		PackageNames.Reserve(World->GetStreamingLevels().Num());
		for (ULevelStreaming* LevelStreaming : World->GetStreamingLevels())
		{
			if (LevelStreaming
				&& !LevelStreaming->GetWorldAsset().IsNull()
				&& LevelStreaming->GetWorldAsset() != World)
			{
				PackageNames.Add(LevelStreaming->GetWorldAssetPackageFName());
			}
		}
		for (FName& LevelName : PackageNames)
		{
			PackageNum++;
			UPackage* LevelPackage = FindObjectFast<UPackage>(NULL, LevelName);

			if (LevelPackage && (LevelPackage->GetLoadTime() > 0))
			{
				Sum += 100.0f;
			}
			else
			{
				const float LevelLoadPercentage = GetAsyncLoadPercentage(LevelName);
				if (LevelLoadPercentage >= 0.0f)
				{
					Sum += 100.0f;
				}
			}
		}
	}

	float Current = Sum / PackageNum;
	m_Progress = Current * 0.05f + m_Progress * 0.95f;
	return m_Progress;
}

//���[�h�J�n���ɌĂяo�����֐�
void FLoadingScreenSystem::OnBeginLoadingScreen(const FString& MapName)
{
	//���[�h���ɕ\�������ʂ̐ݒ�
	FLoadingScreenAttributes LoadingScreen;

	if (m_pGameInstance)
	{
		UUserWidget* LoadingWidget = CreateWidget(m_pGameInstance, m_pGameInstance->GetLoadingUMGClass());
		if (LoadingWidget)
		{
			m_pLoadingScreenWidget = LoadingWidget->TakeWidget();
			LoadingScreen.WidgetLoadingScreen = m_pLoadingScreenWidget;
			LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
			LoadingScreen.bAllowEngineTick = true;
		}

		//���[�r�[�v���C���[��Ń��[�h��ʂ�\������
		IGameMoviePlayer* pGameMoviePlayer = GetMoviePlayer();
		if (pGameMoviePlayer)
		{
			m_bLoading = true;
			m_Progress = 0.0f;
			pGameMoviePlayer->SetupLoadingScreen(LoadingScreen);
		}

		//���[�h����3D�`���؂�
		if (m_pGameInstance->GetGameViewportClient())
		{
			m_pGameInstance->GetGameViewportClient()->bDisableWorldRendering = true;
		}

	}
}

//���[�h�I�����ɌĂяo�����֐�
void FLoadingScreenSystem::OnEndLoadingScreen(UWorld* world)
{
	if (m_pGameInstance)
	{
		//���[�h��ʂ������O��3D�`���ON�ɂ���
		if (m_pGameInstance->GetGameViewportClient())
		{
			m_pGameInstance->GetGameViewportClient()->bDisableWorldRendering = false;
		}
	}

	//���[�h��ʂ̍Đ����~����
	IGameMoviePlayer* pGameMoviePlayer = GetMoviePlayer();
	if (pGameMoviePlayer)
	{
		pGameMoviePlayer->StopMovie();
		m_bLoading = false;
	}
}

/*���̃f���Q�[�g�֐��̓��[�h���ɍ��p�x�ŌĂ΂��̂ŁA�K�؂ȊԊu�ŃX���[�g�̍X�V���ĂԂ悤�ɂ���*/
void FLoadingScreenSystem::OnAsyncLoadingFlushUpdate()
{
	check(IsInGameThread());

	QUICK_SCOPE_CYCLE_COUNTER(STAT_LoadingScreenManager_OnAsyncLoadingFlushUpdate);

	const double CurrentTime = FPlatformTime::Seconds();
	const double DeltaTime = CurrentTime - m_LastTickTime;
	if (DeltaTime > 1.0f / 60.0f)
	{
		m_LastTickTime = CurrentTime;
		if (m_bLoading) {
			// �X���[�g�X�V
			FSlateApplication::Get().Tick();

			{
				TGuardValue<int32> DisableAsyncLoadDuringSync(GDoAsyncLoadingWhileWaitingForVSync, 0);
				FSlateApplication::Get().GetRenderer()->Sync();
			}
		}

		m_LastTickTime = CurrentTime;
	}
}

//�R���X�g���N�^
ULoadingScreenLibrary::ULoadingScreenLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ULoadingScreenLibrary::SetTargetPackageForLoadingProgress(const UObject* WorldContextObject, FName InPackageName)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) { return; }
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
	if (GameInstance == nullptr) { return; }
	if (GameInstance->GetLoadingScreenSystem().IsValid() == false) { return; }
	return GameInstance->GetLoadingScreenSystem()->SetPackageNameForLoadingProgress(InPackageName);
}

float ULoadingScreenLibrary::GetLoadingProgress(const UObject* WorldContextObject)
{
	URacingD_GameInstance* GameInstance = Cast<URacingD_GameInstance>(WorldContextObject->GetOuter());
	if (GameInstance == nullptr)	//Try to get a gameinstance through a world.
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World == nullptr) { return 0.f; }
		GameInstance = Cast<URacingD_GameInstance>(World->GetGameInstance());
		if (GameInstance == nullptr) { return 0.f; }
	}
	if (GameInstance->GetLoadingScreenSystem().IsValid() == false) { return 0.f; }
	return GameInstance->GetLoadingScreenSystem()->GetLoadingProgress();
}

//���x���̃��[�h��ɊJ���֐�
void ULoadingScreenLibrary::LoadAndOpenLevel(const UObject* WorldContextObject, const FName PersistentLevelName, const bool bAbsolute, const FString Options)
{
	//���[�h���郌�x������ݒ�
	ULoadingScreenLibrary::SetTargetPackageForLoadingProgress(WorldContextObject, PersistentLevelName);

	//���x�����J��
	UGameplayStatics::OpenLevel(WorldContextObject, PersistentLevelName, bAbsolute, Options);
}