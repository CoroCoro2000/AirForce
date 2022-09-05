// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelStreamingVolume.h"
#include "DebugLevelStreamingVolume.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API ADebugLevelStreamingVolume : public ALevelStreamingVolume
{
	GENERATED_BODY()
	
public:
	//コンストラクタ
	ADebugLevelStreamingVolume(const FObjectInitializer& ObjectInitializer);

protected:
	//ゲーム開始時に1度だけ実行される関数
	virtual void BeginPlay()override;

public:
	//毎フレーム実行される関数
	virtual void Tick(float DeltaTime)override;

private:
	virtual void PostLoad() override;

#if WITH_EDITOR
private:
	virtual void CheckForErrors() override;

	void UpdateStreamingLevelsRefs();

	virtual void OnConstruction(const FTransform& Transform)override;
#endif // WITH_EDITOR

	UPROPERTY(EditAnywhere, DisplayName = ShapeColor)
		FColor m_Color;
	UPROPERTY(EditAnywhere, DisplayName = Text)
		FText m_Text;
	class UTextRenderComponent* m_pTextComponent;
};
