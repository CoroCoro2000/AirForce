//------------------------------------------------------------------------
// ファイル名		:NetworkPlayerDrone.cpp
// 概要				:ネットワーク用のプレイヤードローンクラス
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#include "NetworkPlayerDrone.h"
#include "Net/UnrealNetwork.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameUtility.h"

// Sets default values
ANetworkPlayerDrone::ANetworkPlayerDrone()
	: m_BodyMesh(nullptr)
	, m_pBodyMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body")))
	, m_WingMesh()
	, m_pWings()
	, m_pSpringArm(CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm")))
	, m_pCamera(CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")))
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
	, m_PlayerId(-1)
	, m_RPSMax(10.f)
	, m_WingAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_TiltLimit(45.f)
	, m_Speed(7.f)
	, m_SpeedPerSecondMax(50.f)
	, m_AxisAccel(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_Acceleration(0.8f)
	, m_Deceleration(1.2f)
	, m_Turning(0.6f)
	, m_Attenuation(0.5f)
	, m_DroneWeight(0.3f)
	, m_Velocity(FVector::ZeroVector)
	, m_pWingRotationSE(NULL)
	, m_HeightMax(400.f)
	, m_HeightFromGround(0.f)
	, m_DistanceToSlope(0.f)
	, m_isControl(false)
	, m_pWindEffect(NULL)
	, m_pWindEmitter(NULL)
	, m_WindRotationSpeed(5.f)
	, m_WindOpacity(0.f)
	, m_WindNoise(15.f)
	, m_bIsPassedRing(false)
	, m_SincePassageCount(0.f)
	, m_CountLimitTime(1.f)
	, m_OverAccelerator(1.5f)
	, m_pLeftSpotLight(NULL)
	, m_pRightSpotLight(NULL)
	, m_pDroneEffects()
	, m_pCloudOfDustEmitter(NULL)
	, m_ShowEffectDistance(50.f)
	, m_GroundMaterialName(TEXT(""))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//メッシュの初期化
	InitializeMesh();
	//カメラの初期化
	InitializeCamera();

	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//プレイヤータグを追加
	Tags.Add(TEXT("Player"));
}

// Called when the game starts or when spawned
void ANetworkPlayerDrone::BeginPlay()
{
	Super::BeginPlay();
	
	//エフェクトの初期設定
	InitializeEmitter();

	if (UCapsuleComponent* pCapsuleComponent = GetCapsuleComponent())
	{
		//オーバーラップ、ヒット時のイベント関数をバインド
		pCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ANetworkPlayerDrone::OnDroneCollisionOverlapBegin);
		pCapsuleComponent->OnComponentHit.AddDynamic(this, &ANetworkPlayerDrone::OnDroneCollisionHit);
	}
}

//レプリケートを登録
void ANetworkPlayerDrone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called every frame
void ANetworkPlayerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	//カメラの更新処理
	UpdateCamera(DeltaTime);

	//カメラとの遮蔽物のコリジョン判定
	UpdateCameraCollsion(DeltaTime);

	//風のエフェクト更新処理
	UpdateWindEffect(DeltaTime);

	//砂煙のエフェクトの更新処理
	UpdateCloudOfDustEffect();
}

//メッシュアセットのセットアップ
void ANetworkPlayerDrone::MeshAssetSetup()
{
	m_BodyMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/NewDrone/Drone.Drone"))).LoadSynchronous();

	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST"))).LoadSynchronous());
	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST"))).LoadSynchronous());
}

//メッシュの初期設定
void ANetworkPlayerDrone::InitializeMesh()
{
	MeshAssetSetup();

	if (m_pBodyMesh && m_BodyMesh)
	{
		//メッシュのアタッチ
		m_pBodyMesh->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		m_pBodyMesh->SetStaticMesh(m_BodyMesh);

		//羽のメッシュコンポーネント生成
		for (int index = 0; index < EWING::NUM; ++index)
		{
			//右回りの羽を調べる
			const bool isRightTrun = (index == 0 || index == 3);
			//羽の名前を設定
			const FName WingName = isRightTrun ?
				(index + 1 < 3) ? TEXT("LF_Wing") : TEXT("RB_Wing") :
				(index + 1 > 2) ? TEXT("LB_Wing") : TEXT("RF_Wing");
			FRotator InitRotaion = FRotator::ZeroRotator;
			InitRotaion.Yaw = (index < 2) ?
				(index == 0) ? -45.f : 45.f :
				(index == 2) ? 45.f : -45.f;

			//配列の追加(識別番号、羽のメッシュ)
			m_pWings.Add(MakeShareable(new FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName))));

			if (m_pWings.IsValidIndex(index))
			{
				if (UStaticMeshComponent* pWingMesh = m_pWings[index]->GetWingMesh())
				{
					if (m_WingMesh.IsValidIndex(0) && m_WingMesh.IsValidIndex(1))
					{
						//羽のメッシュを設定
						pWingMesh->SetStaticMesh(m_WingMesh[isRightTrun]);
						//羽のメッシュコリジョンを無効にする
						pWingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						//ソケットの位置に羽をアタッチ
						pWingMesh->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, WingName);
						pWingMesh->SetRelativeRotation(InitRotaion);
					}
				}
			}
		}
	}
}

//ライトの初期設定
void ANetworkPlayerDrone::InitializeLight()
{
	//ライトコンポーネント生成
	m_pLeftSpotLight = NewObject<USpotLightComponent>(this);
	m_pRightSpotLight = NewObject<USpotLightComponent>(this);

	if (m_pLeftSpotLight && m_pRightSpotLight && m_pBodyMesh)
	{
		m_pLeftSpotLight->RegisterComponent();
		m_pRightSpotLight->RegisterComponent();
		m_pLeftSpotLight->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("LeftSpotLight"));
		m_pRightSpotLight->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightSpotLight"));
		m_pLeftSpotLight->SetOuterConeAngle(20.f);
		m_pRightSpotLight->SetOuterConeAngle(20.f);
		m_pLeftSpotLight->Intensity = 2500.f;
		m_pRightSpotLight->Intensity = 2500.f;
		m_pLeftSpotLight->LightColor = FColor(51, 153, 204);
		m_pRightSpotLight->LightColor = FColor(51, 153, 204);
		m_pLeftSpotLight->SetRelativeRotation(FRotator(0.f, -20.f, 0.f));
		m_pRightSpotLight->SetRelativeRotation(FRotator(0.f, 20.f, 0.f));
	}
}

//エフェクトの初期設定
void ANetworkPlayerDrone::InitializeEmitter()
{
	if (!m_pCamera) { return; }

	//風のエフェクト生成
	m_pWindEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(m_pWindEffect, m_pCamera, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
}

//カメラの初期設定
void ANetworkPlayerDrone::InitializeCamera()
{
	if (!m_pCamera) { return; }
	if (!m_pSpringArm) { return; }

	//アタッチ処理
	m_pSpringArm->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	m_pCamera->AttachToComponent(m_pSpringArm, FAttachmentTransformRules::KeepRelativeTransform);
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

//羽の加速度更新処理
void ANetworkPlayerDrone::UpdateWingAccle(const float& DeltaTime)
{
	//各スティックの入力の値をを取得
	FVector2D RightAxis = FVector2D(m_AxisValue.W, m_AxisValue.Z);
	FVector2D LeftAxis = FVector2D(m_AxisValue.X, m_AxisValue.Y);

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

//羽の回転更新処理
void ANetworkPlayerDrone::UpdateWingRotation(const float& DeltaTime)
{
	//2軸の入力量を合成する
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValue.W, m_AxisValue.Z).Size() +
		FVector2D(m_AxisValue.X, m_AxisValue.Y).Size()) / 2,
		0.f, 1.f);

	//毎秒m_rpsMax * WingAccel回分回転するために毎フレーム羽を回す角度を求める
	for (TSharedPtr<FWing> pWing : m_pWings)
	{
		if (pWing.IsValid())
		{
			if (pWing->GetWingMesh())
			{
				//羽の加速度を0から1の範囲に修正し、正規化する
				const float NormalizeAccelSize = FMath::Clamp((pWing->AccelState + 1.f) / 3.f, 0.f, 1.f);
				//正規化した加速度を使って羽の加速の割合を補間する
				const float WingAccel = FMath::Lerp(m_WingAccelMin, m_WingAccelMax, NormalizeAccelSize);
				//右回りの羽か判別する(左前と右後ろの羽が右回りに回転する)
				const bool isTurnRight = (pWing->GetWingNumber() == EWING::LEFT_FORWARD || pWing->GetWingNumber() == EWING::RIGHT_BACKWARD ? true : false);
				//1フレームに回転する角度を求める
				const float angularVelocity = m_RPSMax * 360.f * DeltaTime * WingAccel * (isTurnRight ? 1.f : -1.f) * MOVE_CORRECTION;

				//羽を回転させる
				pWing->GetWingMesh()->AddLocalRotation(FRotator(0.f, angularVelocity, 0.f));

#ifdef DEBUG_WING
				//*デバッグ用*速度に応じて羽の色変更				
				const FVector WingColor = FVector(FLinearColor::LerpUsingHSV(FColor::Blue, FColor::Yellow, NormalizeAccelSize));
				wing.GetWingMesh()->SetVectorParameterValueOnMaterials(TEXT("WingColor"), WingColor);
#endif // DEBUG_WING
			}
		}
	}
}

//入力の加速度更新処理
void ANetworkPlayerDrone::UpdateAxisAcceleration(const float& DeltaTime)
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
	FVector NormalizeValue = m_AxisValue.GetSafeNormal();

	//入力があるとき加速する
	//XYZ軸
	for (int i = 0; i < VECTOR3_COMPONENT_NUM; i++)
	{
		float Acceleration = m_Acceleration;
		float MaxAcceleration = m_WingAccelMax;
		float AttenRate = DeltaTime * (m_AxisValue[i] != 0.f ? Acceleration : m_Deceleration);
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
	const float AttenRate = FMath::Clamp((m_AxisValue.W != 0.f ? DeltaTime * m_Acceleration : 1.f), 0.f, 1.f);
	//入力がある時は徐々に旋回し、ないときは回さない
	m_AxisAccel.W = FMath::Lerp(m_AxisAccel.W, m_AxisValue.W * m_WingAccelMax, AttenRate);
}

//回転処理
void ANetworkPlayerDrone::UpdateRotation(const float& DeltaTime)
{
	//NULLチェック
	if (!m_pBodyMesh) { return; }

	//メッシュの傾きとアクターのYawの回転量を取得
	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();
	FRotator RootRotation = GetActorRotation();

	//最大角度
	const float MaxDeg = 25.f;
	float HorizontalAxis = (FMath::Abs(m_AxisValue.X) > FMath::Abs(m_AxisValue.W) ? m_AxisValue.X : m_AxisValue.W);

	float RotationSpeed = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
	BodyRotation.Pitch = FMath::Lerp(BodyRotation.Pitch, m_AxisValue.Y * MaxDeg, RotationSpeed);
	RootRotation.Yaw = FMath::Lerp(RootRotation.Yaw, RootRotation.Yaw + m_AxisValue.W * MaxDeg, RotationSpeed);
	BodyRotation.Roll = FMath::Lerp(BodyRotation.Roll, HorizontalAxis * MaxDeg, RotationSpeed);


	FRotator YawRotation = FRotator(0.f, 25.f, 0.f);
	BodyRotation.Yaw = RootRotation.Yaw;
	m_pBodyMesh->SetRelativeRotation(BodyRotation.Quaternion(), true);
	SetActorRotation(RootRotation);
}

//速度更新処理
void ANetworkPlayerDrone::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	if (m_isControl)
	{
		float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
		FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

		FVector LocalAxis = FVector::ZeroVector;
		FVector NormalizeValue = FVector(m_AxisAccel).GetAbs().GetSafeNormal();
		LocalAxis += BodyQuat.GetRightVector() * m_AxisAccel.X;
		LocalAxis += BodyQuat.GetForwardVector() * -m_AxisAccel.Y;
		LocalAxis += BodyQuat.GetUpVector() * m_AxisAccel.Z;

		//ドローンにかかる力の設定
		m_Velocity = CGameUtility::SetDecimalTruncation(LocalAxis * m_Speed, 3);

		//高度上限を超えていたら自動的に高度を下げる
		if (m_HeightFromGround >= m_HeightMax)
		{
			m_Velocity.Z = -3.f;
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
void ANetworkPlayerDrone::UpdateCamera(const float& DeltaTime)
{
	//NULLチェック
	if (!m_pCamera || !m_pSpringArm || !m_pBodyMesh) { return; }

	//移動量に応じてカメラのブレを大きくする
	bool isMove = !m_AxisValue.IsNearlyZero3();

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
	m_CameraRotationYaw = FMath::Lerp(0.f, m_AxisValue.W * 5.f, AttenRate.Yaw);
	CameraRotation.Roll = FMath::Lerp(CameraRotation.Roll, BodyRotation.Roll * 0.7f, AttenRate.Roll);

	//ソケットの位置を更新
	FVector SocketAttenRate = FVector(
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 0.8f, 0.f, 1.f),
		FMath::Clamp(DeltaTime * 1.5f, 0.f, 1.f));

	float HorizontalAxis = (FMath::Abs(m_AxisValue.X) > FMath::Abs(m_AxisValue.W) ? m_AxisValue.X : m_AxisValue.W);

	m_pSpringArm->SocketOffset.X = FMath::Lerp(m_pSpringArm->SocketOffset.X, m_AxisValue.Y * m_CameraSocketOffsetMax.X, SocketAttenRate.X);
	m_pSpringArm->SocketOffset.Y = FMath::Lerp(m_pSpringArm->SocketOffset.Y, HorizontalAxis * m_CameraSocketOffsetMax.Y, SocketAttenRate.Y);
	m_pSpringArm->SocketOffset.Z = FMath::Lerp(m_pSpringArm->SocketOffset.Z, -CameraRotation.Pitch * 1.5f, SocketAttenRate.Z);
	m_pSpringArm->TargetArmLength = FMath::Lerp(m_pSpringArm->TargetArmLength, (isClimbingSlope ? 150.f : 90.f), SocketAttenRate.Z);

	//カメラの回転を更新
	m_pCamera->SetRelativeRotation(CameraRotation.Quaternion() * MOVE_CORRECTION);
	m_pSpringArm->SetRelativeRotation(FRotator(0.f, BodyRotation.Yaw + m_CameraRotationYaw, 0.f) * MOVE_CORRECTION);

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

//右のスティック入力から羽の加速度に変換する処理
float ANetworkPlayerDrone::RightInputValueToWingAcceleration(const int _arrayIndex)
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
float ANetworkPlayerDrone::LeftInputValueToWingAcceleration(const int _arrayIndex)
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

//カメラとの遮蔽物のコリジョン判定
void  ANetworkPlayerDrone::UpdateCameraCollsion(const float& DeltaTime)
{
	if (!m_pCamera) { return; }
	if (!m_pSpringArm) { return; }
	if (!m_pBodyMesh) { return; }

	//レイの始点と終点を設定(ドローンからカメラまでの距離)
	FVector DroneLocation = GetActorLocation();
	FVector CameraLocation = m_pCamera->GetComponentLocation();
	//カメラの左右の座標
	float Len = m_pSpringArm->TargetArmLength;
	FVector CameraUp = DroneLocation + (GetActorUpVector() * Len);

	//ヒット結果を格納する配列
	TArray<FHitResult> OutCameraVerticalHits;

	//トレースする対象(自身は対象から外す)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);
	//レイを飛ばし、WorldStaticのコリジョンチャンネルを持つオブジェクトのヒット判定を取得する
	bool isCameraVerticalHit = GetWorld()->LineTraceMultiByObjectType(OutCameraVerticalHits, DroneLocation, CameraUp, ECollisionChannel::ECC_WorldStatic, CollisionParam);

	float Atten = FMath::Clamp(DeltaTime * 7.f, 0.f, 1.f);
	const float OFFSET_MAX = 30.f;

	float ZOffset = 20.f;

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
						ZOffset = FMath::Lerp(-OFFSET_MAX, 20.f, FMath::Clamp(hitResult.Distance / Len, 0.f, 1.f));
						break;
					}
				}
			}
		}
	}

	//カメラのオフセットを変更
	FVector TargetOffset = FVector(0.f, 0.f, ZOffset);
	FVector NewCameraOffset = FMath::Lerp(m_pSpringArm->TargetOffset, TargetOffset, Atten * 0.5f);
	m_pSpringArm->TargetOffset = (NewCameraOffset - TargetOffset).IsNearlyZero(0.3f) ? TargetOffset : NewCameraOffset;
}

//風のエフェクト更新処理
void ANetworkPlayerDrone::UpdateWindEffect(const float& DeltaTime)
{
	if (!m_pWindEmitter || !m_pBodyMesh) { return; }

	//エフェクトとドローンの座標を取得
	FVector EffectLocation = m_pWindEmitter->GetComponentLocation();
	FVector  DroneLocation = m_pBodyMesh->GetComponentLocation();
	//エフェクトが進行方向へ向くようにする
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - EffectLocation).Rotator();
	//移動量の大きさからエフェクトの不透明度を設定
	float AxisValue = FVector2D(m_AxisValue.X, m_AxisValue.Y).GetSafeNormal().Size();
	//加速率を計算
	float AccelRate = FMath::Clamp(m_AxisAccel.Size3() / m_WingAccelMax, 0.f, 1.f);
	float Opacity = (m_AxisValue.Y < 0.f) ? AxisValue * (m_bIsPassedRing ? 1.f : 0.8f) : 0.f;
	m_WindOpacity = FMath::Lerp(m_WindOpacity, Opacity, DeltaTime * 5.f);
	float WindNoise = (AxisValue != 0.f ? (m_bIsPassedRing ? 8.f : 18.f) : 40.f);
	m_WindNoise = FMath::Lerp(m_WindNoise, WindNoise, DeltaTime * 5.f);
	float effectScale = FMath::Lerp(5.f, 3.f, AxisValue);
	float effectLocationX = FMath::Lerp(-40.f, 0.f, AxisValue);

	m_pWindEmitter->SetRelativeScale3D(FVector(effectScale));
	m_pWindEmitter->SetWorldRotation(LookAtRotation.Quaternion());
	m_pWindEmitter->SetRelativeLocation(FVector(effectLocationX, 0.f, 0.f));
	//エフェクトの不透明度を変更
	m_pWindEmitter->SetVariableFloat(TEXT("User.Mask"), m_WindNoise);
	m_pWindEmitter->SetVariableFloat(TEXT("User.WindOpacity"), m_WindOpacity);
}

//高度の上限をを超えているか確認
void ANetworkPlayerDrone::UpdateAltitudeCheck()
{
	//レイの開始点と終点を設定(ドローンの座標から高度の上限の長さ)
	FVector Start = GetActorLocation();
	FVector End = Start;
	End.Z -= m_HeightMax;
	//ヒット結果を格納する配列
	TArray<FHitResult> OutHits;
	//トレースする対象(自身は対象から外す)
	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this);

	//レイを飛ばし、WorldStaticのコリジョンチャンネルを持つオブジェクトのヒット判定を取得する
	bool isHit = GetWorld()->LineTraceMultiByObjectType(OutHits, Start, End, ECollisionChannel::ECC_WorldStatic, CollisionParam);
	bool OverHeightMax = true;

	m_HeightFromGround = m_HeightMax;

	//レイがヒットしたらアクターのタグを確認し、Groundのタグを持つアクターがあれば高度上限を越えていないのでフラグを降ろす
	if (isHit)
	{
		for (const FHitResult& HitResult : OutHits)
		{
			if (AActor* pHitActor = HitResult.GetActor())
			{
				if (pHitActor->ActorHasTag(TEXT("Ground")))
				{
					OverHeightMax = false;
					//地面からの高さを計測
					m_HeightFromGround = HitResult.Distance;

					if (pHitActor->ActorHasTag(TEXT("LandScape")))
					{
						m_GroundMaterialName = TEXT("LandScape");
					}
					else
					{
						//ヒットしたアクターのマテリアル名を取得
						if (UPrimitiveComponent* pHitComp = HitResult.GetComponent())
						{
							if (UMaterialInterface* pMaterial = pHitComp->GetMaterial(0))
							{
								m_GroundMaterialName = pMaterial->GetName();
							}
						}
					}
					break;
				}
			}
		}
	}
}

//砂埃のエフェクトの表示切替
void ANetworkPlayerDrone::UpdateCloudOfDustEffect()
{
	if (!m_isControl) { return; }
	if (!m_pBodyMesh) { return; }

	//地面との距離が近くなったらエフェクトを表示
	if (m_HeightFromGround <= m_ShowEffectDistance)
	{
		//エフェクトを地面の高さに調整する
		FVector EmitterLocation = m_pBodyMesh->GetUpVector() * (-m_HeightFromGround);

		if (m_pCloudOfDustEmitter)
		{
			//地面のマテリアル名に一致するエフェクトを設定
			if (UNiagaraSystem* pNiagaraSystem = m_pDroneEffects.FindRef(m_GroundMaterialName))
			{
				//設定されているエフェクトが違う場合は差し替える
				if (m_pCloudOfDustEmitter->GetAsset() != pNiagaraSystem)
				{
					m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pBodyMesh, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
				}
				//同じエフェクトなら位置調整のみ行う
				else
				{
					m_pCloudOfDustEmitter->SetRelativeLocation(EmitterLocation);
					m_pCloudOfDustEmitter->Activate();
				}
			}
			//エフェクトを発生させないマテリアルの場合はエフェクトを非表示にする
			else
			{
				if (m_pCloudOfDustEmitter->IsActive())
				{
					m_pCloudOfDustEmitter->Deactivate();
				}
			}
		}
		else
		{
			//地面のマテリアル名と一致するエフェクトを設定
			if (UNiagaraSystem* pNiagaraSystem = m_pDroneEffects.FindRef(m_GroundMaterialName))
			{
				m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pBodyMesh, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
			}
		}
	}
	//地面との距離が離れたら砂煙を非表示にする
	else
	{
		if (m_pCloudOfDustEmitter)
		{
			if (m_pCloudOfDustEmitter->IsActive())
			{
				m_pCloudOfDustEmitter->Deactivate();
			}
		}
	}
}

//ドローンの当たり判定にオブジェクトがオーバーラップした時呼ばれるイベント関数を登録
void ANetworkPlayerDrone::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//タグがRingだった場合
		if (OtherActor->ActorHasTag(TEXT("Ring")))
		{
			//加速フラグを立てる
			m_bIsPassedRing = true;

			//加速計測カウンターをリセット
			m_SincePassageCount = 0.f;
		}
	}
}

//ドローンの当たり判定にオブジェクトがヒットした時呼ばれるイベント関数を登録
void ANetworkPlayerDrone::OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		//ドローンの向きを取得
		FQuat Quat = FRotator(0.f, GetActorRotation().Yaw + 90.f, 0.f).Quaternion();

		//衝突したアクターが電車の場合は外側にはじかれるようにする
		if (OtherActor->ActorHasTag(TEXT("Train")))
		{
			FVector LocalReflectVector = Quat.Inverse().RotateVector(Hit.Normal);
			//反射ベクトルを進行方向に設定
			m_AxisAccel = FVector4(LocalReflectVector, m_AxisAccel.W);
		}
		//電車以外は跳ね返りの処理を行う
		else
		{
			//入力軸を取得
			FVector AxisAccle = m_AxisAccel;
			//ワールド座標に変換
			FVector WorldDir = Quat.RotateVector(AxisAccle);
			//反射ベクトルを求める
			FVector ReflectVector = WorldDir - Hit.Normal * (2.f * (WorldDir | Hit.Normal));
			//求めた反射ベクトルを入力軸の座標に変換
			FVector LocalReflectVector = Quat.Inverse().RotateVector(ReflectVector);
			//反射ベクトルを進行方向に設定
			m_AxisAccel = FVector4(LocalReflectVector * m_Attenuation, m_AxisAccel.W);
		}
	}
}

//【入力バインド】コントローラー入力設定
void ANetworkPlayerDrone::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//軸マッピング
	InputComponent->BindAxis(TEXT("Throttle"), this, &ANetworkPlayerDrone::Input_Throttle);
	InputComponent->BindAxis(TEXT("Elevator"), this, &ANetworkPlayerDrone::Input_Elevator);
	InputComponent->BindAxis(TEXT("Aileron"), this, &ANetworkPlayerDrone::Input_Aileron);
	InputComponent->BindAxis(TEXT("Ladder"), this, &ANetworkPlayerDrone::Input_Ladder);
}

//【入力バインド】スロットル(上下)の入力があった場合呼び出される関数
void ANetworkPlayerDrone::Input_Throttle(float _axisValue)
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
void ANetworkPlayerDrone::Input_Elevator(float _axisValue)
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
void ANetworkPlayerDrone::Input_Aileron(float _axisValue)
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
void ANetworkPlayerDrone::Input_Ladder(float _axisValue)
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