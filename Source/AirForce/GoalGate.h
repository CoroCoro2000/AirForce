// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoalGate.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AGameManager;

UCLASS()
class AIRFORCE_API AGoalGate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoalGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//�I�[�o�[���b�v���ɌĂ΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pGateMash;								//�Q�[�g�̃��b�V��
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pGoalCollision;									//�S�[���̓����蔻��
	UPROPERTY(EditAnywhere)
		AGameManager* m_pGameManager;
};
