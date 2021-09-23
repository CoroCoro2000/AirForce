// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

namespace EINPUTKEY
{
	enum Type
	{
		EINPUTKEY_VERTICAL = 0		UMETA(DisPlayName = "VERTICAL"),		//上下入力
		EINPUTKEY_HORIZONTAL 		UMETA(DisPlayName = "HORIZONTAL"),	//左右入力
	};
}

UCLASS()
class AIRFORCE_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//【入力バインド】コントローラー入力設定
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputUp();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputDown();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputLeft();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputRight();
	UFUNCTION(BlueprintCallable, Category = "Input")
		void InputA();

	//入力可能フラグ設定
	UFUNCTION(BlueprintCallable, Category = "Flag")
		void SetInputEnable(bool _flag) { m_isInputEnable = _flag; }
	//入力可能フラグ取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		bool GetInputEnable() const { return m_isInputEnable; }

	//現在選択したレベル番号取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		int GetLevelIndex() const { return m_LevelIndex; }

	//現在選択したレベル名取得
	UFUNCTION(BlueprintCallable, Category = "Flag")
		FName GetLevelName() const { return m_LevelName[m_LevelIndex]; }
private:
	UPROPERTY(VisibleAnywhere, Category = "Flag")
		bool m_isInputEnable;						//入力可能フラグ

	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<FName> m_LevelName;					//遷移するレベル名
	UPROPERTY(VisibleAnywhere, Category = "Level")
		int m_LevelIndex;							//現在選択しているレベル番号
	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<int> m_LevelAddValue;				//各軸でレベル番号を加算する量
	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<int> m_LevelIndexMin;				//各軸でレベル番号を加算する下限
	UPROPERTY(EditAnywhere, Category = "Level")
		TArray<int> m_LevelIndexMax;				//各軸でレベル番号を加算する上限
};
