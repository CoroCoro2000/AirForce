// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

class USplineComponent;

UCLASS()
class AIRFORCE_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//スプラインを取得
	UFUNCTION(BlueprintCallable)
		USplineComponent* GetSpline()const { return m_pSpline; }
	//進んだ距離からスプラインの座標を取得
	UFUNCTION(BlueprintCallable)
		FVector GetCurrentLocation(const float& length, const bool& bLoop)const;
	//進んだ距離からスプラインの回転を取得
	UFUNCTION(BlueprintCallable)
		FRotator GetCurrentRotation(const float& length, const bool& bLoop)const;


private:
	UPROPERTY(EditAnywhere)
		USplineComponent* m_pSpline;
};
