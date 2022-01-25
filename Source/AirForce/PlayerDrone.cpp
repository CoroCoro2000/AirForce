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
//-----------------------------------------------------------------------

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
#include "Misc/FileHelper.h"
#include "DrawDebugHelpers.h"

//#define DEBUG_CAMERA

//コンストラクタ
APlayerDrone::APlayerDrone()
	: m_pSpringArm(NULL)
	, m_pCamera(NULL)
	, m_CameraTargetLength(90.f)
	, m_FieldOfView(90.f)
	, m_CameraSocketOffset(FVector::ZeroVector)
	, m_CameraSocketOffsetMax(FVector(30.f, 45.f, 45.f))
	, m_CameraMoveLimit(FVector(10.f, 40.f, 20.f))
	, m_CameraRotationAttenRate(FRotator(3.f, 3.f, 2.f))
	, m_MotionBlurAmount(1.5f)
	, m_MotionBlurMax(15.f)
	, m_MotionBlurTargetFPS(8)
	, m_AxisValue(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_StartLocation(FVector::ZeroVector)
	, m_StartQuaternion(FQuat::Identity)
	, m_CameraRotationYaw(0.f)
	, m_bIsOutCourse(false)
	, m_BodyOpacity(1.f)
{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = m_pDroneCollision;

	if (m_pBodyMesh)
	{
		m_pBodyMesh->AttachToComponent(m_pDroneCollision,FAttachmentTransformRules::KeepRelativeTransform);
	}

	//スプリングアーム生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (m_pSpringArm && m_pBodyMesh)
	{
		m_pSpringArm->AttachToComponent(m_pDroneCollision, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//カメラ生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera && m_pSpringArm)
	{
		m_pCamera->AttachToComponent(m_pSpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//カメラの初期設定
	InitializeCamera();

	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//プレイヤータグを追加
	Tags.Add(TEXT("Player"));
}


//ゲーム開始時に1度だけ処理
void APlayerDrone::BeginPlay()
{
	Super::BeginPlay();

	//エフェクトの初期設定
	InitializeEmitter();

	//軸の数だけ配列を用意する
	m_SaveQuatText.Empty();
	m_SaveVelocityText.Empty();
	m_SaveQuatText.SetNum(4);
	m_SaveVelocityText.SetNum(3);

	//初期位置とメッシュの回転を保存
	m_StartLocation = this->GetActorLocation();
	m_StartQuaternion = m_pBodyMesh->GetComponentQuat();
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

	//高度チェック
	UpdateAltitudeCheck();

	//リプレイ更新処理
	UpdateReplay(DeltaTime);

	//カメラの更新処理
	UpdateCamera(DeltaTime);

	//カメラとの遮蔽物のコリジョン判定
	UpdateCameraCollsion(DeltaTime);

	//風のエフェクト更新処理
	UpdateWindEffect(DeltaTime);

	//砂煙のエフェクトの更新処理
	UpdateCloudOfDustEffect();
}

//エフェクトの初期設定
void APlayerDrone::InitializeEmitter()
{
	if (!m_pCamera) { return; }

	//風のエフェクト生成
	m_pWindEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(m_pWindEffect, m_pCamera, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
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
	m_pSpringArm->TargetOffset.Z = 20.f;
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
				//羽の回転量の合成
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
			pWing->AccelState = RightInputValueToWingAcceleration(pWing->GetWingNumber());
		}
	}
	//左スティックのみの場合
	else if (!LeftAxis.IsZero())
	{
		for (TSharedPtr<FWing> pWing : m_pWings)
		{
			pWing->AccelState = LeftInputValueToWingAcceleration(pWing->GetWingNumber());
		}
	}
}

//入力の加速度更新処理
void APlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
{
	//リングをくぐっていたら
	if (m_bIsPassedRing)
	{
		//上限になるまで時間を計測
		if (m_SincePassageCount < m_CountLimitTime)
		{
			m_SincePassageCount += DeltaTime;
		}
		//上限を越えたら、フラグを降ろす
		else
		{
			m_bIsPassedRing = false;
		}
	}

	//入力軸を正規化する(ベクトルの大きさが上限を越えないように)
	FVector NormalizeValue = m_AxisValuePerFrame.GetSafeNormal();

	//入力があるとき加速する
	//XYZ軸
	for (int i = 0; i < VECTOR3_COMPONENT_NUM; i++)
	{
		float Acceleration = m_Acceleration;
		float MaxAcceleration = m_WingAccelMax;
		float AttenRate = DeltaTime * (m_AxisValuePerFrame[i] != 0.f ? Acceleration : m_Deceleration);
		//リングをくぐっていたら加速する
		if (m_bIsPassedRing)
		{
			Acceleration *= m_OverAccelerator;
			MaxAcceleration *= m_OverAccelerator;
			AttenRate = DeltaTime * Acceleration * m_OverAccelerator;
		}

		//入力量に応じて加速度を増減させる
		m_AxisAccel[i] = FMath::Lerp(m_AxisAccel[i], NormalizeValue[i] * MaxAcceleration, FMath::Clamp(AttenRate, 0.f, 1.f));
	}

	//旋回(W軸)
	const float AttenRate = FMath::Clamp((m_AxisValuePerFrame.W != 0.f ? DeltaTime * m_Acceleration : 1.f), 0.f, 1.f);
	//入力がある時は徐々に旋回し、ないときは回さない
	m_AxisAccel.W = FMath::Lerp(m_AxisAccel.W, m_AxisValuePerFrame.W * m_WingAccelMax, AttenRate);
}

//ドローンの回転処理
void APlayerDrone::UpdateRotation(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	if (m_isReplay && !IsEndPlayBackReplay())
	{
		//読み込んだ移動量のテキストファイルをfloatに変換する
		bool IsValidTextArray = true;
		bool IsValidAxisTextArray = true;

		for (int index = 0; index < VECTOR4_COMPONENT_NUM; ++index)
		{
			IsValidTextArray = m_SaveQuatText.IsValidIndex(index);
			IsValidAxisTextArray = m_SaveQuatText[index].IsValidIndex(m_PlaybackFlame);
			if (!IsValidTextArray || IsValidAxisTextArray)
			{
				break;
			}
		}

		FQuat ReplayQuat = FQuat::Identity;

		if (IsValidTextArray && IsValidAxisTextArray)
		{
			ReplayQuat.X = FCString::Atof(*(m_SaveQuatText[0][m_PlaybackFlame]));
			ReplayQuat.Y = FCString::Atof(*(m_SaveQuatText[1][m_PlaybackFlame]));
			ReplayQuat.Z = FCString::Atof(*(m_SaveQuatText[2][m_PlaybackFlame]));
			ReplayQuat.W = FCString::Atof(*(m_SaveQuatText[3][m_PlaybackFlame]));
		}
		m_pBodyMesh->SetWorldRotation(ReplayQuat * MOVE_CORRECTION);
		return; 
	}

	Super::UpdateRotation(DeltaTime);

	//コントロール可能なら回転量を保存する
	if (m_isControl)
	{
		FQuat BodyQuat = m_pBodyMesh->GetComponentQuat();
		bool IsValidTextArray = true;
		for (int index = 0; index < VECTOR4_COMPONENT_NUM; ++index)
		{
			IsValidTextArray = m_SaveQuatText.IsValidIndex(index);
			if (!IsValidTextArray)
			{
				break;
			}
		}

		if (IsValidTextArray)
		{
			m_SaveQuatText[0].Add(FString::SanitizeFloat(BodyQuat.X));
			m_SaveQuatText[1].Add(FString::SanitizeFloat(BodyQuat.Y));
			m_SaveQuatText[2].Add(FString::SanitizeFloat(BodyQuat.Z));
			m_SaveQuatText[3].Add(FString::SanitizeFloat(BodyQuat.W));
		}
	}
}

//速度更新処理
void APlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	Super::UpdateSpeed(DeltaTime);

	//コントロール可能なら移動量を保存する
	if (m_isControl)
	{
		int index = 0;
		for (TArray<FString>& SaveVelocityText : m_SaveVelocityText)
		{
			SaveVelocityText.Add(FString::SanitizeFloat(m_Velocity[index]));
			++index;
		}
	}
	else if (m_isReplay && !IsEndPlayBackReplay())
	{
		//読み込んだ移動量のテキストファイルをfloatに変換する
		int index = 0;
		for (const TArray<FString> SaveVelocityText : m_SaveVelocityText)
		{
			if (SaveVelocityText.IsValidIndex(m_PlaybackFlame))
			{
				m_Velocity[index] = FCString::Atof(*(SaveVelocityText[m_PlaybackFlame]));
			}
			++index;
		}
	}
	else
	{
		if (CGameUtility::SetDecimalTruncation(m_Velocity, 3).GetAbsMax() != 0.f)
		{
			m_Velocity = FMath::Lerp(m_Velocity, FVector::ZeroVector, DeltaTime * m_Deceleration);
		}
	}
	//座標を更新
	AddActorWorldOffset(m_Velocity * MOVE_CORRECTION, true);
}

//カメラ更新処理
void APlayerDrone::UpdateCamera(const float& DeltaTime)
{
	//NULLチェック
	if (!m_pCamera || !m_pSpringArm || !m_pBodyMesh) { return; }

	//移動量に応じてカメラのブレを大きくする
	bool isMove = !m_AxisValuePerFrame.IsNearlyZero3();

	//レイの開始点と終点を設定(ドローンの座標から前方に向かって)
	float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
	FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

	float RayLength = 2000.f;
	FVector Start = GetActorLocation();
	FVector End = Start + BodyQuat.GetForwardVector() * RayLength;
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
			if (AActor* pHitActor = HitResult.GetActor())
			{
				if (pHitActor->ActorHasTag(TEXT("Slope")))
				{
					isClimbingSlope = true;
					//傾斜との距離を測定する
					m_DistanceToSlope = HitResult.Distance;
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

	//カメラと機体の角度を取得
	FRotator CameraRotation = m_pCamera->GetRelativeRotation();
	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();

	//減衰比率を設定(フレーム落ちした際にLerpの上限を越えないように上限を1でクランプする)
	FRotator AttenRate = FRotator(
		FMath::Clamp(DeltaTime * m_CameraRotationAttenRate.Pitch, 0.f, 1.f),
		FMath::Clamp(DeltaTime * m_CameraRotationAttenRate.Yaw, 0.f, 1.f),
		FMath::Clamp(DeltaTime * m_CameraRotationAttenRate.Roll, 0.f, 1.f));

	//斜面を登っているときは見上げるような角度にする
	if (isClimbingSlope)
	{
		float radSlope = FMath::Atan2(m_HeightFromGround, m_DistanceToSlope);
		float degSlope = FMath::ClampAngle(FMath::RadiansToDegrees(radSlope), SLOPE_MIN, m_TiltLimit);
		CameraRotation.Pitch = FMath::Lerp(CameraRotation.Pitch, degSlope, AttenRate.Pitch);
	}
	else
	{
		CameraRotation.Pitch = FMath::Lerp(CameraRotation.Pitch, FMath::Clamp(BodyRotation.Pitch, -5.f, 5.f), AttenRate.Pitch);
	}
	m_CameraRotationYaw = FMath::Lerp(0.f, m_AxisValuePerFrame.W * 5.f, AttenRate.Yaw);
	CameraRotation.Roll = FMath::Lerp(CameraRotation.Roll, BodyRotation.Roll * 0.7f, AttenRate.Roll);

	//ソケットの位置を更新
	FVector SocketAttenRate = FVector(
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 0.8f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f));

	float HorizontalAxis = (FMath::Abs(m_AxisValuePerFrame.X) > FMath::Abs(m_AxisValuePerFrame.W) ? m_AxisValuePerFrame.X : m_AxisValuePerFrame.W);

	m_pSpringArm->SocketOffset.X = FMath::Lerp(m_pSpringArm->SocketOffset.X, m_AxisValuePerFrame.Y * m_CameraSocketOffsetMax.X, SocketAttenRate.X);
	m_pSpringArm->SocketOffset.Y = FMath::Lerp(m_pSpringArm->SocketOffset.Y, HorizontalAxis * m_CameraSocketOffsetMax.Y, SocketAttenRate.Y);
	m_pSpringArm->SocketOffset.Z = FMath::Lerp(m_pSpringArm->SocketOffset.Z, -CameraRotation.Pitch * 1.5f, SocketAttenRate.Z);
	m_pSpringArm->TargetArmLength = FMath::Lerp(m_pSpringArm->TargetArmLength, (isClimbingSlope ? 150.f : 90.f), SocketAttenRate.Z);

	//カメラの回転を更新
	m_pCamera->SetRelativeRotation(CameraRotation.Quaternion() * MOVE_CORRECTION);
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, BodyRotation.Yaw + m_CameraRotationYaw, 0.f) * MOVE_CORRECTION);

	if (!m_isReplay)
	{
		//移動に応じて視野角を変更
		float FOV = isMove ? (m_bIsPassedRing ? 125.f : 105.f) : 90.f;
		float FOVAttenRate = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
		float NewFOV = FMath::Lerp(m_pCamera->FieldOfView, FOV, FOVAttenRate);
		m_pCamera->SetFieldOfView(NewFOV);

		//移動に応じてモーションブラーをかける
		float MotionBlurAmount = isMove ? (m_bIsPassedRing ? m_MotionBlurAmount : m_MotionBlurAmount * 0.8f) : 0.5f;
		float MotionBlurMax = isMove ? (m_bIsPassedRing ? m_MotionBlurMax : m_MotionBlurMax * 0.8f) : 5.f;
		int32 MotionBlurTargetFPS = isMove ? (m_bIsPassedRing ? m_MotionBlurTargetFPS : m_MotionBlurTargetFPS * 0.8f) : 30;
		m_pCamera->PostProcessSettings.MotionBlurAmount = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurAmount, MotionBlurAmount, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurMax = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurMax, MotionBlurMax, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurTargetFPS = MotionBlurTargetFPS;

	}
	else 
	{
		bool isReplayMove = !m_Velocity.IsNearlyZero();
		//移動に応じて視野角を変更
		float FOV = isReplayMove ? (m_bIsPassedRing ? 125.f : 105.f) : 90.f;
		float FOVAttenRate = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
		float NewFOV = FMath::Lerp(m_pCamera->FieldOfView, FOV, FOVAttenRate);
		m_pCamera->SetFieldOfView(NewFOV);

		//移動に応じてモーションブラーをかける
		float MotionBlurAmount = isReplayMove ? (m_bIsPassedRing ? m_MotionBlurAmount : m_MotionBlurAmount * 0.8f) : 0.5f;
		float MotionBlurMax = isReplayMove ? (m_bIsPassedRing ? m_MotionBlurMax : m_MotionBlurMax * 0.8f) : 5.f;
		int32 MotionBlurTargetFPS = isReplayMove ? (m_bIsPassedRing ? m_MotionBlurTargetFPS : m_MotionBlurTargetFPS * 0.8f) : 30;
		m_pCamera->PostProcessSettings.MotionBlurAmount = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurAmount, MotionBlurAmount, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurMax = FMath::Lerp(m_pCamera->PostProcessSettings.MotionBlurMax, MotionBlurMax, FOVAttenRate);
		m_pCamera->PostProcessSettings.MotionBlurTargetFPS = MotionBlurTargetFPS;
	}
}

//カメラとの遮蔽物のコリジョン判定
void  APlayerDrone::UpdateCameraCollsion(const float& DeltaTime)
{
	if (!m_pCamera) { return; }
	if (!m_pSpringArm) { return; }
	if (!m_pBodyMesh) { return; }

	//レイの始点と終点を設定(ドローンからカメラまでの距離)
	FVector DroneLocation = GetActorLocation();
	FVector CameraLocation = m_pCamera->GetComponentLocation();
	//カメラの左右の座標
	float Len = m_pSpringArm->TargetArmLength;
	FVector CameraRightVec = m_pCamera->GetRightVector();
	FVector CameraUp = CameraLocation + (m_pCamera->GetUpVector() * Len);
	FVector CameraLeft = CameraLocation - (CameraRightVec * Len);
	FVector CameraRight = CameraLocation + (CameraRightVec * Len);
	
	//ヒット結果を格納する配列
	FHitResult OutSpringArmHit;
	TArray<FHitResult> OutCameraWidthHits;
	TArray<FHitResult> OutCameraVerticalHits;

	//トレースする対象(自身は対象から外す)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);
	//レイを飛ばし、WorldStaticのコリジョンチャンネルを持つオブジェクトのヒット判定を取得する
	bool isSpringArmHit = GetWorld()->LineTraceSingleByObjectType(OutSpringArmHit, DroneLocation, CameraLocation, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool isCameraWidthHit = GetWorld()->LineTraceMultiByObjectType(OutCameraWidthHits, CameraLeft, CameraRight, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool isCameraVerticalHit = GetWorld()->LineTraceMultiByObjectType(OutCameraVerticalHits, CameraLocation, CameraUp, ECollisionChannel::ECC_WorldStatic, CollisionParam);

	float Atten = FMath::Clamp(DeltaTime * 7.f, 0.f, 1.f);

	FVector TargetOffset = FVector(0.f, 0.f, 20.f);

	//カメラの横判定
	if (isCameraWidthHit)
	{		
		//カメラに重なったオブジェクトを確認
		if (OutCameraWidthHits.Num() > 0)
		{
			for (const FHitResult& hitResult : OutCameraWidthHits)
			{
				if (AActor* pHitActor = hitResult.GetActor())
				{
					//カメラをブロックするオブジェクトがある場合はオフセット位置を調整する
					if (pHitActor->ActorHasTag(TEXT("CameraBlocking")))
					{
					
						break;
					}
				}
			}
		}
	}

	//カメラの縦判定
	if (isCameraVerticalHit)
	{
		//カメラに重なったオブジェクトを確認
		if (OutCameraVerticalHits.Num() > 0)
		{
			for (const FHitResult& hitResult : OutCameraVerticalHits)
			{
				if (AActor* pHitActor = hitResult.GetActor())
				{
					//カメラをブロックするオブジェクトがある場合はオフセット位置を調整する
					if (pHitActor->ActorHasTag(TEXT("CameraBlocking")))
					{
						TargetOffset.Z = FMath::Lerp(-30.f, 20.f, FMath::Clamp(hitResult.Distance / Len, 0.f, 1.f));
						break;
					}
				}
			}
		}
	}

	//カメラのオフセットを変更
	FVector NewCameraOffset = FMath::Lerp(m_pSpringArm->TargetOffset, TargetOffset, Atten * 0.5f);
	m_pSpringArm->TargetOffset = (NewCameraOffset - TargetOffset).IsNearlyZero(0.3f) ? TargetOffset : NewCameraOffset;
}

//風のエフェクトの更新処理
void APlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	Super::UpdateWindEffect(DeltaTime);
}

//リプレイの初期設定
void APlayerDrone::InitializeReplay()
{
	SetActorLocation(m_StartLocation);
	m_pBodyMesh->SetWorldRotation(m_StartQuaternion);
	m_PlaybackFlame = 0;
	m_isReplay = true;
}
//リプレイ更新処理
void APlayerDrone::UpdateReplay(const float& DeltaTime)
{
	if (!m_isReplay || IsEndPlayBackReplay()) { return; }

	m_PlaybackFlame++;
}
//レースの座標保存
void APlayerDrone::WritingBestRaceVector()
{
	if (m_SaveVelocityText.Num() != m_SaveVelocityLoadPath.Num()) { return; }

	//テキストファイル書き込み
	int index = 0;
	for (const TArray<FString>& SaveVelocityText : m_SaveVelocityText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::BEST] + m_SaveVelocityLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveVelocityText, *FliePath);
		++index;
	}
}

//レースのクオータニオン保存
void APlayerDrone::WritingBestRaceQuaternion()
{
	if (m_SaveQuatText.Num() != m_SaveQuatLoadPath.Num()) { return; }

	//テキストファイル書き込み
	int index = 0;
	for (const TArray<FString> SaveQuatText : m_SaveQuatText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::BEST] + m_SaveQuatLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveQuatText, *FliePath);
		++index;
	}
}

//リプレイのレースの座標ファイル書き込み
void APlayerDrone::WritingReplayRaceVector()
{
	if (m_SaveVelocityText.Num() != m_SaveVelocityLoadPath.Num()) { return; }

	//テキストファイル書き込み
	int index = 0;
	for (const TArray<FString>& SaveVelocityText : m_SaveVelocityText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::REPLAY] + m_SaveVelocityLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveVelocityText, *FliePath);
		++index;
	}
}

//リプレイのレースのクオータニオンファイル書き込み
void APlayerDrone::WritingReplayRaceQuaternion()
{
	if (m_SaveQuatText.Num() != m_SaveQuatLoadPath.Num()) { return; }

	//テキストファイル書き込み
	int index = 0;
	for (const TArray<FString> SaveQuatText : m_SaveQuatText)
	{
		FString FliePath = FPaths::ProjectDir() + m_SaveRecordFolderPath + m_SaveStageFolderPath + m_SaveTypeFolderPath[EPATH_DRONE::REPLAY] + m_SaveQuatLoadPath[index];
		FFileHelper::SaveStringArrayToFile(SaveQuatText, *FliePath);
		++index;
	}
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
}