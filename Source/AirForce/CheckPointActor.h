//--------------------------------------------------------------------------------------------
// �t�@�C����		:CheckPointActor.h
// �T�v				:���[�X���[�h���̃`�F�b�N�|�C���g�Ƃ��Ĉ����A�N�^�[
// �쐬��			:2021/08/27
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//--------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPointActor.generated.h"


class UBoxComponent;

UCLASS()
class AIRFORCE_API ACheckPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�I�u�W�F�N�g���I�[�o�[���b�v�������Ă΂��C�x���g�֐���o�^
	UFUNCTION()
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	//���̃`�F�b�N�|�C���g���擾
	ACheckPointActor* GetNextCheckPoint()const { return m_pNextCheckPointActor; }

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pCheckPointCollision;
	UPROPERTY(EditAnywhere)
		ACheckPointActor* m_pNextCheckPointActor;		//���̃`�F�b�N�|�C���g
};
