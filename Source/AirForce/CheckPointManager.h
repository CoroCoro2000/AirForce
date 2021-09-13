//------------------------------------------------------------------------
// �t�@�C����		:CheckPointManager.h
// �T�v				:�`�F�b�N�|�C���g���Ǘ�����}�l�[�W���[�N���X
// �쐬��			:2021/09/13
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/09/13
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPointManager.generated.h"

class ACheckPoint;

UCLASS()
class AIRFORCE_API ACheckPointManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		TArray<ACheckPoint*> m_pCheckPoints;		//�Ǘ�����`�F�b�N�|�C���g���i�[����z��
	UPROPERTY(VisibleAnywhere)
		int m_PassedCheckpointNum;						//�ʉ߂��ꂽ�`�F�b�N�|�C���g�̐�

};
