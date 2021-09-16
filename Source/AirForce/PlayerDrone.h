//------------------------------------------------------------------------
// ファイル名		:PlayerDrone.h
// 概要				:ドローンのベースを継承したプレイヤーのドローンクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容		:
//------------------------------------------------------------------------
// 更新者			:19CU0104 池田翔一郎
// 更新内容		:2021/06/07 ドローンの軌跡エフェクトを追加
//------------------------------------------------------------------------

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "DroneBase.h"
#include "PlayerDrone.generated.h"

//前方宣言
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class UNiagaraComponent;

//各軸の入力情報を管理する列挙
UENUM(BlueprintType)
namespace EINPUT_AXIS
{
	enum Type
	{
		THROTTLE					UMETA(DisplayName = "THROTTLE"),	//上下
		ELEVATOR					UMETA(DisplayName = "ELEVATOR"),	//前後
		AILERON					UMETA(DisplayName = "AILERON"),	//左右
		LADDER						UMETA(DisplayName = "LADDER"),		//旋回
		NUM							UMETA(Hidden)
	};
}

//--------------------------------------------------------------------
//#define DEBUG_UpdateCamera			//カメラのデバッグ
//#define DEBUG_IsOverHeightMax
//--------------------------------------------------------------------

UCLASS()
class AIRFORCE_API APlayerDrone : public ADroneBase
{
	GENERATED_BODY()

public:
	//コンストラクタ
	APlayerDrone();
protected:
	//ゲーム開始時に1度だけ処理
	virtual void BeginPlay() override;

public:
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;

	//【入力バインド】コントローラー入力設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//動いているか判定する(引数：入力軸)
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool IsMoving(const FVector _axisValue)const { return  (!_axisValue.IsZero() ? true : false); }

	//レースの座標ファイル書き込み
	void WritingRaceVector();

	//レースのクオータニオンファイル書き込み
	void WritingRaceQuaternion();
private:
	//【入力バインド】各スティックの入力
	void Input_Throttle(float _axisValue);
	void Input_Elevator(float _axisValue);
	void Input_Aileron(float _axisValue);
	void Input_Ladder(float _axisValue);

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SwitchGameMode(const TEnumAsByte<EGAMEMODE::Type> GameMode) { m_GameMode = (GameMode == EGAMEMODE::GAMEMODE_FPS ? EGAMEMODE::GAMEMODE_TPS : EGAMEMODE::GAMEMODE_FPS); }

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void SwitchViewPort();

	UFUNCTION(BlueprintCallable, Category = "Target")
		UCameraComponent* GetCamera() const { return m_pCamera; }	//カメラ取得

	UFUNCTION(BlueprintCallable, Category = "InputAxis")
		float GetInputValue(const TEnumAsByte<EINPUT_AXIS::Type> _Axis)const { return m_AxisValue[_Axis]; }


	//カメラの初期設定
	void InitializeCamera();

	//カメラ更新処理
	void UpdateCamera(const float& DeltaTime);

	//入力ステートから羽の加速度に変換する処理
	float RightInputValueToWingAcceleration(const int _arrayIndex);
	float LeftInputValueToWingAcceleration(const int _arrayIndex);

	//羽の加速度更新処理
	virtual void UpdateWingAccle(const float& DeltaTime);
	 
	//入力の加速度更新処理
	virtual void UpdateAxisAcceleration(const float& DeltaTime);

	//ステート更新処理
	virtual void UpdateState()override;

	//回転処理
	void UpdateRotation(const float& DeltaTime);

	//移動処理
	void UpdateSpeed(const float& DeltaTime)override;

	//カメラとの遮蔽物のコリジョン判定
	void UpdateCameraCollsion();

	//風のエフェクトの更新処理
	void UpdateWindEffect(const float& DeltaTime);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCamera")
		USpringArmComponent* m_pSpringArm;									//スプリングアーム
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerCamera")
		UCameraComponent* m_pCamera;										//カメラ
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraTargetLength;											//プレイヤーとカメラの距離
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_FieldOfView;												//カメラの視野角
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffset;										//カメラの位置
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffsetMax;									//カメラが追従する範囲の上限
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraMoveLimit;											//カメラの移動できる上限
	//-------------------------------------------------------------------------------------------------------
private:
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraSystem* m_pLightlineEffect;									//ラインエフェクト

	TArray<AActor*> m_pHitActors;											//スプリングアームの直線に衝突しているActor

	UPROPERTY(VisibleAnywhere, Category = "Drone|Input")
		FVector4 m_AxisValue;												//各軸の入力値(0:AILERON、1:ELEVATOR、2:THROTTLE、3:LADDER)

	float m_CameraRotationYaw;
};
