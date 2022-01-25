// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterBox.generated.h"

class UStaticMeshComponent;

UCLASS()
class AIRFORCE_API AWaterBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform)override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void InitializeMesh();

private:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pSurfaceMesh;								//����(�\)�̃��b�V��
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pBackfaceMesh;							//����(��)�̃��b�V��
	UPROPERTY(EditAnywhere)
		FVector2D m_Size;																	//�傫��
	UPROPERTY(EditAnywhere)
		float m_BoundaryThickness;													//���ʂ̋��E���̌���
};
