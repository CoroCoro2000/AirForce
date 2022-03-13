//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:TickLODActor.h
// �T�v				:�A�N�^�[��TickLOD���x����ύX����
// �쐬��			:2022/03/09
// �쐬��			:19CU0105 �r������
//------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TickLODActor.generated.h"

UCLASS()
class AIRFORCE_API ATickLODActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATickLODActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//Tick���X�V����t���[����ݒ�
	void SetTickFPS(const uint8& NewTickFPS) { m_TickFPS = NewTickFPS; }
	//���݂̍X�V����Ă���FPS���擾
	UFUNCTION(BlueprintCallable)
		uint8 GetTickFPS()const { return m_TickFPS; }

protected:
	UPROPERTY(EditAnywhere, Category = "TickLOD")
		uint8 m_TickFPS;																						//1�b�Ԃ�Tick���X�V�����
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		float m_LastTickTime;																				//�Ō��Tick�����s���ꂽ����
};
