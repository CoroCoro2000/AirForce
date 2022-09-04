#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

/**
 * @brief ���O�C���������̃f���Q�[�g
 * @param  Success ���O�C���ɐ���������
*/
DECLARE_DYNAMIC_DELEGATE_OneParam(FLoginCompleted, bool, Success);

UCLASS()
class AIRFORCE_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//�R���X�g���N�^
	ANetworkPlayerController();

public:
	UFUNCTION(BlueprintPure, Category = "Racing-D|Network", meta = (WorldContext = WorldContextObject))
	/**
	 * @brief �v���C���[�R���g���[���[�擾
	 * @return 
	*/
	static ANetworkPlayerController* GetNetworkPlayerController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Racing-D|Network")
	/**
	 * @brief EOS�Ƀ��O�C��
	 * @param _loginCompleted ���O�C����̃f���Q�[�g
	*/
	void LoginEOS(const FLoginCompleted& _loginCompleted);

private:
	/**
	 * @brief ���O�C����̓�������
	 * @param _localUserNum 
	 * @param _bWasSuccessful 
	 * @param _userId 
	 * @param _error 
	*/
	void OnLoginCompleted_Internal(int32 _localUserNum, bool _bWasSuccessful, const FUniqueNetId& _userId, const FString& _error);

private:
	UPROPERTY()
	/**
	 * @brief ���O�C���������̃f���Q�[�g
	*/
	FLoginCompleted OnLoginCompleted;
};
