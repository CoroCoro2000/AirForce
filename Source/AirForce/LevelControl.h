// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/LatentActionManager.h"
#include "LevelControl.generated.h"

UCLASS()
class AIRFORCE_API ALevelControl : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void Completed();      // <- これが完了時に呼び出される関数です。上にUFUNCTION()の設定が必ず要ります。

	void LoadLevel(const FName& level);
	void UnloadLevel(const FName& level);
	bool ShowLevel(const FName& level) const;
	bool HideLevel(const FName& level) const;
	bool IsCompleted() const;

private:
	FLatentActionInfo m_LatentAction;
	bool m_Complete;
};
