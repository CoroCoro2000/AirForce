
// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_GameInstance.h"
#include "LoadingScreenLibrary.h"
#include "Blueprint/UserWidget.h"
#include "SaveRecord.h"
#include "ConfigParameter.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/AsyncActionHandleSaveGame.h"

const FString URacingD_GameInstance::SaveSlotName = TEXT("GameData");

//コンストラクタ
URacingD_GameInstance::URacingD_GameInstance()
	: m_pLoadingScreenSystem(nullptr)
{

}

//ゲーム開始時に実行する関数
void URacingD_GameInstance::Init()
{
	Super::Init();

	//ローディングシステムの生成
	m_pLoadingScreenSystem = MakeShareable(new FLoadingScreenSystem(this));

	//コンフィグパラメーターの初期化
	CConfigParameter::InitConfigParameter();
}

//ゲーム終了時に実行する関数
void URacingD_GameInstance::Shutdown()
{
	if (m_pLoadingScreenSystem.IsValid())
	{
		m_pLoadingScreenSystem.Reset();
		m_pLoadingScreenSystem = nullptr;
	}

	Super::Shutdown();
}

//ゲームインスタンスの取得
URacingD_GameInstance* URacingD_GameInstance::Get()
{
	URacingD_GameInstance* pGameInstance = nullptr;
	if (GEngine)
	{
		if (const FWorldContext* pContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
		{
			pGameInstance = Cast<URacingD_GameInstance>(pContext->OwningGameInstance);
		}
	}
	return pGameInstance;
}

//ゲームデータのロード
void URacingD_GameInstance::LoadGameData(const FString& SlotName, const int32& SlotIndex)
{
	if(m_pSaveRecord) { return; }

	//セーブデータが存在するか確認
	if(UGameplayStatics::DoesSaveGameExist(SlotName,SlotIndex))
	{
		m_pSaveRecord = Cast<USaveRecord>(UGameplayStatics::LoadGameFromSlot(SlotName,SlotIndex));

#if WITH_EDITOR
		TArray<FStringFormatArg> FormatArray;
		FormatArray.Add(FStringFormatArg(*SlotName));
		FormatArray.Add(FStringFormatArg(SlotIndex));
		const FString log = FString::Format(TEXT("Load Success SlotName[{0}], SlotIndex[{1}]"),FormatArray);
		UKismetSystemLibrary::PrintString(this,log,true,false);
#endif
	}
	//セーブデータがない場合は新しく作成
	else
	{
		m_pSaveRecord = Cast<USaveRecord>(UGameplayStatics::CreateSaveGameObject(USaveRecord::StaticClass()));

#if WITH_EDITOR
		TArray<FStringFormatArg> FormatArray;
		FormatArray.Add(FStringFormatArg(*SlotName));
		FormatArray.Add(FStringFormatArg(SlotIndex));
		const FString log = FString::Format(TEXT("Save data not found. SlotName[{0}], SlotIndex[{1}]"),FormatArray);
		UKismetSystemLibrary::PrintString(this,log,true,false,FLinearColor::Red);
		UKismetSystemLibrary::PrintString(this,TEXT("New Save data Create."),true,false);
#endif
	}
}

//ゲームデータの非同期ロード(C++)
void URacingD_GameInstance::AsyncLoadGameData(const FString& SlotName, const int32 SlotIndex, FAsyncLoadGameFromSlotDelegate Callback = FAsyncLoadGameFromSlotDelegate())
{
	UGameplayStatics::AsyncLoadGameFromSlot(SlotName,SlotIndex,Callback);
}

//ゲームデータの非同期ロード(C++)
void URacingD_GameInstance::AsyncLoadGameData(const FString& SlotName, const int32 SlotIndex)
{
	auto Callback = FAsyncLoadGameFromSlotDelegate::CreateLambda([this](const FString& LoadedSlotName, const int32 LoadedSlotIndex, USaveGame* LoadedData)
	{
		this->LoadCompleted(LoadedSlotName,LoadedSlotIndex,LoadedData);
	});
	UGameplayStatics::AsyncLoadGameFromSlot(SlotName,SlotIndex,Callback);
}

//ゲームデータのセーブ
void URacingD_GameInstance::SaveGameData(const FString& SlotName, const int32& SlotIndex)const
{
	if(!m_pSaveRecord) { return; }

	const bool isSuccess = UGameplayStatics::SaveGameToSlot(m_pSaveRecord,SlotName,SlotIndex);
	
#if WITH_EDITOR
	TArray<FStringFormatArg> FormatArray;
	FormatArray.Add(FStringFormatArg(*SlotName));
	FormatArray.Add(FStringFormatArg(SlotIndex));
	const FString log = isSuccess ?
		FString::Format(TEXT("Save Success SlotName[{0}], SlotIndex[{1}]"),FormatArray):
		FString::Format(TEXT("Save Failed SlotName[{0}], SlotIndex[{1}]"),FormatArray);
	
	UKismetSystemLibrary::PrintString(this,log,true,false);
#endif
}

//ゲームデータの非同期セーブ(C++)
void URacingD_GameInstance::AsyncSaveGameData(const FString& SlotName, const int32 SlotIndex, FAsyncSaveGameToSlotDelegate Callback = FAsyncSaveGameToSlotDelegate())
{
	UGameplayStatics::AsyncSaveGameToSlot(m_pSaveRecord,SlotName,SlotIndex,Callback);
}

//ゲームデータの非同期セーブ(C++)
void URacingD_GameInstance::AsyncSaveGameData(const FString& SlotName, const int32 SlotIndex)
{
	auto Callback = FAsyncSaveGameToSlotDelegate::CreateLambda([this](const FString& SavedSlotName, const int32 SavedSlotIndex, bool bSuccess)
	{
		SaveCompleted(SavedSlotName,SavedSlotIndex,bSuccess);
	});
	UGameplayStatics::AsyncSaveGameToSlot(m_pSaveRecord,SlotName,SlotIndex,Callback);
}

//非同期ロード完了時に呼ばれるコールバック
void URacingD_GameInstance::LoadCompleted(const FString& SlotName, const int32 SlotIndex, USaveGame* LoadData)
{
	//ロードに成功したらセーブデータを保持
	if(LoadData)
	{
		m_pSaveRecord = Cast<USaveRecord>(LoadData);

#if WITH_EDITOR
		TArray<FStringFormatArg> FormatArray;
		FormatArray.Add(FStringFormatArg(*SlotName));
		FormatArray.Add(FStringFormatArg(SlotIndex));
		const FString log = FString::Format(TEXT("AsyncLoad Success SlotName[{0}], SlotIndex[{1}]"),FormatArray);
		UKismetSystemLibrary::PrintString(this,log,true,false);
#endif 
	}
	//ロードに失敗したら新しいデータを作成
	else
	{
		m_pSaveRecord = Cast<USaveRecord>(UGameplayStatics::CreateSaveGameObject(USaveRecord::StaticClass()));

#if WITH_EDITOR
		TArray<FStringFormatArg> FormatArray;
		FormatArray.Add(FStringFormatArg(*SlotName));
		FormatArray.Add(FStringFormatArg(SlotIndex));
		const FString log = FString::Format(TEXT("AsyncLoad Failed SlotName[{0}], SlotIndex[{1}]"),FormatArray);
		UKismetSystemLibrary::PrintString(this,log,true,false,FLinearColor::Red);
#endif 
	}
}

//非同期セーブ完了時に呼ばれるコールバック
void URacingD_GameInstance::SaveCompleted(const FString& SavedSlotName, const int32 SavedSlotIndex, bool bSuccess)
{
	//ログ出力
	if(bSuccess)
	{
#if WITH_EDITOR
		TArray<FStringFormatArg> FormatArray;
		FormatArray.Add(FStringFormatArg(*SavedSlotName));
		FormatArray.Add(FStringFormatArg(SavedSlotIndex));
		const FString log = FString::Format(TEXT("AsyncSave Success SlotName[{0}], SlotIndex[{1}]"),FormatArray);
	
		UKismetSystemLibrary::PrintString(this,log,true,false);
#endif 
	}
	else
	{
#if WITH_EDITOR
		TArray<FStringFormatArg> FormatArray;
		FormatArray.Add(FStringFormatArg(*SavedSlotName));
		FormatArray.Add(FStringFormatArg(SavedSlotIndex));
		const FString log = FString::Format(TEXT("AsyncSave Failed SlotName[{0}], SlotIndex[{1}]"),FormatArray);
	
		UKismetSystemLibrary::PrintString(this,log,true,false,FLinearColor::Red);
#endif 
	}
}

//ゲームデータの保存名取得
FString URacingD_GameInstance::GetSaveSlotName()
{
	return URacingD_GameInstance::SaveSlotName;
}