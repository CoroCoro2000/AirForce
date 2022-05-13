// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CourseSelect.generated.h"

class UDataTable;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class AIRFORCE_API ACourseSelect : public APawn
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ACourseSelect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//選択中のコース番号取得
	UFUNCTION(BlueprintCallable)
		int GetSelectCourseNumber()const { return m_CourseNumber; }

	//入力可能フラグ設定
	UFUNCTION(BlueprintCallable)
		void SetInputEnable(const bool _bInputEnable) { m_bInputEnable = _bInputEnable; }
	//入力可能フラグ取得
	UFUNCTION(BlueprintCallable)
		bool GetInputEnable() const { return m_bInputEnable; }
	//入力可能フラグ設定
	UFUNCTION(BlueprintCallable)
		void SetCourseSelectCompleted(const bool _bCourseSelectCompletedEnable) { m_bCourseSelectCompleted = _bCourseSelectCompletedEnable; }
	//コース選択終了フラグ取得
	UFUNCTION(BlueprintCallable)
		bool GetCourseSelectCompleted() const { return m_bCourseSelectCompleted; }

	//選択中のコース名取得
	UFUNCTION(BlueprintCallable)
		FName GetSelectCourseName()const;

	//選択中のコースベストタイム取得
	UFUNCTION(BlueprintCallable)
		FString GetSelectCourseBestTime(int _CourseNumber)const;

	//【入力バインド】各スティックの入力
	UFUNCTION(BlueprintCallable)
		void Input_Right();
	UFUNCTION(BlueprintCallable)
		void Input_Left();
	UFUNCTION(BlueprintCallable)
		void Input_Decide();
	UFUNCTION(BlueprintCallable)
		void Input_Cansel();
private:

	//メッシュ初期化
	void InitializeMesh();
	//メッシュの回転
	void UpdateMeshRotation(const float& DeltaTime);
	//メッシュの座標移動
	void UpdateLocation(const float& DeltaTime);
	//コース別ベストタイム読み込み
	void InitializeCourseBestTimeText();

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* m_pDummyComponent;					//3Dミニマップの回転の中心にするダミーオブジェクト
	UPROPERTY(VisibleAnywhere)
		int m_CourseNumber;									//コース番号
	UPROPERTY(VisibleAnywhere)
		int m_CourseTotal;									//コース数
	UPROPERTY(VisibleAnywhere)
		bool m_bInputEnable;								//入力可能フラグ
	UPROPERTY(VisibleAnywhere)
		bool m_bCourseSelectCompleted;						//コース選択終了フラグ
	UPROPERTY(VisibleAnywhere)
		float m_CurrentRotation;
	UPROPERTY(VisibleAnywhere)
		float m_TargetRotation;
	UPROPERTY(EditAnywhere)
		float m_TargetRotationSpeed;						//ダミーオブジェクトの回転速度
	UPROPERTY(EditAnywhere)
		UDataTable* m_pLevelDataTable;						//シーンのデータテーブル
	UPROPERTY(EditAnywhere)
		TArray<UStaticMeshComponent*> m_pMinimapMeshes;		//3Dのミニマップメッシュ
	UPROPERTY(EditAnywhere)
		UStaticMesh* m_pMinimapDefaultMesh;					//3Dのデフォルトミニマップメッシュ
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInterface*> m_pMinimapMaterials;	//3Dのミニマップのマテリアル
	UPROPERTY(EditAnywhere)
		TArray<FString> m_pCourseBestTimeText;				//コース別ベストタイムテキスト
};
