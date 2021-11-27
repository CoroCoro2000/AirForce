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
	//ストリーミングのBox判定を取得
	UFUNCTION(BlueprintCallable)
		UBoxComponent* GetStreamingVolume()const { return m_pStreamingVolume; }

private:
	//ロードをする関数
	void LoadStream();
	//アンロードをする関数
	void UnloadStream();
	//ロード完了時に実行する関数
	UFUNCTION()
		void LoadCompleted();

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pStreamingVolume;						//ストリーミングを開始するトリガー
	UPROPERTY(EditAnywhere)
		FName m_LevelName;													//ストリーミングロードするレベルの名前

#if WITH_EDITOR
	float m_LoadStartTime;
	float m_UnloadStartTime;
	bool m_bLoad;
#endif // WITH_EDITOR

};
