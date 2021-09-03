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
#include "GameUtility.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
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
	, m_CameraSocketOffsetMax(0.f)
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_pLightlineEffect(NULL)
	, m_pWindEffect(NULL)
	, m_WindRotationSpeed(5.f)
	, m_AxisValue(FVector4(0.f, 0.f, 0.f, 0.f))
{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = m_pDroneCollision;

	if (m_pBodyMesh)
	{
		m_pBodyMesh->SetupAttachment(m_pDroneCollision);
	}

	//スプリングアーム生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_pSpringArm && m_pBodyMesh)
	{
		m_pSpringArm->SetupAttachment(m_pDroneCollision);
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

	//チェックポイントを指す矢印生成
	m_pWindEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ArrowEffect"));
	if (m_pWindEffect)
	{
		m_pWindEffect->SetupAttachment(m_pDroneCollision);
	}

	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


//ゲーム開始時に1度だけ処理
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();

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

	//カメラの更新処理
	UpdateCamera(DeltaTime);

	//カメラとの遮蔽物のコリジョン判定
	UpdateCameraCollsion();

	//視点の切り替え
	//SwitchViewPort();

	//風のエフェクト更新処理
	UpdateWindEffect(DeltaTime);
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
	float wingsAccel[EWING::NUM] = { 0.f,0.f,0.f,0.f };

	//スロットルの入力がある時
	if (m_AxisValue.Z != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValue.Z;
		}
	}

	//ラダーの入力がある時
	if (m_AxisValue.W != 0.f)
	{
		//右入力がされているかどうか
		const bool isRight = (m_AxisValue.W > 0.f ? true : false);
		wingsAccel[EWING::LEFT_FORWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? -1.f : 1.f);
		wingsAccel[EWING::LEFT_BACKWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_FORWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_BACKWARD] += FMath::Abs(m_AxisValue.W) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//左のスティック入力から羽の加速度に変換する処理
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[EWING::NUM] = { 0.f,0.f,0.f,0.f };

	//エレベーターの入力がある時
	if (m_AxisValue.Y != 0.f)
	{
		//前入力がされているかどうか
		const bool isForward = (m_AxisValue.Y > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValue.Y);
		wingAccel[(isForward ? EWING::LEFT_BACKWARD : EWING::LEFT_FORWARD)] += axisAbsValue;
		wingAccel[(isForward ? EWING::RIGHT_BACKWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
	}

	//エルロンの入力がある時
	if (m_AxisValue.X != 0.f)
	{
		//右入力がされているかどうか
		const bool isRight = (m_AxisValue.X > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValue.X);
		wingAccel[(isRight ? EWING::LEFT_FORWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
		wingAccel[(isRight ? EWING::LEFT_BACKWARD : EWING::RIGHT_BACKWARD)] += axisAbsValue;
	}
	return wingAccel[_arrayIndex];
}

//羽の加速度更新処理
void APlayerDrone::UpdateWingAccle()
{
	//各スティックの入力の値をを取得
	FVector2D RightAxis = FVector2D(m_AxisValue.W, m_AxisValue.Z);
	FVector2D LeftAxis = FVector2D(m_AxisValue.X, m_AxisValue.Y);

	//入力がなければ終了
	if ((RightAxis.IsZero() && LeftAxis.IsZero()) || !m_isControl)
	{
		for (FWing& wing : m_Wings)
		{
			wing.AccelState = 0.f;
		}
		return;
	}

	//両方の入力がある場合
	if (!RightAxis.IsZero() && !LeftAxis.IsZero())
	{
		for (FWing& wing : m_Wings)
		{
			//PPAP
			wing.AccelState = RightInputValueToWingAcceleration(wing.GetWingNumber()) + LeftInputValueToWingAcceleration(wing.GetWingNumber());
		}
		return;
	}

	//右スティックのみの場合
	if (!RightAxis.IsZero())
	{
		for (FWing& wing : m_Wings)
		{
			wing.AccelState = RightInputValueToWingAcceleration(wing.GetWingNumber());
		}
	}
	//左スティックのみの場合
	else if (!LeftAxis.IsZero())
	{
		for (FWing& wing : m_Wings)
		{
			wing.AccelState = LeftInputValueToWingAcceleration(wing.GetWingNumber());
		}
	}
}

//羽の回転更新処理
void APlayerDrone::UpdateWingRotation(const float& DeltaTime)
{
	//2軸の入力量を合成する
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValue.W, m_AxisValue.Z).Size() +
		FVector2D(m_AxisValue.X, m_AxisValue.Y).Size()) / 2,
		0.f, 1.f);

	//毎秒m_rpsMax * WingAccel回分回転するために毎フレーム羽を回す角度を求める
	for (FWing& wing : m_Wings)
	{
		if (wing.GetWingMesh())
		{
			//羽の加速度を0から1の範囲に修正し、正規化する
			const float NormalizeAccelSize = FMath::Clamp((wing.AccelState + 1.f) / 3.f, 0.f, 1.f);
			//正規化した加速度を使って羽の加速の割合を補間する
			const float WingAccel = FMath::Lerp(m_WingAccelMin
				, m_WingAccelMax, NormalizeAccelSize);
			//右回りの羽か判別する(左前と右後ろの羽が右回りに回転する)
			const bool isTurnRight = (wing.GetWingNumber() == EWING::LEFT_FORWARD || wing.GetWingNumber() == EWING::RIGHT_BACKWARD ? true : false);
			//1フレームに回転する角度を求める
			const float angularVelocity = m_RPSMax * 360.f * DeltaTime * WingAccel * (isTurnRight ? 1.f : -1.f) * MOVE_CORRECTION;

			//羽を回転させる
			wing.GetWingMesh()->AddLocalRotation(FRotator(0.f, angularVelocity, 0.f));

#ifdef DEBUG_WING
			//*デバッグ用*速度に応じて羽の色変更				
			const FVector WingColor = FVector(FLinearColor::LerpUsingHSV(FColor::Blue, FColor::Yellow, NormalizeAccelSize));
			wing.GetWingMesh()->SetVectorParameterValueOnMaterials(TEXT("WingColor"), WingColor);
#endif // DEBUG_WING
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
			if (m_AxisAccel[i] < m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//浮力がホバリング状態より小さい時
		else if (m_AxisValue[i] < 0.f)
		{
			if (m_AxisAccel[i] > -m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValue[i] * DeltaTime;
			}
		}
		//浮力が重力と釣り合う時(ホバリング状態)
		else
		{
			if (m_AxisAccel[i] == m_AxisAccel.W)
			{
				m_AxisAccel[i] = 0.f;
			}
				
			m_AxisAccel[i] *= m_Deceleration;
		}

		//m_AxisAccel[i] = CGameUtility::SetDecimalTruncation(m_AxisAccel[i], 3);
		m_AxisAccel[i] = FMath::Clamp(m_AxisAccel[i], -m_WingAccelMax, m_WingAccelMax);
	}
}

//ドローンの回転処理
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);

	//NULLチェック
	if (!m_pBodyMesh) { return; }

	//羽の回転量からドローンの角速度の最大値を設定
	m_AngularVelocity = FVector(
		(m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState),
		(m_Wings[EWING::LEFT_BACKWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_FORWARD].AccelState),
		(m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState));
	m_AngularVelocity.Z = FMath::Abs(m_AngularVelocity.Z) * m_AxisAccel.W;

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
		float radius = m_Gravity.Size() / (radAngularVelocity * radAngularVelocity);
		//半径 * 角速度 ^ 2で遠心力を取得
		if (m_AngularVelocity != FVector::ZeroVector)
		{
			m_CentrifugalForce = FVector(0.f, 0.f, radius * (radAngularVelocity * radAngularVelocity));
		}
		else
		{
			m_CentrifugalForce = FVector::ZeroVector;
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

	//オートマチックで操作するとき
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		FRotator BodyRotation = m_pBodyMesh->GetComponentRotation();
		BodyRotation.Pitch = 0.f;
		BodyRotation.Roll = 0.f;

		FQuat BodyQuat = BodyRotation.Quaternion();

		float speed = 3.5f;
		m_Velocity = FVector::ZeroVector;
		m_Velocity += BodyQuat.GetRightVector() * speed * m_AxisAccel.X;
		m_Velocity += BodyQuat.GetForwardVector() * speed * -m_AxisAccel.Y;
		m_Velocity += BodyQuat.GetUpVector() * speed * m_AxisAccel.Z;
		m_Speed = m_Velocity.Size();
		AddActorWorldOffset(m_Velocity * MOVE_CORRECTION, true);
	}
	else
	{
		Super::UpdateSpeed(DeltaTime);
	}
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

	FRotator BodyRotation = m_pBodyMesh->GetComponentRotation();
	BodyRotation.Pitch = 0.f;
	BodyRotation.Roll = 0.f;
	FQuat BodyQuat = BodyRotation.Quaternion();

	FVector CameraLocation = m_pCamera->GetComponentLocation();
	FVector DroneLocation = GetActorLocation() + BodyQuat.GetForwardVector() * 100000000.f;
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - CameraLocation).Rotator();
	FRotator CameraRotation = m_pCamera->GetComponentRotation();
	FRotator NewRotation = FMath::RInterpTo(CameraRotation, LookAtRotation, DeltaTime, 10.f);
	NewRotation.Roll = 0.f;

	FRotator Camera = FRotator::ZeroRotator;
	//NewRotation.Pitch = m_pBodyMesh->GetComponentRotation().Pitch * -1.f;
	//Camera.Pitch = m_pBodyMesh->GetRelativeRotation().Pitch * -1.f;
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		//Camera.Roll = m_pBodyMesh->GetRelativeRotation().Roll * -1.f;
	}

	//m_pSpringArm->SetRelativeRotation(Camera.Quaternion());
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, m_pBodyMesh->GetRelativeRotation().Yaw, 0.f));
	m_pCamera->SetWorldRotation(NewRotation.Quaternion());

	//ソケット
	m_pSpringArm->SocketOffset = FVector(m_AxisAccel.Y, m_AxisAccel.X, 0.f) * m_CameraSocketOffsetMax / m_WingAccelMax;
}

//カメラとの遮蔽物のコリジョン判定
void  APlayerDrone::UpdateCameraCollsion()
{

}

//風のエフェクトの更新処理
void APlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	if (m_pWindEffect) 
	{
		//エフェクトとチェックポイントの座標を取得
		FVector EffectLocation = m_pWindEffect->GetComponentLocation();
		FVector  Direction = EffectLocation + m_Velocity.GetSafeNormal();
		//エフェクトが進行方向へ向くようにする
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction - EffectLocation).Rotator();
		//移動量の大きさからエフェクトの不透明度を設定
		FVector Accel = m_AxisAccel;
		float AccelRate = FMath::Clamp(Accel.Size() / m_WingAccelMax, 0.f, 1.f);
		float WindOpacity = FMath::Lerp(0.f, 1.f, AccelRate);

		//回転処理
		m_pWindEffect->SetWorldRotation(LookAtRotation.Quaternion() * MOVE_CORRECTION);
		//エフェクトの不透明度を変更
		m_pWindEffect->SetVariableFloat(TEXT("User.WindOpacity"), WindOpacity);
	}
#ifdef DEBUG_WindEffect
	else
	{
		//NULLだった場合ログ表示
		UE_LOG(LogTemp, Error, TEXT("NULL:m_pWindEffect"));
	}
#endif // DEBUG_WindEffect
}

//【入力バインド】コントローラー入力設定
void APlayerDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//軸マッピング
	InputComponent->BindAxis(TEXT("Throttle"), this, &APlayerDrone::Input_Throttle);
	InputComponent->BindAxis(TEXT("Elevator"), this, &APlayerDrone::Input_Elevator);
	InputComponent->BindAxis(TEXT("Aileron"), this, &APlayerDrone::Input_Aileron);
	InputComponent->BindAxis(TEXT("Ladder"), this, &APlayerDrone::Input_Ladder);
}

//【入力バインド】スロットル(上下)の入力があった場合呼び出される関数
void APlayerDrone::Input_Throttle(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.Z = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.Z = 0.f;
	}
		
	//入力された値が正なら
	if (m_AxisValue.Z > 0.f)
	{
		//上昇移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Up = true;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
	//入力された値が負なら
	else if (m_AxisValue.Z < 0.f)
	{
		//下降移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = true;
	}
	//値が入力されていないなら
	else
	{
		m_MoveDirectionFlag.sFlag.Up = false;
		m_MoveDirectionFlag.sFlag.Down = false;
	}
}

//【入力バインド】エレベーター(前後)の入力があった場合呼び出される関数
void APlayerDrone::Input_Elevator(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.Y = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.Y = 0.f;
	}
		
	//入力された値が正なら
	if (m_AxisValue.Y > 0.f)
	{
		//前方移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Forward = true;
		m_MoveDirectionFlag.sFlag.Backward = false;
	}
	//入力された値が負なら
	else if (m_AxisValue.Y < 0.f)
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
void APlayerDrone::Input_Aileron(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.X = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.X = 0.f;
	}

	//入力された値が正なら
	if (m_AxisValue.X > 0.f)
	{
		//右移動フラグを立てる
		m_MoveDirectionFlag.sFlag.Right = true;
		m_MoveDirectionFlag.sFlag.Left = false;
	}
	//入力された値が負なら
	else if (m_AxisValue.X < 0.f)
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
void APlayerDrone::Input_Ladder(float _axisValue)
{
	if (m_isControl)
	{
		m_AxisValue.W = FMath::Clamp(_axisValue, -1.f, 1.f);
	}
	else
	{
		m_AxisValue.W = 0.f;
	}
		
	//入力された値が正なら
	if (m_AxisValue.W > 0.f)
	{
		//右旋回フラグを立てる
		m_MoveDirectionFlag.sFlag.RightTurning = true;
		m_MoveDirectionFlag.sFlag.LeftTurning = false;
	}
	//入力された値が負なら
	else if (m_AxisValue.W < 0.f)
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