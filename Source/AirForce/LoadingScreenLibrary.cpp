// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenLibrary.h"
#include "RacingD_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SWidget.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Framework/Application/SlateApplication.h"
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
	, m_AsyncLoadingFlushUpdateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
{
	//���[�h���̃X���[�g�X�V������֐����o�C���h
	m_AsyncLoadingFlushUpdateHandle = FCoreDelegates::OnAsyncLoadingFlushUpdate.AddRaw(this, &FLoadingScreenSystem::OnAsyncLoadingFlushUpdate);
}

//�f�X�g���N�^
FLoadingScreenSystem::~FLoadingScreenSystem()
{
	//���[�h���̃X���[�g�X�V������֐��̃o�C���h������
	if (m_AsyncLoadingFlushUpdateHandle.IsValid())
	{
		FCoreDelegates::OnAsyncLoadingFlushUpdate.Remove(m_AsyncLoadingFlushUpdateHandle);
	}
}

//���t���[�����s�����֐�
void FLoadingScreenSystem::Tick(float DeltaTime)
{

}

//���[�h�E�B�W�F�b�g�̕\��
void FLoadingScreenSystem::ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass, const FName InPackageName)
{
	if (m_bShowing) { return; }
	if (!m_pGameInstance) { return; }

	if (UUserWidget* pLoadingWidget = CreateWidget(m_pGameInstance, WidgetClass))
	{
		m_pLoadingScreenWidget = pLoadingWidget->TakeWidget();
		if (m_pLoadingScreenWidget.IsValid())
		{
			if (UGameViewportClient* pGameViewportClient = m_pGameInstance->GetGameViewportClient())
			{
				//���[�h����p�b�P�[�W����\��
				m_PackageName = InPackageName;
				//���[�h��ʂ�\��
				const int32 ZOrder = 10000;
				pGameViewportClient->AddViewportWidgetContent(m_pLoadingScreenWidget.ToSharedRef(), ZOrder);

				// Widget���\������Ă���͂��Ȃ̂�3D�`������S�ɃJ�b�g����
				pGameViewportClient->bDisableWorldRendering = true;
				if (!GIsEditor)
				{
					FSlateApplication::Get().Tick();
				}
				//�\����Ԃɐ؂�ւ���
				m_bShowing = true;
			}
		}
	}
}

//���[�h�E�B�W�F�b�g�̔�\��
void FLoadingScreenSystem::HideLoadingScreen()
{
	if (!m_bShowing) { return; }
	if (!GEngine) { return; }
	if (!m_pGameInstance) { return; }
	if (!m_pLoadingScreenWidget.IsValid()) { return; }

	if (UGameViewportClient* pGameViewportClient = m_pGameInstance->GetGameViewportClient())
	{
		//3D�`���ON�ɂ���
		pGameViewportClient->bDisableWorldRendering = false;
		//���[�h��ʂ��\���ɂ���
		pGameViewportClient->RemoveViewportWidgetContent(m_pLoadingScreenWidget.ToSharedRef());
		//�E�B�W�F�b�g�̃|�C���^��������邽�߂�GC���Ăяo��
		GEngine->ForceGarbageCollection(true);
		//��\����Ԃɐ؂�ւ���
		m_bShowing = false;
	}
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
		if (m_bShowing) 
		{
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

//���[�h��ʂ̕\��
void ULoadingScreenLibrary::ShowLoadingScreen(const TSubclassOf<UUserWidget> WidgetClass, FName InPackageName)
{
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		if (TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem = pGameInstance->GetLoadingScreenSystem())
		{
			pLoadingScreenSystem->ShowLoadingScreen(WidgetClass, InPackageName);
		}
	}
}

//���[�h��ʂ̔�\��
void ULoadingScreenLibrary::HideLoadingScreen()
{
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		if (TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem = pGameInstance->GetLoadingScreenSystem())
		{
			pLoadingScreenSystem->HideLoadingScreen();
		}
	}
}

float ULoadingScreenLibrary::GetLoadingProgress()
{
	float progress = 0.f;
	if (URacingD_GameInstance* pGameInstance = URacingD_GameInstance::Get())
	{
		if (TSharedPtr<FLoadingScreenSystem> pLoadingScreenSystem = pGameInstance->GetLoadingScreenSystem())
		{
			progress = pLoadingScreenSystem->GetLoadingProgress();
		}
	}
	return progress;
}