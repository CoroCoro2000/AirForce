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
#include "Misc/FileHelper.h"
#include "DrawDebugHelpers.h"

//コンストラクタ
APlayerDrone::APlayerDrone()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(90.f)
	, m_FieldOfView(90.f)
	, m_CameraSocketOffset(FVector::ZeroVector)
	, m_CameraSocketOffsetMax(FVector(30.f, 45.f, 30.f))
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_pLightlineEffect(NULL)
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
	m_pWindEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindEffect"));
	if (m_pWindEffect)
	{
		m_pWindEffect->SetupAttachment(m_pCamera);
	}

	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


//ゲーム開始時に1度だけ処理
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.TickInterval = 0.f;
}

//毎フレーム処理
void APlayerDrone::Tick(float DeltaTime)
{
	//入力量をフレームに同期
	m_AxisValuePerFrame = m_AxisValue;
	

	//羽の更新処理
	UpdateWingAccle(DeltaTime);

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
	if (m_AxisValuePerFrame.Z != 0.f)
	{
		for (float& wingAccel : wingsAccel)
		{
			wingAccel += m_AxisValuePerFrame.Z;
		}
	}

	//ラダーの入力がある時
	if (m_AxisValuePerFrame.W != 0.f)
	{
		//右入力がされているかどうか
		const bool isRight = (m_AxisValuePerFrame.W > 0.f ? true : false);
		wingsAccel[EWING::LEFT_FORWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? -1.f : 1.f);
		wingsAccel[EWING::LEFT_BACKWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_FORWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? 1.f : -1.f);
		wingsAccel[EWING::RIGHT_BACKWARD] += FMath::Abs(m_AxisValuePerFrame.W) * (isRight ? -1.f : 1.f);
	}
	return wingsAccel[_arrayIndex];
}

//左のスティック入力から羽の加速度に変換する処理
float APlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
{
	float wingAccel[EWING::NUM] = { 0.f,0.f,0.f,0.f };

	//エレベーターの入力がある時
	if (m_AxisValuePerFrame.Y != 0.f)
	{
		//前入力がされているかどうか
		const bool isForward = (m_AxisValuePerFrame.Y > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValuePerFrame.Y);
		wingAccel[(isForward ? EWING::LEFT_BACKWARD : EWING::LEFT_FORWARD)] += axisAbsValue;
		wingAccel[(isForward ? EWING::RIGHT_BACKWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
	}

	//エルロンの入力がある時
	if (m_AxisValuePerFrame.X != 0.f)
	{
		//右入力がされているかどうか
		const bool isRight = (m_AxisValuePerFrame.X > 0.f ? true : false);
		const float axisAbsValue = FMath::Abs(m_AxisValuePerFrame.X);
		wingAccel[(isRight ? EWING::LEFT_FORWARD : EWING::RIGHT_FORWARD)] += axisAbsValue;
		wingAccel[(isRight ? EWING::LEFT_BACKWARD : EWING::RIGHT_BACKWARD)] += axisAbsValue;
	}
	return wingAccel[_arrayIndex];
}

//羽の加速度更新処理
void APlayerDrone::UpdateWingAccle(const float& DeltaTime)
{
	//各スティックの入力の値をを取得
	FVector2D RightAxis = FVector2D(m_AxisValuePerFrame.W, m_AxisValuePerFrame.Z);
	FVector2D LeftAxis = FVector2D(m_AxisValuePerFrame.X, m_AxisValuePerFrame.Y);

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

//入力の加速度更新処理
void APlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
{
	for (int i = 0; i < EINPUT_AXIS::NUM; i++)
	{
		//浮力がホバリング状態より大きいとき
		if (m_AxisValuePerFrame[i] > 0.f)
		{
			if (m_AxisAccel[i] < m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValuePerFrame[i] * DeltaTime;
			}
		}
		//浮力がホバリング状態より小さい時
		else if (m_AxisValuePerFrame[i] < 0.f)
		{
			if (m_AxisAccel[i] > -m_WingAccelMax)
			{
				m_AxisAccel[i] += m_AxisValuePerFrame[i] * DeltaTime;
			}
		}
		//浮力が重力と釣り合う時(ホバリング状態)
		else
		{
			if (m_AxisAccel[i] == m_AxisAccel.W)
			{
				m_AxisAccel[i] = 0.f;
			}
			
			//0に近くなったら一定量で0にする
			if (FMath::Abs(m_AxisAccel[i]) > 0.005f)
			{
				m_AxisAccel[i] *= m_Deceleration;
			}
			else
			{
				m_AxisAccel[i] = 0.f;
			}
		}

		m_AxisAccel[i] = FMath::Clamp(m_AxisAccel[i], -m_WingAccelMax, m_WingAccelMax);
	}
}

//ドローンの回転処理
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	Super::UpdateRotation(DeltaTime);

	if (m_isControl)
	{
		m_SaveQuatX.Add(FString::SanitizeFloat(m_SaveQuat.X));
		m_SaveQuatY.Add(FString::SanitizeFloat(m_SaveQuat.Y));
		m_SaveQuatZ.Add(FString::SanitizeFloat(m_SaveQuat.Z));
		m_SaveQuatW.Add(FString::SanitizeFloat(m_SaveQuat.W));
	}
}

//速度更新処理
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	Super::UpdateSpeed(DeltaTime);

	if (m_isControl)
	{
		m_SaveVelocityX.Add(FString::SanitizeFloat(m_Velocity.X));
		m_SaveVelocityY.Add(FString::SanitizeFloat(m_Velocity.Y));
		m_SaveVelocityZ.Add(FString::SanitizeFloat(m_Velocity.Z));

	}
	else
	{
		if (CGameUtility::SetDecimalTruncation(m_Velocity, 3).GetAbsMax() != 0.f)
		{
			m_Velocity *= m_Deceleration;
		}
	}

	AddActorWorldOffset(m_Velocity * MOVE_CORRECTION, true);
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

	//レイの開始点と終点を設定(ドローンの座標から前方に向かって)
	float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
	FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

	FVector Start = GetActorLocation();
	FVector End = Start + BodyQuat.GetForwardVector() * 1500.f;
	//ヒット結果を格納する配列
	TArray<FHitResult> OutHits;
	//トレースする対象(自身は対象から外す)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	//レイを飛ばし、WorldStaticのコリジョンチャンネルを持つオブジェクトのヒット判定を取得する
	bool isHit = GetWorld()->LineTraceMultiByObjectType(OutHits, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool isClimbingSlope = false;
	//レイがヒットしたらアクターのタグを確認
	if (isHit)
	{
		//Slopeのタグを持つアクターがあればカメラを上げるフラグを立てる
		for (const FHitResult& HitResult : OutHits)
		{
			if (HitResult.GetActor())
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Slope")))
				{
					isClimbingSlope = true;

					//傾斜との距離を測定する
					m_DistanceToSlope = FVector::Dist(GetActorLocation(), HitResult.Location);
					break;
				}
			}
		}
	}

#ifdef DEBUG_UpdateCamera
	FColor LineColor = isClimbingSlope ? FColor::Yellow : FColor::Blue;
	//デバッグ用のラインを描画
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.f);

#endif // DEBUG_UpdateCamera
	FRotator CameraRotation = m_pCamera->GetRelativeRotation();
	if (FMath::Abs(m_AxisValuePerFrame.X) > 0.2f)
	{
		if (FMath::Abs(m_pCamera->GetRelativeRotation().Roll) < FMath::Abs(m_pBodyMesh->GetRelativeRotation().Roll))
		{
			CameraRotation.Roll += m_AxisValuePerFrame.X * 5.f * DeltaTime;
		}
	}
	else
	{
		CameraRotation.Roll *= 0.94f;
	}

	//レイが傾斜に当たっていたら、現在の高さと傾斜との距離から勾配を求める
	if (isClimbingSlope)
	{
		float radSlope = FMath::Atan2(m_HeightFromGround, m_DistanceToSlope);
		float degSlope = FMath::ClampAngle(FMath::RadiansToDegrees(radSlope), SLOPE_MIN, m_TiltLimit);
		FRotator NewPRotation = FMath::RInterpTo(
			FRotator(CameraRotation.Pitch, 0.f, 0.f),
			FRotator(degSlope, 0.f, 0.f),
			DeltaTime,
			4.f);
		CameraRotation.Pitch = NewPRotation.Pitch;

		m_pSpringArm->SocketOffset.Z = FMath::Lerp(0.f, -m_TiltLimit, FMath::Clamp(NewPRotation.Pitch / m_TiltLimit, 0.f, 1.f));
	}
	else
	{
		CameraRotation.Pitch *= 0.96f;
		m_pSpringArm->SocketOffset.Z *= 0.96f;
	}

	//カメラの回転を更新
	m_pCamera->SetRelativeRotation(CameraRotation * MOVE_CORRECTION);
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, m_pBodyMesh->GetRelativeRotation().Yaw, 0.f) * MOVE_CORRECTION);

	//ソケットの位置を更新
	FVector NewSocketOffset = FVector(m_AxisAccel.Y, m_AxisAccel.X, 0.f) * m_CameraSocketOffsetMax / m_WingAccelMax;
	m_pSpringArm->SocketOffset.X = NewSocketOffset.X;
	m_pSpringArm->SocketOffset.Y = NewSocketOffset.Y;

	//移動量に応じて視野角を変更
	m_pCamera->SetFieldOfView(90.f - m_AxisAccel.Y * 10.f);
}

//カメラとの遮蔽物のコリジョン判定
void  APlayerDrone::UpdateCameraCollsion()
{

}

//風のエフェクトの更新処理
void APlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	Super::UpdateWindEffect(DeltaTime);
}

//レースの座標保存
void APlayerDrone::WritingRaceVector()
{
	//テキストファイル書き込み
	FFileHelper::SaveStringArrayToFile(m_SaveVelocityX, *(FPaths::ProjectDir() + FString("Record/VX.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveVelocityY, *(FPaths::ProjectDir() + FString("Record/VY.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveVelocityZ, *(FPaths::ProjectDir() + FString("Record/VZ.txt")));
}

//レースのクオータニオン
void APlayerDrone::WritingRaceQuaternion()
{
	//テキストファイル書き込み
	FFileHelper::SaveStringArrayToFile(m_SaveQuatX, *(FPaths::ProjectDir() + FString("Record/QX.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveQuatY, *(FPaths::ProjectDir() + FString("Record/QY.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveQuatZ, *(FPaths::ProjectDir() + FString("Record/QZ.txt")));
	FFileHelper::SaveStringArrayToFile(m_SaveQuatW, *(FPaths::ProjectDir() + FString("Record/QW.txt")));
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
		m_AxisValue.Z = CGameUtility::SetDecimalTruncation(m_AxisValue.Z, 3);
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
		m_AxisValue.Y = CGameUtility::SetDecimalTruncation(m_AxisValue.Y, 3);
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
		m_AxisValue.X = CGameUtility::SetDecimalTruncation(m_AxisValue.X, 3);
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
		m_AxisValue.W = CGameUtility::SetDecimalTruncation(m_AxisValue.W, 3);
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