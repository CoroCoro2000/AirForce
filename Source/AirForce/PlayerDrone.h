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
		THROTTLE				UMETA(DisplayName = "THROTTLE"),//上下
		ELEVATOR				UMETA(DisplayName = "ELEVATOR"),//前後
		AILERON					UMETA(DisplayName = "AILERON"),	//左右
		LADDER					UMETA(DisplayName = "LADDER"),	//旋回
		NUM						UMETA(Hidden)
	};
}

namespace EPATH_DRONE
{
	enum Type
	{
		REPLAY					UMETA(DisplayName = "REPLAY"),	//リプレイ
		BEST					UMETA(DisplayName = "BEST"),	//ベストタイム
		NUM						UMETA(Hidden)
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
	//デストラクタ
	virtual ~APlayerDrone() {}

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

	//リプレイが再生終了したか判定する
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool IsEndPlayBackReplay()const { return  (m_PlaybackFlame > m_SaveVelocityText[EPATH_DRONE::REPLAY].Num() -1 ? true : false); }

	//リプレイのレースの座標ファイル書き込み
	UFUNCTION(BlueprintCallable, Category = "SaveRecord")
		void WritingReplayRaceVector();
	//リプレイのレースのクオータニオンファイル書き込み
	UFUNCTION(BlueprintCallable, Category = "SaveRecord")
		void WritingReplayRaceQuaternion();

	//1位のレースの座標ファイル書き込み
	UFUNCTION(BlueprintCallable, Category = "SaveRecord")
		void WritingBestRaceVector();
	//1位のレースのクオータニオンファイル書き込み
	UFUNCTION(BlueprintCallable, Category = "SaveRecord")
		void WritingBestRaceQuaternion();

	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		void SetIsOutCourse(const bool _isOutCourse) { m_bIsOutCourse = _isOutCourse; }
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool GetIsOutCourse()const { return m_bIsOutCourse; }

	//リプレイの初期設定
	UFUNCTION(BlueprintCallable, Category = "Replay")
		void InitializeReplay();
	UFUNCTION(BlueprintCallable, Category = "Replay")
		bool GetisReplay()const { return m_isReplay; }
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

	//エフェクトの初期設定
	void InitializeEmitter();

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

	//回転処理
	void UpdateRotation(const float& DeltaTime);

	//移動処理
	void UpdateSpeed(const float& DeltaTime)override;

	//カメラとの遮蔽物のコリジョン判定
	void UpdateCameraCollsion(const float& DeltaTime);

	//風のエフェクトの更新処理
	void UpdateWindEffect(const float& DeltaTime);

	//リプレイ更新処理
	void UpdateReplay(const float& DeltaTime);

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

	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FRotator m_CameraRotationAttenRate;							//カメラが傾く速さ

	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_MotionBlurAmount;											//モーションブラーの強度
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_MotionBlurMax;													//モーションブラーの最大歪み
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		int32 m_MotionBlurTargetFPS;											//モーションブラーのターゲットFPS
	//-------------------------------------------------------------------------------------------------------
private:
	UPROPERTY(VisibleAnywhere, Category = "Drone|Input")
		FVector4 m_AxisValue;												//各軸の入力値(0:AILERON、1:ELEVATOR、2:THROTTLE、3:LADDER)

	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveTypeFolderPath;

	UPROPERTY(EditAnywhere, Category = "Ghost")
		bool m_isReplay;

	FVector m_StartLocation;
	FQuat m_StartQuaternion;

	float m_CameraRotationYaw;

	bool m_bIsOutCourse;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_BodyOpacity;
};
