//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:TickLODActor.h
// �T�v				:1�t���[���������Tick�X�V�񐔂�ύX�ł���A�N�^�[�̃x�[�X�N���X
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
	//FPS��ݒ�
	void SetTickFPS(const float& NewTickFPS) { m_TickFPS = NewTickFPS; }
	//FPS���擾
	UFUNCTION(BlueprintCallable)
		float GetTickFPS()const { return m_TickFPS; }
	//LOD�A�N�^�[�̔ԍ���ݒ�
	void SetNumber(const uint32& Number) { m_ActorNumber = Number; }

protected:
	//�����\�ȃt���[��������
	bool IsProcessableFrame()const;

protected:
	UPROPERTY(EditAnywhere, Category = "TickLOD")
		float m_TickFPS;																						//FPS
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		float m_LastTickTime;																				//�Ō��Tick�����s���ꂽ����
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		uint32 m_ActorNumber;																				//�ԍ�
	UPROPERTY(VisibleAnywhere, Category = "TickLOD")
		uint32 m_FrameCount;																				//�����t���[����
};
