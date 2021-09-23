//------------------------------------------------------------------------
// ファイル名		:DroneBase.h
// 概要				:ドローンのベースクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/04/19 プレイヤーとエネミーの共通項の追加
//------------------------------------------------------------------------

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "DroneBase.generated.h"

//前方宣言
class UStaticMeshComponent;
class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;

//移動用ビットフィールド
struct FMoveDirectionFlag
{
	//すべてのフラグに対して設定を行う関数
	void SetAllFlag(const bool& _isUp, const bool& _isDown, const bool& _isForward, const bool& _isBackward, const bool& _isRight, const bool& _isLeft, const bool& _isRightTurning, const bool& _isLeftTurning)
	{

	}

		uint8 Up					 : 1;		//上昇			0
		uint8 Down				 : 1;		//下降			1
		uint8 Forward			 : 1;		//前方移動	2
		uint8 Backward			: 1;		//後方移動	3
		uint8 Right			 		 : 1;		//右移動		4
		uint8 Left					 : 1;		//左移動		5
		uint8 RightTurning		: 1;		//右回転		6
		uint8 LeftTurning		 : 1;		//左回転		7
};

//移動用共用体
union MoveDirection
{
	uint8 iBits : 8;		//一括管理(0 ~ 255の値で管理)
	FMoveDirectionFlag sFlag;	//個別管理
};

//状態ビットフィールド
struct FStateFlag
{
		uint8 Wait			: 1;		//地面待機						0
		uint8 Hovering	: 1;		//ホバリング(空中待機)		1
		uint8 Move			: 1;		//移動中							2
		uint8 Crash		: 1;		//墜落							3
};

//状態用共用体
union State
{
	uint8 iBits : 4;								//一括管理(0 ~ 15の値で管理)
	FStateFlag sFlag;							//個別管理
};

//羽の番号の列挙
UENUM(BlueprintType)
namespace EWING
{
	enum Type
	{
		LEFT_FORWARD						UMETA(DisplayName = "LEFT_FORWARD"),
		RIGHT_FORWARD					UMETA(DisplayName = "RIGHT_FORWARD"),
		LEFT_BACKWARD					UMETA(DisplayName = "LEFT_BACKWARD"),
		RIGHT_BACKWARD				UMETA(DisplayName = "RIGHT_BACKWARD"),
		NUM										UMETA(Hidden)
	};
}

//羽の情報を管理する構造体
USTRUCT(BlueprintType)
struct FWing
{
	GENERATED_USTRUCT_BODY()

		//コンストラクタ
		FWing()
		: WingNumber(0)
		, pWingMesh(NULL)
		, AccelState(0.f)
	{}

	FWing(const uint8 wingNum, UStaticMeshComponent* wingMesh)
		: WingNumber(wingNum)
		, pWingMesh(wingMesh)
		, AccelState(0.f)
	{}

public:
	uint8 GetWingNumber()const { return WingNumber; }							//羽番号取得
	UStaticMeshComponent* GetWingMesh()const { return pWingMesh; }	//羽のメッシュ取得

private:
	UPROPERTY(EditAnywhere, DisplayName = "WingNumber")
		uint8 WingNumber;																//識別番号(1:左前、2:右前、3:左後ろ、4:右後ろ)
	UPROPERTY(EditAnywhere, DisplayName = "WingMesh")
		UStaticMeshComponent* pWingMesh;										//メッシュ

public:
	UPROPERTY(EditAnywhere, DisplayName = "AcceleState")
		float AccelState;																		//加速度の段階(-1:最小の加速度、0:加速度なし、1:加速度あり、2:最大の加速度)
};

//	視点切り替え
UENUM(BlueprintType)
namespace EGAMEMODE
{
	enum Type
	{
		GAMEMODE_FPS	UMETA(DisplayName = "FPS"),		//1人称
		GAMEMODE_TPS	UMETA(DisplayName = "TPS"),		//3人称
	};
}

//ドローンの操作状態列挙
UENUM(BlueprintType)
namespace EDRONEMODE
{
	enum Type
	{
		DRONEMODE_AUTOMATICK	UMETA(DisplayName = "AUTO"),	//オートマチック
		DRONEMODE_MANUAL			UMETA(DisplayName = "MANUAL")	//マニュアル
	};
}

//defineマクロ
//現在のFPSを計測
#define FPS (1.f / DeltaTime)
//フレームレートが低下しても移動量に影響が無いよう補正する値
#define MOVE_CORRECTION (60.f / FPS)
#define SLOPE_MIN 0.f
#define SPEED_MIN -10.5f
#define SPEED_MAX 10.5f
#define VECTOR3_COMPONENT_NUM 3
#define VECTOR4_COMPONENT_NUM 4
//--------------------------------------------------------------------
//#define DEGUG_ACCEL					//加速度のデバッグ
//#define DEBUG_GRAVITY				//重力のデバッグ
//#define DEBUG_WING					//羽のデバッグ
//#define DEBUG_COLLISION_WINDRANGE		//風の範囲の当たり判定のデバッグ表示
//#define DEBUG_COLLISION_WINDRANGE_OVERLAPDETA	//風の範囲の当たり判定にオーバーラップしているアクターの数
//#define DEBUG_WindRangeOverlap_Begin		//オーバーラップ開始時のデバッグ
//#define DEBUG_WindRangeOverlap_End		//オーバーラップ開始時のデバッグ
//--------------------------------------------------------------------

UCLASS()
class AIRFORCE_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ADroneBase();
protected:
	//ゲーム開始時に1度だけ処理
	virtual void BeginPlay() override;

public:
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;

	//【入力バインド】コントローラー入力設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	入力フラグの取得
	void SetisControl(const bool _isControl) { m_isControl = _isControl; }

protected:
	//ドローンの当たり判定にオブジェクトがオーバーラップした時呼ばれるイベント関数を登録
	UFUNCTION()
		virtual void OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//ドローンの当たり判定にオブジェクトがヒットした時呼ばれるイベント関数を登録
	UFUNCTION()
		void OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	//操作可能フラグ取得
	UFUNCTION(BlueprintCallable, Category = "Drone")
		bool GetisControl() { return  m_isControl; }

	//ボディメッシュの回転量取得
	UFUNCTION(BlueprintCallable, Category = "Drone")
		FRotator GetBodyMeshRotation() { return m_pBodyMesh->GetComponentRotation(); }

	//重力加速度の取得
	float GetGravitationalAcceleration()const { return m_GravityScale * m_DescentTime * m_DescentTime / 2.f; }

	//	ドローンの時速(kilometers per hour)取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetKPH(const float DeltaTime)const { return m_Velocity.Size() * (60.f / (1.f / DeltaTime)) / 100000.f / DeltaTime * 3600.f * 5.f; }
	
protected:
	//羽の加速度更新処理
	virtual void UpdateWingAccle(const float& DeltaTime);
	//ステート更新処理
	virtual void UpdateState();
	//回転処理
	virtual void UpdateRotation(const float& DeltaTime);
	//速度更新処理
	virtual void UpdateSpeed(const float& DeltaTime);
	//羽の回転更新処理
	virtual void UpdateWingRotation(const float& DeltaTime);
	//重力更新処理
	float UpdateGravity(const float& DeltaTime);
	//風のエフェクト更新処理
	virtual void UpdateWindEffect(const float& DeltaTime);

	//進行軸と入力軸が逆向きか確認
	bool IsReverseInput(const float& _movingAxis, const float& _axisValue)const { return (_movingAxis < 0.f && 0.f < _axisValue) || (_movingAxis > 0.f && 0.f > _axisValue); }

	//高度の上限をを超えているか確認
	bool IsOverHeightMax();

protected:
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "GameMode")
		TEnumAsByte<EGAMEMODE::Type> m_GameMode;	//	視点切り替え

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "GameMode")
		TEnumAsByte<EDRONEMODE::Type> m_DroneMode;	//	ドローン操作切り替え
	//-------------------------------------------------------------------------------------------------------
	//BODY
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;
	//ドローンのコリジョン
	UPROPERTY(EditAnywhere, Category = "Collision")
		USphereComponent* m_pDroneCollision;
	//WING
	UPROPERTY(EditAnywhere, Category = "Wing")
		FWing m_Wings[EWING::NUM];								
	//1秒間の羽の最大回転数
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_RPSMax;
	//羽の加速度
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccele;
	//最小の加速度の倍率
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMin;
	//最大の加速度の倍率
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMax;
	//移動フラグ管理
	MoveDirection m_MoveDirectionFlag;
	//ステートフラグ管理
	State m_StateFlag;

	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_TiltLimit;									//傾きの上限
	UPROPERTY(EditAnywhere, Category = "Physical")
		float  m_Speed;									//ドローンの秒速(m)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_SpeedPerSecondMax;						//ドローンの最大秒速(m)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector4 m_AxisAccel;
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Acceleration;							//加速度
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Deceleration;							//減速度
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Turning;								//逆入力した時の減速率
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_DroneWeight;							//ドローンの重量(kg)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_Velocity;								//このドローンにかかっている力の量
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_CentrifugalForce;						//遠心力
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_AngularVelocity;									//角速度(振動の角速度)
	UPROPERTY(EditAnywhere, Category = "Physical|Gravity")
		float m_GravityScale;							//重力係数
	UPROPERTY(EditAnywhere, Category = "Physical")
		FVector m_Gravity;								//重力
	UPROPERTY(VisibleAnywhere, Category = "Physical|Gravity")
		float m_DescentTime;							//落下している時間
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_pWingRotationSE;			//羽の回転SE

	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_HeightMax;															//ドローンが飛ぶことのできる地面からの高さの範囲
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_HeightFromGround;												//地面からの高さ
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_DistanceToSlope;													//斜面までの距離

	UPROPERTY(EditAnywhere, Category = "Drone")
		bool m_isControl;								//操作可能フラグ
	UPROPERTY(EditAnywhere, Category = "Drone")
		bool m_isFloating;								//滑空フラグ
	UPROPERTY(EditAnywhere, Category = "Drone")
		FVector4 m_AxisValuePerFrame;													//毎フレーム更新される入力の値

	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraComponent* m_pWindEffect;									//風のエフェクト
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindRotationSpeed;											//風のエフェクトの回転速度
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindOpacity;
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindScale;
	TArray<TArray<FString>> m_SaveVelocityText;			//読み込んだ毎フレームの移動量を格納する配列
	TArray<TArray<FString>> m_SaveQuatText;				//読み込んだ毎フレームの回転量を格納する配列
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveVelocityLoadPath;							//移動量が書きこまれたファイルをたどるパスを設定
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveQuatLoadPath;								//回転量が書きこまれたファイルをたどるパスを設定

	UPROPERTY(VisibleAnywhere, Category = "Ring")
		bool m_bIsPassedRing;															//リングをくぐったかどうか
	UPROPERTY(VisibleAnywhere, Category = "Ring")
		float m_SincePassageCount;													//リングをくぐってからの経過時間
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_CountLimitTime;													//リングをくぐってから数える時間の上限	
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_OverAccelerator;												//リングをくぐったときの加速倍率
};
