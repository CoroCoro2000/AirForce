//------------------------------------------------------------------------
// ファイル名		:DroneBase.cpp
// 概要				:ドローンのベースクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/04/19 プレイヤーとエネミーの共通項の追加
//------------------------------------------------------------------------

//インクルード
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameUtility.h"

//コンストラクタ
ADroneBase::ADroneBase()
	: m_GameMode(EGAMEMODE::GAMEMODE_TPS)
	, m_DroneMode(EDRONEMODE::DRONEMODE_AUTOMATICK)
	, m_pBodyMesh(NULL)
	, m_pDroneCollision(NULL)
	, m_Wings{}
	, m_RPSMax(10.f)
	, m_WingAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_MoveDirectionFlag()
	, m_StateFlag()
	, m_TiltLimit(45.f)
	, m_Speed(7.f)
	, m_SpeedPerSecondMax(50.f)
	, m_AxisAccel(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_Acceleration(0.f)
	, m_Deceleration(0.96f)
	, m_Turning(0.6f)
	, m_DroneWeight(0.3f)
	, m_Velocity(FVector::ZeroVector)
	, m_CentrifugalForce(FVector::ZeroVector)
	, m_AngularVelocity(FVector::ZeroVector)
	, m_GravityScale(0.98f)
	, m_Gravity(FVector::ZeroVector)
	, m_DescentTime(0.f)
	, m_pWingRotationSE(NULL)
	, m_RingAcquisition(0)
	, m_HeightMax(400.f)
	, m_HeightFromGround(0.f)
	, m_DistanceToSlope(0.f)
	, m_isControl(false)
	, m_isFloating(true)
	, m_AxisValuePerFrame(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_SaveVelocity(FVector::ZeroVector)
	, m_SaveQuat(FQuat::Identity)
	, m_pWindEffect(NULL)
	, m_WindRotationSpeed(5.f)

{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	//ドローンの当たり判定生成
	m_pDroneCollision = CreateDefaultSubobject<USphereComponent>(TEXT("DroneCollision"));
	if (m_pDroneCollision)
	{
		RootComponent = m_pDroneCollision;
		m_pDroneCollision->SetSphereRadius(13.f);
	}

	//ボディのメッシュアセットを探索
	ConstructorHelpers::FObjectFinder<UStaticMesh> pBodyMesh(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Body/CGAXR_BODY.CGAXR_BODY'"));
	//ボディメッシュ生成
	m_pBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	//ボディメッシュの検索、生成に成功したら
	if (m_pBodyMesh && pBodyMesh.Succeeded())
	{
		//メッシュのセットアップ
		m_pBodyMesh->SetupAttachment(m_pDroneCollision);
		m_pBodyMesh->SetStaticMesh(pBodyMesh.Object);
	}

	//羽のメッシュアセットを探索
	//右ねじと左ねじの羽を取得する
	ConstructorHelpers::FObjectFinder<UStaticMesh> pRightTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> pLeftTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST'"));
	
	//羽のメッシュ検索に成功したら羽の生成処理
	if (!pRightTwistWing.Succeeded() || !pLeftTwistWing.Succeeded()) { return; }
	for (int index = 0; index < EWING::NUM; ++index)
	{
		//右回りの羽を調べる
		const bool isRightTrun = (index == 0 || index == 3) ? true : false;

		const FName WingName = isRightTrun ?
			(index + 1 < 3) ? "LF_Wing" : "RB_Wing" :
			(index + 1 > 2) ? "LB_Wing" : "RF_Wing";
		FRotator InitRotaion = FRotator::ZeroRotator;
		InitRotaion.Yaw = (index < 2) ?
			(index == 0) ? -45.f : 45.f :
			(index == 2) ? 45.f : -45.f;

		//配列の追加(識別番号、羽のメッシュ)
		m_Wings[index] = FWing(FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName)));

		if (m_Wings[index].GetWingMesh())
		{
			//羽のメッシュを設定
			m_Wings[index].GetWingMesh()->SetStaticMesh((isRightTrun ? pLeftTwistWing.Object : pRightTwistWing.Object));
			//ボディにアタッチする
			m_Wings[index].GetWingMesh()->SetupAttachment(m_pBodyMesh);
			//羽のメッシュコリジョンを無効にする
			m_Wings[index].GetWingMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//ソケットの位置に羽をアタッチ
			m_Wings[index].GetWingMesh()->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, WingName);
			m_Wings[index].GetWingMesh()->SetRelativeRotation(InitRotaion);
		}
	}
}

//ゲーム開始時に1度だけ処理
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pDroneCollision)
	{
		//オーバーラップ、ヒット時のイベント関数をバインド
		m_pDroneCollision->OnComponentBeginOverlap.AddDynamic(this, &ADroneBase::OnDroneCollisionOverlapBegin);
		m_pDroneCollision->OnComponentHit.AddDynamic(this, &ADroneBase::OnDroneCollisionHit);
	}

	//質量*重力加速度を重力に設定
	m_Gravity = FVector(0.f, 0.f, m_DroneWeight * m_GravityScale);
}

//毎フレーム処理
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ステート更新処理
	//UpdateState();
	// 
	//羽の加速度更新処理
	UpdateWingAccle(DeltaTime);

	//羽の回転更新処理
	UpdateWingRotation(DeltaTime);

	//回転処理
	UpdateRotation(DeltaTime);

	//速度更新処理
	UpdateSpeed(DeltaTime);

}

//【入力バインド】コントローラー入力設定
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//ステート更新処理
void ADroneBase::UpdateState()
{

}

//羽の加速度更新処理
void ADroneBase::UpdateWingAccle(const float& DeltaTime)
{
}

//羽の回転更新処理
void ADroneBase::UpdateWingRotation(const float& DeltaTime)
{
	//2軸の入力量を合成する
	const float InputValueSize = FMath::Clamp((
		FVector2D(m_AxisValuePerFrame.W, m_AxisValuePerFrame.Z).Size() +
		FVector2D(m_AxisValuePerFrame.X, m_AxisValuePerFrame.Y).Size()) / 2,
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

//回転処理
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
	//NULLチェック
	if (!m_pBodyMesh) { return; }

	//羽の回転量からドローンの角速度の最大値を設定
	m_AngularVelocity = FVector(
		(m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState),
		(m_Wings[EWING::LEFT_BACKWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_FORWARD].AccelState),
		(m_Wings[EWING::RIGHT_FORWARD].AccelState + m_Wings[EWING::LEFT_BACKWARD].AccelState) - (m_Wings[EWING::LEFT_FORWARD].AccelState + m_Wings[EWING::RIGHT_BACKWARD].AccelState));
	m_AngularVelocity.Z = FMath::Abs(m_AngularVelocity.Z) * m_AxisAccel.W;

	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();

	BodyRotation.Pitch += m_AngularVelocity.Y;
	BodyRotation.Roll += m_AngularVelocity.X;

	//	オートマチックで操作するとき
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float deg = 25.f;

		if (m_AngularVelocity.Y != 0.f)
		{
			//前後にドローンが傾きすぎないように補正
			if (BodyRotation.Pitch > deg)
			{
				BodyRotation.Pitch = deg;
			}
			else if (BodyRotation.Pitch < -deg)
			{
				BodyRotation.Pitch = -deg;
			}
		}
		else
		{
			if (FMath::Abs(CGameUtility::SetDecimalTruncation(BodyRotation.Pitch, 3)) != 0.f)
			{
				BodyRotation.Pitch *= m_Deceleration;
			}
		}

		if (m_AngularVelocity.X != 0.f)
		{

			if (BodyRotation.Roll > deg)
			{
				BodyRotation.Roll = deg;
			}
			else if (BodyRotation.Roll < -deg)
			{
				BodyRotation.Roll = -deg;
			}
		}
		else
		{
			if (FMath::Abs(CGameUtility::SetDecimalTruncation(BodyRotation.Roll, 3)) != 0.f)
			{
				BodyRotation.Roll *= m_Deceleration;
			}
			else
			{
				BodyRotation.Roll = 0.f;
			}
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
	//FQuat qAngularVelocity = FQuat::MakeFromEuler(m_AngularVelocity);
	//m_pBodyMesh->AddLocalRotation(qAngularVelocity * MOVE_CORRECTION, true);

	FRotator NewRotation = BodyRotation;
	NewRotation.Yaw += m_AngularVelocity.Z;
	m_pBodyMesh->SetRelativeRotation(NewRotation.Quaternion() * MOVE_CORRECTION, true);
	m_SaveQuat = m_pBodyMesh->GetRelativeRotation().Quaternion();
}

//速度更新処理
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	//オートマチックで操作するとき
	if (m_DroneMode == EDRONEMODE::DRONEMODE_AUTOMATICK)
	{
		float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
		FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

		m_Velocity = FVector::ZeroVector;

		m_Velocity += BodyQuat.GetRightVector() * m_Speed * m_AxisAccel.X * (IsReverseInput(m_AxisAccel.X, m_AxisValuePerFrame.X) ? m_Turning : 1.f);
		m_Velocity += BodyQuat.GetForwardVector() * m_Speed * -m_AxisAccel.Y * (IsReverseInput(m_AxisAccel.Y, m_AxisValuePerFrame.Y) ? m_Turning : 1.f);
		m_Velocity += BodyQuat.GetUpVector() * m_Speed * m_AxisAccel.Z * (IsReverseInput(m_AxisAccel.Z, m_AxisValuePerFrame.Z) ? m_Turning : 1.f);

		//上限でクランプ
		m_Velocity = CGameUtility::SetDecimalTruncation(m_Velocity, 3);
		//高度上限を超えていたら自動的に高度を下げる
		if (IsOverHeightMax())
		{
			m_Velocity.Z = -3.f;
		}

	}

	//マニュアルで操作するとき
	else
	{
		const FVector Direction = m_pBodyMesh->GetUpVector();
		//浮力の大きさを測る
		float Buoyancy = 0.f;
		for (const FWing& wing : m_Wings)
		{
			Buoyancy += wing.AccelState;
		}
		Buoyancy /= (float)EWING::NUM;

#ifdef DEGUG_ACCEL
		UE_LOG(LogTemp, Warning, TEXT("Buoyancy:%f"), Buoyancy);
#endif

		//浮力がホバリング状態より大きいとき
		if (Buoyancy > 0.f)
		{
			if (m_Acceleration < 5.f)
			{
				m_Acceleration += Buoyancy * DeltaTime;
			}
		}
		//浮力がホバリング状態より小さい時
		else if (Buoyancy < 0.f)
		{
			if (m_Acceleration > -2.f)
			{
				m_Acceleration += Buoyancy * DeltaTime;
			}
		}
		//浮力が重力と釣り合う時(ホバリング状態)
		else if (Buoyancy == 0.f)
		{
			m_Acceleration *= 59.f * DeltaTime;
		}

		//推進力の設定
		FVector Propulsion = Direction * (m_Acceleration + m_Gravity.Z);

		//傾きがある時
		if (Direction.Z < 1.f)
		{
			float Centrifugal = 1.f - Direction.Z;
			Propulsion.X += Direction.X * Centrifugal;
			Propulsion.Y += Direction.Y * Centrifugal;
		}

#ifdef DEGUG_ACCEL
		UE_LOG(LogTemp, Warning, TEXT("Propulsion:%s"), *Propulsion.ToString());
#endif
		////重力を抜いた移動量を保持する
		//m_Velocity = Propulsion;
		////重力を加算
		//Propulsion.Z += UpdateGravity(DeltaTime);

		////移動処理
		//AddActorWorldOffset(Propulsion * MOVE_CORRECTION, true);

#ifdef DEGUG_ACCEL
		UE_LOG(LogTemp, Warning, TEXT("Move:%s"), *Propulsion.ToString());
#endif
	}

}

//重力更新処理
float ADroneBase::UpdateGravity(const float& DeltaTime)
{
	float newGravity = m_Gravity.Z;
	const float UpForce = m_pBodyMesh->GetUpVector().Z;
	//上向きの力がない時
	if (UpForce < 0.f)
	{
		//重力加速度を計算
		newGravity = (m_DescentTime * m_DescentTime * (-m_GravityScale) * 0.5f) + (m_DescentTime * m_Velocity.Z);

		//空中にいるなら
		if (m_isFloating)
		{
			//落下時間を増加
			m_DescentTime += DeltaTime;
		}
	}
	else
	{
		m_DescentTime = 0.f;
		newGravity *= -1.f;
	}

#ifdef DEBUG_GRAVITY
	UE_LOG(LogTemp, Warning, TEXT("newGravity%f"), newGravity);
#endif // DEBUG_GRAVITY

	return newGravity;
}

//風のエフェクト更新処理
void ADroneBase::UpdateWindEffect(const float& DeltaTime)
{

	if (m_pWindEffect)
	{
		//エフェクトとドローンの座標を取得
		FVector EffectLocation = m_pWindEffect->GetComponentLocation();
		FVector  DroneLocation = m_pBodyMesh->GetComponentLocation();
		//エフェクトが進行方向へ向くようにする
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - EffectLocation).Rotator();
		//移動量の大きさからエフェクトの不透明度を設定
		float AccelValue = FMath::Clamp(FVector2D(m_Velocity.X, m_Velocity.Y).Size() / SPEED_MAX, 0.f, 1.f);
		float WindOpacity = (m_AxisAccel.Y < 0.f) ? AccelValue * 0.7f : 0.f;
		float WindMask = FMath::Lerp(50.f, 40.f, AccelValue);
		float effectScale = FMath::Lerp(2.f, 1.f, AccelValue);
		float effectLocationX = FMath::Lerp(-40.f, 0.f, AccelValue);

		m_pWindEffect->SetRelativeScale3D(FVector(effectScale));
		m_pWindEffect->SetWorldRotation(LookAtRotation.Quaternion());
		m_pWindEffect->SetRelativeLocation(FVector(effectLocationX, 0.f, 0.f));
		//エフェクトの不透明度を変更
		m_pWindEffect->SetVariableFloat(TEXT("User.Mask"), WindOpacity);
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

//高度の上限をを超えているか確認
bool ADroneBase::IsOverHeightMax()
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

	//レイがヒットしたらアクターのタグを確認し、Groundのタグを持つアクターがあれば高度上限を越えていないのでフラグを降ろす
	if (isHit)
	{
		for (const FHitResult& HitResult : OutHits)
		{
			if (HitResult.GetActor())
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Ground")))
				{
					OverHeightMax = false;
					//地面からの高さを計測
					m_HeightFromGround = FVector::Dist(GetActorLocation(), HitResult.Location);
					break;
				}
			}
		}
	}
#ifdef DEBUG_IsOverHeightMax
	//上限を越えたら黄色、越えていないなら青
	FColor LineColor = OverHeightMax ? FColor::Yellow : FColor::Blue;
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.f);
#endif // DEBUG_IsOverHeightMax

	return OverHeightMax;
}

//オーバーラップ開始時に呼ばれる処理
void ADroneBase::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//タグがPlayerだった場合
		if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			return;
		}

		//タグがRingだった場合
		if (OtherActor->ActorHasTag(TEXT("Ring")))
		{
			m_RingAcquisition++;
		}
	}
#ifdef DEBUG_CollisionOverlap_Begin
	UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
#endif // DEBUG_OVERLAP_BEGIN
}

//ドローンの当たり判定にオブジェクトがヒットした時呼ばれるイベント関数を登録
void ADroneBase::OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		m_isFloating = false;

		if (FVector(m_AxisAccel).GetAbsMax() > 0.1f)
		{

			FVector progressVector = m_AxisAccel;

			//ヒットしたアクターの法線ベクトルを取得
			FVector HitActorNormal = Hit.Normal;

			//進行ベクトルと法線ベクトルの内積を求める
			float dot = progressVector | HitActorNormal;

			//反射ベクトルを求める
			FVector reflectVector = progressVector - dot * 2.f * HitActorNormal;

			//反射ベクトルを進行方向に設定
			m_AxisAccel = reflectVector * 0.5f;
		}



		m_isFloating = true;
	}
}