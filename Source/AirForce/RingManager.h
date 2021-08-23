//-----------------------------------------------------------------------------------------------------------------
// �t�@�C����		:RingManager.h
// �T�v				:�����O���Ǘ�����A�N�^�[�N���X
// �쐬��			:2021/08/20
// �쐬��			:19CU0105 �r������
// �X�V���e			:2021/08/20	�r������		:	�Q�[���J�n���ɂ��ׂẴ����O�ɔԍ������蓖�Ă鏈�����쐬
//						:2021/08/20	�r������		:	�����O��������ꂽ��z�񂩂�폜���鏈�����쐬
//-----------------------------------------------------------------------------------------------------------------

//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RingManager.generated.h"

//�O���錾
class ARing;
class ADroneBase;
class AGameManager;

//�f�o�b�Odefine
#define DEBUG_RING_COUNT

UCLASS()
class AIRFORCE_API ARingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	//�R���X�g���N�^
	ARingManager();

protected:
	//�Q�[���J�n����1�x�����Ă΂�鏈��
	virtual void BeginPlay() override;

public:	
	//���t���[���Ă΂�鏈��
	virtual void Tick(float DeltaTime) override;

public:
	//�����O�̍ő�l���擾
	UFUNCTION(BlueprintCallable, Category = "RingManager")
		int GetMaxCount()const { return m_MaxRingCount; }
	//�����O�̎c�萔�擾
	UFUNCTION(BlueprintCallable, Category = "RingManager")
		int GetCount()const { return m_RingCount; }
	
private:
	//�����O��`�悷�邩�ǂ����̔���
	bool IsDraw(const int& _ringIndex) const;
	//�����O���̍X�V
	void UpdateRingInfo();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ARing*> m_pChildRings;			//�����O���i�[����R���e�i
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int m_MaxRingCount;							//�z�u����Ă��郊���O�̍��v��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int m_RingCount;								//�����O�̎c�萔
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int m_RingDrawUpNumber;					//�擾���������O�̉���܂Ń����O��\�����邩���߂�l
	UPROPERTY(EditAnywhere)
		ADroneBase* m_pDrone;					//�h���[���̏��
	UPROPERTY(EditAnywhere)
		AGameManager* m_pGameManager;	//�Q�[���}�l�[�W���[�̏��
};
