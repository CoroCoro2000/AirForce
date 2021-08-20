//------------------------------------------------------------------------
// ファイル名	:PlayerDrone.cpp
// 概要				:プレイヤーのドローンクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容		:
//------------------------------------------------------------------------
// 更新者		:19CU0104 池田翔一郎
// 更新内容		:2021/06/07 ドローンの軌跡エフェクトを追加
//				:2021/06/16 ドローンの羽の回転処理の追加
//------------------------------------------------------------------------

//インクルード
#include "PlayerDrone.h"
#include "DroneBase.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

//コンストラクタ
APlayerDrone::APlayerDrone()
	: m_GameMode(EGAMEMODE::GAMEMODE_TPS)
	, m_DroneMode(EDRONEMODE::DRONEMODE_AUTOMATICK)
	, m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(90.f)
	, m_FieldOfView(90.f)
	, m_CameraSocketOffset(FVector::ZeroVector)
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_pLightlineEffect(NULL)
	, m_bCanControl(true)
	, m_AxisValue{ 0.f, 0.f, 0.f, 0.f }
{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	//ドローン用コリジョン生成
	m_pDroneBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerDroneComp"));
	if (m_pDroneBoxComp && m_pBodyMesh)
	{
		m_pDroneBoxComp->SetupAttachment(m_pBodyMesh);
		RootComponent = m_pBodyMesh;
	}

	//スプリングアーム生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_pSpringArm && m_pBodyMesh)
	{
		m_pSpringArm->SetupAttachment(m_pBodyMesh);
	}

	//カメラ生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera && m_pSpringArm)
	{
		m_pCamera->SetupAttachment(m_pSpringArm);
	}

	//カメラの初期設定
	InitializeCamera();

	//ドローン用ラインエフェクト生成
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> LineEffect(TEXT("/Game/Effect/LightLine/E_LightLine_System.E_LightLine_System"));
	if (LineEffect.Succeeded())
	{
		m_pLightlineEffect = LineEffect.Object;
	}

	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


//ゲーム開始時に1度だけ処理
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();
}

//このオブジェクトが破棄されるときに呼び出される関数
void APlayerDrone::BeginDestory()
{
	Super::BeginDestroy();

	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		//領域の開放
		if (pWing.IsValid())
		{
			pWing.Reset();
		}
	}
}

//毎フレーム処理
void APlayerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//羽の更新処理
	UpdateWingAccle();

	//羽の更新処理
	UpdateWingRotation(DeltaTime);

	//入力の加速度更新処理
	UpdateAxisAcceleration(DeltaTime);

	//回転処理
	UpdateRotation(DeltaTime);

	//移動処理
	UpdateSpeed(DeltaTime);

	//重心移動処理
	UpdateCenterOfGravity(DeltaTime);

	//カメラの更新処理
	UpdateCamera(DeltaTime);

	//カメラとの遮蔽物のコリジョン判定
	UpdateCameraCollsion();

	//視点の切り替え
	//SwitchViewPort();
}

//カメラの初期設定
void APlayerDrone::InitializeCamera()
{
	//NULLチェック
	if (!m_pCamera || !m_pSpringArm) { return; }
	
	//スプリングアームの長さを設定
	m_pSpringArm->TargetArmLength = m_CameraTargetLength;
	//スプリングアームの回転ラグを適用する
	m_pSpringArm->bEnableCameraRotationLag = true;
	m_pSpringArm->CameraRotationLagSpeed = 10.f;

	//カメラのコリジョンテストを行わないようにする
	m_pSpringArm->bDoCollisionTest = false;

	//カメラの視野角の設定
	m_pCamera->SetFieldOfView(m_FieldOfView);
}

//	視点の切り替え
void APlayerDrone::SwitchViewPort()
{

}

//右のスティック入力から羽の加速度に変換する処理
float APlayerDrone::RightInputValueToWingAcceleration(const int _arrayIndex)
{
	//羽の加速度
	float wingsAccel[WING_ARRAY_MAX] = { 0.f,0.f,0.f,0.f };

	//スロットルの入力がある時
	if (m_AxisValue[EINPUT_AXIS::THROTTLE] != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValue[EINPUT_AXIS::THROTTLE];
		}
	}

	//ラダーの入力がある時
	if (m_AxisValue[EINPUT_AXIS::LADDER] != 0.f)
	{
		//右入力がされているかどうか
		const bool isRight = (m_AxisValue[EINPUT_AXIS::LADDER] > 0.f ? true : false);
		wingsAccel[LF_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? -1.f : 1.f);
		wingsAccel[LB_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? 1.f : -1.f);
		wingsAccel[RF_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? 1.f : -1.f);
		wingsAccel[RB_WING] += FMath::Abs(m_AxisValue[EINPUT_AXIS::LADDER]) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//左のスティック入力から羽の加速度に変換する処理
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[WING_ARRAY_MAX] = { 0.f,0.f,0.f,0.f };

	//エレベーターの入力がある時
	if (m_AxisValue[EINPUT_AXIS::ELEVATOR] != 0.f)
	{
		//前入力がされているかどうか
		const bool isForward = (m_AxisValue[EINPUT_AXIS::ELEVATOR] > 0.f ? true : false);
		wingAccel[(isForward ? LB_WING : LF_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::ELEVATOR]);
		wingAccel[(isForward ? RB_WING : RF_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::ELEVATOR]);
	}

	//エルロンの入力がある時
	if (m_AxisValue[EINPUT_AXIS::AILERON] != 0.f)
	{
		//右入力がされているかどうか
		const bool isRight = (m_AxisValue[EINPUT_AXIS::AILERON] > 0.f ? true : false);
		wingAccel[(isRight ? LF_WING : RF_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::AILERON]);
		wingAccel[(isRight ? LB_WING : RB_WING)] += FMath::Abs(m_AxisValue[EINPUT_AXIS::AILERON]);
	}
	return wingAccel[_arrayIndex];
}

//羽の加速度更新処理
void APlayerDrone::UpdateWingAccle()
{
	//各スティックの入力の値をを取得
	FVector2D RightAxis = FVector2D(m_AxisValue[EINPUT_AXIS::LADDER], m_AxisValue[EINPUT_AXIS::THROTTLE]);
	FVector2D LeftAxis = FVector2D(m_AxisValue[EINPUT_AXIS::AILERON], m_AxisValue[EINPUT_AXIS::ELEVATOR]);

	//入力がなければ終了
	if ((RightAxis.IsZero() && LeftAxis.IsZero()) || !m_isControl)
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid()) 
			{
				pWing->AccelState = 0.f;
			}
		}
		return;
	}

	//両方の入力がある場合
	if (!RightAxis.IsZero() && !LeftAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid())
			{
				//PPAP
				pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber()) + LeftInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
		return;
	}

	//右スティックのみの場合
	if (!RightAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid())
			{
				pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
	}
	//左スティックのみの場合
	else if (!LeftAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			if (pWing.IsValid())
			{
				pWing->AccelState = LeftInputValueToWingAcceleration(pWing->GetWingNumber());
			}
		}
	}
}

//羽の回転更新処理
void APlayerDrone::UpdateWingRotation(const float& DeltaTime)
{
	//2軸の入力量を合成する
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValue[EINPUT_AXIS::LADDER], m_AxisValue[EINPUT_AXIS::THROTTLE]).Size() +
		FVector2D(m_AxisValue[EINPUT_AXIS::AILERON], m_AxisValue[EINPUT_AXIS::ELEVATOR]).Size()) / 2,
		0.f, 1.f);

	//毎秒m_rpsMax * WingAccel回分回転するために毎フレーム羽を回す角度を求める
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (pWing.IsValid())
		{
			//羽の加速度を0から1の範囲に修正し、正規化する
			const float NormalizeAccelSize = FMath::Clamp((pWing->AccelState + 1.f) / 3.f, 0.f, 1.f);
			//正規化した加速度を使って羽の加速の割合を補間する
			const float WingAccel = FMath::Lerp(m_WingAccelMin
, m_WingAccelMax, NormalizeAccelSize);
			//右回りの羽か判別する(左前と右後ろの羽が右回りに回転する)
			const bool isTurnRight = (pWing->GetWingNumber() == LF_WING || pWing->GetWingNumber() == RB_WING ? true : false);
			//1フレームに回転する角度を求める
			const float angularVelocity = m_rpsMax * 360.f * DeltaTime * WingAccel * (isTurnRight ? 1.f : -1.f) * MOVE_CORRECTION;

			if (pWing->GetWingMesh())
			{
				//羽を回転させる
				pWing->GetWingMesh()->AddLocalRotation(FRotator(0.f, angularVelocity, 0.f));

#ifdef DEBUG_WING
				//*デバッグ用*速度に応じて羽の色変更				
				const FVector WingColor = FVector(FLinearColor::LerpUsingHSV(FColor::Blue, FColor::Yellow, NormalizeAccelSize));
				pWing->GetWingMesh()->SetVectorParameterValueOnMaterials(TEXT("WingColor"), WingColor);
#endif // DEBUG_WING
			}
		}
	}
}

//入力の加速度更新処理
void APlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
{
	for (int i = 0; i < EINPUT_AXIS::NUM; i++)
	{
		//浮力がホバリング状態より大きいとき
		if (m_AxisValue[i] > 0.f)
		{
			if (m_AxisAcceleration[i] < 1.5f)
			{
				m_AxisAcceleration[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//浮力がホバリング状態より小さい時
		else if (m_AxisValue[i] < 0.f)
		{
			if (m_AxisAcceleration[i] > -1.5f)
			{
				m_AxisAcceleration[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//浮力が重力と釣り合う時(ホバリング状態)
		else
		{
			if (m_AxisAcceleration[i] == m_AxisAcceleration[EINPUT_AXIS::LADDER])
				m_AxisAcceleration[i] = 0.f;

			m_AxisAcceleration[i] *= 0.98f;
			m_AxisAcceleration[i] = SetDecimalTruncation(m_AxisAcceleration[i], 3.f);
		}

		m_AxisAcceleration[i] = FMath::Clamp(m_AxisAcceleration[i], -1.5f, 1.5f);
	}
}

//ドローンの回転処理
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);

	//NULLチェック
	if (!m_pBodyMesh) { return; }
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (!pWing.IsValid()) { return; }
	}

	//羽の回転量からドローンの角速度の最大値を設定
	m_AngularVelocity = FVector(
		(m_pWings[LF_WING]->AccelState + m_pWings[LB_WING]->AccelState) - (m_pWings[RF_WING]->AccelState + m_pWings[RB_WING]->AccelState),
		(m_pWings[LB_WING]->AccelState + m_pWings[RB_WING]->AccelState) - (m_pWings[LF_WING]->AccelState + m_pWings[RF_WING]->AccelState),
		(m_pWings[RF_WING]->AccelState + m_pWings[LB_WING]->AccelState) - (m_pWings[LF_WING]->AccelState + m_pWings[RB_WING]->AccelState));
	m_AngularVelocity.Z = FMath::Abs(m_AngularVelocity.Z) * m_AxisAcceleration[EINPUT_AXIS::LADDER];

	//	オートマチックで操作するとき
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float deg = 15.f;
		//前後にドローンが傾きすぎないように補正
		if (((m_pBodyMesh->GetRelativeRotation().Pitch < -deg) && (m_AngularVelocity.Y < 0.f) )|| ( (m_pBodyMesh->GetRelativeRotation().Pitch > deg) && (m_AngularVelocity.Y > 0.f)))
		{
			m_AngularVelocity.Y = 0.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Pitch > 0.f) && (m_AngularVelocity.Y == 0.f))
		{
			m_AngularVelocity.Y = -1.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Pitch < 0.f) && (m_AngularVelocity.Y == 0.f))
		{
			m_AngularVelocity.Y = 1.f;
		}

		//左右にドローンが傾きすぎないように補正
		if ((m_pBodyMesh->GetRelativeRotation().Roll < -deg) && (m_AngularVelocity.X < 0.f) || (m_pBodyMesh->GetRelativeRotation().Roll > deg) && (m_AngularVelocity.X > 0.f))
		{
			m_AngularVelocity.X = 0.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Roll > 0.f) && (m_AngularVelocity.X == 0.f))
		{
			m_AngularVelocity.X = -1.f;
		}
		else if ((m_pBodyMesh->GetRelativeRotation().Roll < 0.f) && (m_AngularVelocity.X == 0.f))
		{
			m_AngularVelocity.X = 1.f;
		}
	}

	//アマチュアで操作するとき
	else
	{
		//角速度の取得(yaw軸は含めない)
		float angularVelocity = FVector(m_AngularVelocity.X, m_AngularVelocity.Y, 0.f).Size();
		//角速度をradに変換
		float radAngularVelocity = FMath::DegreesToRadians(angularVelocity);
		//重力 / 角速度 ^ 2でドローンが円運動するときの半径を求める
		float radius = Gravity.Size() / (radAngularVelocity * radAngularVelocity);
		//半径 * 角速度 ^ 2で遠心力を取得
		if (m_AngularVelocity != FVector::ZeroVector)
		{
			Centrifugalforce = FVector(0.f, 0.f, radius * (radAngularVelocity * radAngularVelocity));
		}
		else
		{
			Centrifugalforce = FVector::ZeroVector;
		}
	}


	//オイラー角をクォータニオンに変換
	FQuat qAngularVelocity = FQuat::MakeFromEuler(m_AngularVelocity);
	//ドローンを回転させる
	//UE_LOG(LogTemp, Warning, TEXT("qAngularVelocity:%s"), *qAngularVelocity.ToString());
	m_pBodyMesh->AddLocalRotation(qAngularVelocity * MOVE_CORRECTION, true);
}

//速度更新処理
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	//Super::UpdateSpeed(DeltaTime);

	//オートマチックで操作するとき
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float speed = 3.5f;
		FVector Auto = FVector::ZeroVector;
		Auto += m_pCamera->GetForwardVector() * speed * -m_AxisAcceleration[EINPUT_AXIS::ELEVATOR];
		Auto += m_pCamera->GetRightVector() * speed * m_AxisAcceleration[EINPUT_AXIS::AILERON];
		Auto += m_pCamera->GetUpVector() * speed * m_AxisAcceleration[EINPUT_AXIS::THROTTLE];
		m_Speed = Auto.Size();
		AddActorWorldOffset(Auto * MOVE_CORRECTION, true);
	}
}

//重心移動処理
void APlayerDrone::UpdateCenterOfGravity(const float& DeltaTime)
{
	Super::UpdateCenterOfGravity(DeltaTime);
}

//ステート更新処理
void APlayerDrone::UpdateState()
{

}

//カメラ更新処理
void APlayerDrone::UpdateCamera(const float& DeltaTime)
{
	//NULLチェック
	if (!m_pCamera || !m_pSpringArm || !m_pBodyMesh) { return; }

#ifdef DEBUG_CAMERA
	//カメラの前方にレイを飛ばす
	//DrawDebugLine(GetWorld(),
	//	m_pCamera->GetComponentLocation(),
	//	m_pCamera->GetComponentLocation() + m_pCamera->GetForwardVector() * 1000.f,
	//	FColor::Blue,
	//	false,
	//	DeltaTime + 2.f);

	//プレイヤーの前方にレイを飛ばす
	DrawDebugLine(GetWorld(),
		m_pBodyMesh->GetComponentLocation(),
		m_pBodyMesh->GetComponentLocation() + m_pBodyMesh->GetForwardVector() * 1000.f,
		FColor::Red,
		false,
		DeltaTime + 0.05f);

#endif // DEBUG_CAMERA

	FRotator Camera = FRotator::ZeroRotator;
	Camera.Pitch = GetActorRotation().Pitch * -1.f;
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
		Camera.Roll = GetActorRotation().Roll * -1.f;
	FVector Direction = m_pBodyMesh->GetUpVector();

	m_pSpringArm->SetRelativeRotation(Camera.Quaternion());


}

//カメラとの遮蔽物のコリジョン判定
void  APlayerDrone::UpdateCameraCollsion()
{
}

//【入力バインド】コントローラー入力設定
void APlayerDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//軸マッピング
	InputComponent->BindAxis(TEXT("Throttle"), this, &APlayerDrone::Drone_Throttle);
	InputComponent->BindAxis(TEXT("Elevator"), this, &APlayerDrone::Drone_Elevator);
	InputComponent->BindAxis(TEXT("Aileron"), this, &APlayerDrone::Drone_Aileron);
	InputComponent->BindAxis(TEXT("Ladder"), this, &APlayerDrone::Drone_Ladder);
}

//【入力バインド】スロットル(上下)の入力があった場合呼び出される関数
void APlayerDrone::Drone_Throttle(float _axisValue)
{
	if (m_isControl)
		m_AxisValue[EINPUT_AXIS::THROTTLE] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::THROTTLE] = 0.f;

	//入力された値が正なら
	if (m_AxisValue[EINPUT_AXIS::THROTTLE] > 0.f)
	{
		//上昇移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Up = true;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
	//入力された値が負なら
	else if (m_AxisValue[EINPUT_AXIS::THROTTLE] < 0.f)
	{
		//下降移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = true;
	}
	//値が入力されていないなら
	else
	{
		m_AxisValue[EINPUT_AXIS::THROTTLE] = 0.f;
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
}

//【入力バインド】エレベーター(前後)の入力があった場合呼び出される関数
void APlayerDrone::Drone_Elevator(float _axisValue)
{
	if (m_isControl)
		m_AxisValue[EINPUT_AXIS::ELEVATOR] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::ELEVATOR] = 0.f;

	//入力された値が正なら
	if (m_AxisValue[EINPUT_AXIS::ELEVATOR] > 0.f)
	{
		//前方移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Forward = true;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
	//入力された値が負なら
	else if (m_AxisValue[EINPUT_AXIS::ELEVATOR] < 0.f)
	{
		//後方移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Forward = false;
		m_MoveDirectionFlag.sFlag.Backward = true;
	}

	else
	{
		m_MoveDirectionFlag.sFlag.Forward = false;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
}

//【入力バインド】エルロン(左右)の入力があった場合呼び出される関数
void APlayerDrone::Drone_Aileron(float _axisValue)
{
	if (m_isControl)
		m_AxisValue[EINPUT_AXIS::AILERON] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::AILERON] = 0.f;

	//入力された値が正なら
	if (m_AxisValue[EINPUT_AXIS::AILERON] > 0.f)
	{
		//右移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Right = true;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
	//入力された値が負なら
	else if (m_AxisValue[EINPUT_AXIS::AILERON] < 0.f)
	{
		//左移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Right = false;
		m_MoveDirectionFlag.sFlag.Left = true;
	}
	//値が入力されていないなら
	else
	{
		m_MoveDirectionFlag.sFlag.Right = false;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
}

//【入力バインド】ラダー(旋回)の入力があった場合呼び出される関数
void APlayerDrone::Drone_Ladder(float _axisValue)
{
	if(m_isControl)
		m_AxisValue[EINPUT_AXIS::LADDER] = FMath::Clamp(_axisValue, -1.f, 1.f);
	else
		m_AxisValue[EINPUT_AXIS::LADDER] = 0.f;

	//入力された値が正なら
	if (m_AxisValue[EINPUT_AXIS::LADDER] > 0.f)
	{
		//右旋回フラグを立てる
		m_MoveDirectionFlag.sFlag.RightTurning = true;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
	//入力された値が負なら
	else if (m_AxisValue[EINPUT_AXIS::LADDER] < 0.f)
	{
		//左旋回フラグを立てる
		m_MoveDirectionFlag.sFlag.RightTurning = false;
		m_MoveDirectionFlag.sFlag.LeftTurning = true;
	}
	//値が入力されていないなら
	else
	{
		m_MoveDirectionFlag.sFlag.RightTurning = false;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
}