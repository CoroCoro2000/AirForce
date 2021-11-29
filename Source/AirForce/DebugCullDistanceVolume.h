// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CullDistanceVolume.h"
#include "DebugCullDistanceVolume.generated.h"

/**
 * 
 */
UCLASS()
class AIRFORCE_API ADebugCullDistanceVolume : public ACullDistanceVolume
{
    GENERATED_BODY()

public:
    ADebugCullDistanceVolume(const FObjectInitializer& ObjectInitializer);

protected:
    //ゲーム開始時に1度だけ実行される関数
    virtual void BeginPlay()override;

public:
    //毎フレーム実行される関数
    virtual void Tick(float DeltaTime)override;

#if WITH_EDITOR
public:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditMove(bool bFinished) override;
    virtual void Destroyed() override;

private:
    virtual void OnConstruction(const FTransform& Transform)override;
#endif // WITH_EDITOR

    UPROPERTY(EditAnywhere, DisplayName = ShapeColor)
        FColor m_Color;
    UPROPERTY(EditAnywhere, DisplayName = Text)
        FText m_Text;
    class UTextRenderComponent* m_pTextComponent;
};
