//------------------------------------------------------------------------------------------------------------------------------------------------
// �t�@�C����		:TickLODManager.h
// �T�v				:�A�N�^�[��TickLOD���x�����Ǘ�����}�l�[�W���[�N���X
// �쐬��			:2022/03/09
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/03/09		:�v���C���[���痣�ꂽ�ʒu�ɂ��郊���O��FPS�������鏈���̒ǉ�
//------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TickLODManager.generated.h"

class APlayerDrone;
class ATickLODActor;

//�A�N�^�[��TickLOD�ݒ�̃p�����[�^�[�\����
USTRUCT(BlueprintType)
struct FTickLODSetting
{
	GENERATED_BODY()

public:
	FTickLODSetting()
		: FPS(60)
		, Distance(1500.f)
	{}

	FTickLODSetting(const uint8& NewFPS, const float& NewDistance)
		: FPS(NewFPS)
		, Distance(NewDistance)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 FPS;																	//�ݒ肳�ꂽ�͈͓��ɑ��݂���A�N�^�[��FPS����l
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;															//���̋����͈͓̔��ɑ��݂���A�N�^�[��FPS��ݒ肷��
};

UCLASS()
class AIRFORCE_API ATickLODManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATickLODManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//�z�u���Ɏ��s�����֐�
	virtual void OnConstruction(const FTransform& Transform)override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//LOD�̃A�N�^�[�z��̏�����
	void InitializeActorArray();
	//LOD�̍X�V
	void UpdateLOD();

private:
	UPROPERTY(EditAnywhere)
		TArray<FTickLODSetting> m_TickLODSettings;								//�����ɉ����������O��FPS���ς���ݒ荀�ځA�z��͋������߂����Ƀ\�[�g�����
	UPROPERTY(EditAnywhere)
		APlayerDrone* m_pPlayer;																//�v���C���[
	UPROPERTY(EditAnywhere)
		TArray<ATickLODActor*> m_pTickLODActors;										//LOD�̐ݒ������A�N�^�[���i�[����z��
};
