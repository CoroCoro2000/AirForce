//------------------------------------------------------------------------
// ファイル名		:DroneBase.h
// 概要				:ドローンのベースクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/04/19 プレイヤーとエネミーの共通項の追加
//------------------------------------------------------------------------

//インクルードガード
#pragma once
#pragma warning(disable : 4582)

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "DroneBase.generated.h"

//前方宣言
class UStaticMeshComponent;
class UBoxComponent;

//移動用ビットフィールド
USTRUCT(BlueprintType)
struct FMoveDirectionFlag
{
	GENERATED_USTRUCT_BODY()

	//コンストラクタ
	FMoveDirectionFlag(const uint8 up, const uint8 down, const uint8 forward, const uint8 backward, const uint8 right, const uint8 left, const uint8 rightTurning, const uint8 leftTurning)
	: Up(up)
	, Down(down)
	, Forward(forward)
	, Backward(backward)
	, Right(right)
	, Left(left)
	, RightTurning(rightTurning)
	, LeftTurning(leftTurning)
	{}
	FMoveDirectionFlag() : FMoveDirectionFlag(0, 0, 0, 0, 0, 0, 0, 0) {}

	UPROPERTY(EditAnywhere, DisplayName = "Up")
		uint8 Up					 : 1;		//上昇			0
	UPROPERTY(EditAnywhere, DisplayName = "Down")
		uint8 Down				 : 1;		//下降			1
	UPROPERTY(EditAnywhere, DisplayName = "Forward")
		uint8 Forward			 : 1;		//前方移動	2
	UPROPERTY(EditAnywhere, DisplayName = "Backward")
		uint8 Backward		 : 1;		//後方移動	3
	UPROPERTY(EditAnywhere, DisplayName = "Right")
		uint8 Right			 	 : 1;		//右移動		4
	UPROPERTY(EditAnywhere, DisplayName = "Left")
		uint8 Left					 : 1;		//左移動		5
	UPROPERTY(EditAnywhere, DisplayName = "RightTurning")
		uint8 RightTurning	 : 1;		//右回転		6
	UPROPERTY(EditAnywhere, DisplayName = "LeftTurning")
		uint8 LeftTurning		 : 1;		//左回転		7
};

//移動用共用体
union MoveDirection
{
	//コンストラクタ
	MoveDirection()
		: iBits(0)
		, sFlag(FMoveDirectionFlag(0, 0, 0, 0, 0, 0, 0, 0))
	{}
	MoveDirection(const uint8 bits) :iBits(bits) {}
	MoveDirection(const FMoveDirectionFlag moveFlag) :sFlag(moveFlag) {}

	uint8 iBits : 8;		//一括管理(0 ~ 255の値で管理)
	FMoveDirectionFlag sFlag;	//個別管理
};

//状態ビットフィールド
USTRUCT(BlueprintType)
struct FStateFlag
{
	GENERATED_USTRUCT_BODY()
	//コンストラクタ
	FStateFlag(const uint8 wait, const uint8 hovering, const uint8 move, const uint8 crash)
	: Wait(wait)
	, Hovering(hovering)
	, Move(move)
	, Crash(crash)
	{}
	FStateFlag() : FStateFlag(0, 0, 0, 0) {}

	UPROPERTY(EditAnywhere, DisplayName = "Wait")
		uint8 Wait			: 1;		//地面待機						0
	UPROPERTY(EditAnywhere, DisplayName = "Hovering")
		uint8 Hovering	: 1;		//ホバリング(空中待機)		1
	UPROPERTY(EditAnywhere, DisplayName = "Move")
		uint8 Move			: 1;		//移動中							2
	UPROPERTY(EditAnywhere, DisplayName = "Crash")
		uint8 Crash		: 1;		//墜落							3
};

//状態用共用体
union State
{
	//コンストラクタ
	State()
		: iBits(0)
		, sFlag(FStateFlag(0, 0, 0, 0))
	{}
	State(const uint8 bits) :iBits(bits) {}
	State(const FStateFlag stateFlag) :sFlag(stateFlag) {}

	uint8 iBits : 4;								//一括管理(0 ~ 15の値で管理)
	FStateFlag sFlag;							//個別管理
};

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

private:
	UPROPERTY(EditAnywhere, DisplayName = "WingNumber")
		uint8 WingNumber;																//識別番号(1:左前、2:右前、3:左後ろ、4:右後ろ)
	UPROPERTY(EditAnywhere, DisplayName = "WingMesh")
		UStaticMeshComponent* pWingMesh;												//メッシュ
public:
	UPROPERTY(EditAnywhere, DisplayName = "AcceleState")
		float AccelState;																//加速度の段階(-1:最小の加速度、0:加速度なし、1:加速度あり、2:最大の加速度)

public:
	uint8 GetWingNumber()const { return WingNumber; }							//羽番号取得
	UStaticMeshComponent* GetWingMesh()const { return pWingMesh; }	//羽のメッシュ取得
};

//defineマクロ
//羽の最大数
#define WING_ARRAY_MAX 4
//羽の要素番号
#define LF_WING 0
#define RF_WING 1
#define LB_WING 2
#define RB_WING 3
//浮力の状態
#define BUOYANCY_HOVERING 0.f
//現在のFPSを計測
#define FPS (1.f / DeltaTime)
//フレームレートが低下しても移動量に影響が無いよう補正する値
#define MOVE_CORRECTION (60.f / FPS)
//--------------------------------------------------------------------
//#define DEGUG_ACCEL					//加速度のデバッグ
//#define DEBUG_GRAVITY				//重力のデバッグ
//#define DEBUG_WING					//羽のデバッグ
//#define DEBUG_OVERLAP_BEGIN	//オーバーラップ開始時のデバッグ
//#define DEBUG_OVERLAP_END		//オーバーラップ終了時のデバッグ
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
	//このオブジェクトが破棄されるときに呼び出される関数
	virtual void BeginDestory();
public:
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;

	//【入力バインド】コントローラー入力設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	入力フラグの取得
	void SetisControl(const bool _isControl) { m_isControl = _isControl; }

	//オーバーラップ時に呼ばれるイベント関数を登録
	UFUNCTION()
		virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//オーバーラップしていたアクターから離れた瞬間呼ばれるイベント関数
	UFUNCTION()
		virtual void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	//重力加速度の取得
	float GetGravitationalAcceleration()const { return m_GravityScale * m_DescentTime * m_DescentTime / 2.f; }
	//羽の加速度を正規化して返す
	float GetWingNormalizeAccele()const { return m_WingAccele / (m_WingHoveringAccele * m_WingAccelMax); }

	//	ドローンの時速(kilometers per hour)取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetSpeed()const { return m_Speed; }

	//	ドローンの時速(kilometers per hour)取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetKPH(const float _deltaTime)const { return m_Speed / 100000.f / _deltaTime * 3600.f * 10.f; }
	
	//	ドローンのリング獲得数取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Ring")
		int GetRingAcquisition() { return m_RingAcquisition; }
protected:
	//羽の加速度更新処理
	virtual void UpdateWingAccle();
	//重心移動処理
	virtual void UpdateCenterOfGravity(const float& DeltaTime);
	//ステート更新処理
	virtual void UpdateState();
	//回転処理
	virtual void UpdateRotation(const float& DeltaTime);
	//速度更新処理
	virtual void UpdateSpeed(const float& DeltaTime);
	//移動処理
	virtual void UpdateMove(const float& DeltaTime);

	//羽の回転更新処理
	virtual void UpdateWingRotation(const float& DeltaTime);

	//重力更新処理
	float UpdateGravity(const float& DeltaTime);

	//小数第n位未満切り捨て
	virtual float SetDecimalTruncation(float value, float n);

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;

	//羽
	/*-------------------------------------------------------------------------*/
	TArray<TSharedPtr<FWing>> m_pWings;								//ドローンの羽
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_rpsMax;											//1秒間の羽の最大回転数
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccele;									//羽の加速度
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingOldAccele;								//羽の加速度
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMin;								//最小の加速度の倍率
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingHoveringAccele;						//ホバリング(入力なし)時の加速度の倍率
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMax;								//最大の加速度の倍率
	/*-------------------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, Category = "Collision")
		UBoxComponent* m_pDroneBoxComp;					//ドローンの当たり判定

	MoveDirection m_MoveDirectionFlag;					//移動フラグ管理
	State m_StateFlag;									//ステートフラグ管理

	FVector m_CurrentLocation;							//ドローンの現在地
	FVector m_PrevCurrentLocation;						//1つ前のドローンの現在地

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float  m_Speed;									//ドローンの秒速(m)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_SpeedPerSecondMax;						//ドローンの最大秒速(m)

	//-----------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Acceleration;							//加速度
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Deceleration;							//加速度
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_Turning;								
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_MaxSpeed;

	//-------------------------------------------------------------------------------
	FQuat m_OldRotation;						//1フレーム前の傾き

	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_DroneWeight;							//ドローンの重量(kg)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_Velocity;								//このドローンにかかっている力の量
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_CenterOfGravity;						//ドローンの重心
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector Centrifugalforce;						//遠心力

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_AngularVelocity;									//角速度(振動の角速度)

	UPROPERTY(EditAnywhere, Category = "Physical|Gravity")
		float m_GravityScale;							//重力係数
	UPROPERTY(EditAnywhere, Category = "Physical")
		FVector Gravity;								//重力
	UPROPERTY(VisibleAnywhere, Category = "Physical|Gravity")
		float m_DescentTime;							//落下している時間

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* WingRotationSE;								//羽の回転SE

	UPROPERTY(EditAnywhere, Category = "Flag")
		bool m_isControl;								//操作可能フラグ

	UPROPERTY(EditAnywhere, Category = "Flag")
		bool m_isFloating;								//操作可能フラグ

	UPROPERTY(VisibleAnywhere, Category = "Ring")
		int m_RingAcquisition;							//リング獲得数
};
