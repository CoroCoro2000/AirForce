//-------------------------------------------------------------------------------------
// �t�@�C����	:GameManager.h
// �T�v			:�Q�[���̐i�s���Ǘ�����N���X
// �쐬��		:2021/07/07
// �쐬��		:19CU0104 �r�c�Ĉ�Y
// �X�V���e		:
//-------------------------------------------------------------------------------------

//�C���N���[�h�K�[�h
#pragma once

//�C���N���[�h
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneBase.h"
#include "Ring.h"
#include "GameManager.generated.h"

//�V�[���Ǘ���
UENUM(BlueprintType)
namespace ECURRENTSCENE
{
	enum Type
	{
		SCENE_TITLE = 0		UMETA(DisPlayName = "TITLE"),
		SCENE_FIRST 			UMETA(DisPlayName = "FIRST"),
		SCENE_RESULT			UMETA(DisPlayName = "RESULT"),
	};
}

//�V�[���Ǘ���
UENUM(BlueprintType)
namespace ENEXTSCENE
{
	enum Type
	{
		SCENE_ONCEMORE = 0		UMETA(DisPlayName = "ONCEMORE"),
		SCENE_RASECHANGE			UMETA(DisPlayName = "RASECHANGE"),
		SCENE_EXIT						UMETA(DisPlayName = "EXIT"),
	};
}

USTRUCT(BlueprintType)
struct FNEXTSCENE
{
	GENERATED_USTRUCT_BODY()

public:
	FNEXTSCENE()
	:_NextScene(ENEXTSCENE::SCENE_ONCEMORE)
	{}

	FNEXTSCENE operator++(int) 
	{
		_NextScene= TEnumAsByte<ENEXTSCENE::Type>(_NextScene + 1);
		return *this;
	}

	FNEXTSCENE operator--(int) 
	{ 
		_NextScene = TEnumAsByte<ENEXTSCENE::Type>(_NextScene - 1);
	return *this; 
	}

	FNEXTSCENE operator=(int n)
	{
		_NextScene = TEnumAsByte<ENEXTSCENE::Type>(n);
		return *this;
	}

	bool operator>(int n) { return _NextScene > n ? true : false; }
	bool operator<(int n) { return _NextScene < n ? true : false; }
	bool operator==(int n) { return _NextScene == n ? true : false; }

	TEnumAsByte<ENEXTSCENE::Type> GetNextScene()const { return _NextScene; }
public:
	UPROPERTY(EditAnywhere, Category = "FNEXTSCENE", DisplayName = "NEXTSCENE")
		TEnumAsByte<ENEXTSCENE::Type> _NextScene;
};

UCLASS()
class AIRFORCE_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	//�R���X�g���N�^
	AGameManager();

protected:
	//�Q�[���J�n����1�x��������
	virtual void BeginPlay() override;

public:	
	//���t���[������
	virtual void Tick(float DeltaTime) override;


	void NextSceneUp();
	void NextSceneDown();

	//�|�[�Y�̐ݒ�
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetIsPause() { m_isPause = !m_isPause; }

	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetisSceneTransition(const bool b) { m_isSceneTransition = b; }

	void SetLeftStickInputValue(const float n) { m_LeftStickInputValue = n; }
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsPause() const {return m_isPause; }

	//���[�X�J�n�t���O�̎擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsStart()const { return m_isStart; }

	//�S�[���t���O�̎擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetIsGoal(const bool& _isGoal) { m_isGoal = _isGoal; }

	//�S�[���t���O�̎擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetIsGoal()const { return m_isGoal; }

	//�V�[���J�ڃt���O�̎擾
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetisSceneTransition()const { return m_isSceneTransition; }

	//���[�X�O�̃J�E���g�_�E�����Ԃ̎擾
	UFUNCTION(BlueprintCallable, Category = "Stage")
		float GetCountDownTime()const { return m_CountDownTime; }

	//���b�v�^�C���̕��擾
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapMinute()const { return (int)m_RapTime / 60; }
	//���b�v�^�C���̕b�擾
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapSecond()const { return (int)m_RapTime % 60; }
	//���b�v�^�C���̃~���b�擾
	UFUNCTION(BlueprintCallable, Category = "Stage")
		int GetRapMiliSecond()const { return (m_RapTime - (int)m_RapTime) * 1000; }

	//���[�X��̃V�[���J�ڎ擾
	UFUNCTION(BlueprintCallable, Category = "Stage")
		TEnumAsByte<ENEXTSCENE::Type> GetNextScene()const { return m_NextScene.GetNextScene(); }

private:
	UPROPERTY(EditAnywhere, Category = "Flag")
		TEnumAsByte<ECURRENTSCENE::Type> m_CurrentScene;				//���݂̃V�[���X�e�[�g
	UPROPERTY(EditAnywhere, Category = "Flag")
		FNEXTSCENE m_NextScene;						//���̃V�[���X�e�[�g

	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isPause;								//�|�[�Y�t���O
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isStart;								//���[�X�J�n�t���O
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isGoal;								//�S�[���t���O
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isSceneTransition;					//���[�X��̃V�[���J�ڃt���O

	UPROPERTY(EditAnywhere, Category = "Stage")
		float m_CountDownTime;						//���[�X�O�̃J�E���g�_�E��
	UPROPERTY(VisibleAnywhere, Category = "Stage")
		float m_RapTime;							//�S�[������܂ł̎���

	UPROPERTY(VisibleAnywhere, Category = "Drone")
		ADroneBase* m_Drone;						//�h���[��

	UPROPERTY(VisibleAnywhere, Category = "Drone")
		int m_GoalRingNumber;						//�S�[���̃����O�̒ʂ��ԍ�

	float m_LeftStickInputValue;
};
