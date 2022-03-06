//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:RingManager.h
// �T�v				:���x����ɂ���Ring���Ǘ�����}�l�[�W���[�N���X
// �쐬��			:2022/03/06
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/03/06		:�v���C���[���痣�ꂽ�ʒu�ɂ��郊���O��FPS�������鏈���̒ǉ�
//------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingManager.generated.h"

class ARing;
class APlayerDrone;

//�����O��TickLOD�ݒ�̃p�����[�^�[�\����
USTRUCT(BlueprintType)
struct FTickLODSetting
{
	GENERATED_BODY()

public:
	FTickLODSetting()
		: FPS(60)
		, Distance(500.f)
	{}

	FTickLODSetting(const float& NewFPS, const float& NewDistance)
		: FPS(NewFPS)
		, Distance(NewDistance)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FPS;																	//�ݒ肳�ꂽ�͈͓��ɑ��݂��郊���O��FPS����l
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;															//���̋����͈͓̔��ɑ��݂��郊���O��FPS��ݒ肷��
};

UCLASS()
class AIRFORCE_API ARingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARingManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//�z�u���Ɏ��s�����֐�
	virtual void OnConstruction(const FTransform& Transform)override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//�����O�̏�����
	void InitializeRing();
	//�����O�̍X�V
	void UpdateRings();

private:
	UPROPERTY(EditAnywhere)
		APlayerDrone* m_pPlayer;										//�v���C���[
	UPROPERTY(EditAnywhere)
		TArray<ARing*> m_pRings;									//���̃}�l�[�W���[���Ǘ����郊���O
	UPROPERTY(EditAnywhere)
		TArray<FTickLODSetting> m_TickLODSettings;		//�����ɉ����������O��FPS���ς���ݒ荀�ځA�z��͋������߂����Ƀ\�[�g�����
};
