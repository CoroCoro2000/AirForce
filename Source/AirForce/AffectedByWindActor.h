//--------------------------------------------------------------------------------------------
// �t�@�C����		:AAffectedByWindActor.cpp
// �T�v				:�h���[���̕��̉e�����󂯂�A�N�^�[�N���X
// �쐬��			:2021/08/26
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AffectedByWindActor.generated.h"

class UStaticMeshComponent;

#define WINDSPEED_MIN 1.f
#define WINDSPEED_MAX 5.f

UCLASS()
class AIRFORCE_API AAffectedByWindActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAffectedByWindActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//�󂯂镗�̋�����ݒ肷��֐�
	void SetWindSpeed(const float& _windSpeed);

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pMesh;			//���b�V��
	UPROPERTY(EditAnywhere)
		float m_WindSpeed;
	UPROPERTY(EditAnywhere)
		float m_TempWindSpeed;
};
