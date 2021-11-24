// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelStreamingVolumeActor.generated.h"

//前方宣言
class UBoxComponent;

UCLASS()
class AIRFORCE_API ALevelStreamingVolumeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamingVolumeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//オーバーラップした瞬間呼び出される関数
	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//オーバーラップしていたコンポーネントが離れた瞬間呼び出される関数
	UFUNCTION()
		virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

private:
	//ストリーミングロードに利用するUUIDを取得
	int32 GetStreamingLevelNum()const;
	//サブレベルの表示をする関数
	UFUNCTION()
		void ShowLevel();
	//サブレベルを非表示にする関数
	UFUNCTION()
		void HideLevel();

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pStreamingVolume;						//ストリーミングを開始するトリガー
	UPROPERTY(EditAnywhere)
		TArray<FName> m_LoadLevelNames;								//ロード、表示するレベルの名前
	UPROPERTY(EditAnywhere)
		FName m_UnloadLevelName;											//アンロード、非表示にするレベルの名前
	UPROPERTY(VisibleAnywhere)
		int m_LoadIndex;															//ロードするマップの番号
};
