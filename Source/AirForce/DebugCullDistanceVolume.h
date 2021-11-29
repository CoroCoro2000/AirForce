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
    //�Q�[���J�n����1�x�������s�����֐�
    virtual void BeginPlay()override;

public:
    //���t���[�����s�����֐�
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
