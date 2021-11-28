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
	//~ Begin UObject Interface.
	virtual void PostLoad() override;
	//~ End UObject Interface.


#if WITH_EDITOR
private:
	//~ Begin AActor Interface.
	virtual void CheckForErrors() override;
	//~ End AActor Interface.

	/** Updates list of streaming levels that are referenced by this streaming volume */
	void UpdateStreamingLevelsRefs();

	virtual void OnConstruction(const FTransform& Transform)override;

	UPROPERTY(EditAnywhere, DisplayName = ShapeColor)
		FColor m_Color;
	UPROPERTY(EditAnywhere, DisplayName = Text)
		FText m_Text;
	class UTextRenderComponent* m_pTextComponent;
#endif // WITH_EDITOR
};
