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
#include "GameUtility.h"

//コンストラクタ
ADroneBase::ADroneBase()
	: m_pBodyMesh(NULL)
	, m_pDroneCollision(NULL)
	, m_Wings{}
	, m_RPSMax(10.f)
	, m_WingAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_WindSpeed(2.f)
	, m_MoveDirectionFlag()
	, m_StateFlag()
	, m_Speed(0.f)
	, m_SpeedPerSecondMax(50.f)
	, m_AxisAccel(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_Acceleration(0.f)
	, m_Deceleration(0.96f)
	, m_DroneWeight(0.3f)
	, m_Velocity(FVector::ZeroVector)
	, m_CentrifugalForce(FVector::ZeroVector)
	, m_AngularVelocity(FVector::ZeroVector)
	, m_GravityScale(0.98f)
	, m_Gravity(FVector::ZeroVector)
	, m_DescentTime(0.f)
	, m_pWingRotationSE(NULL)
	, m_isControl(false)
	, m_isFloating(true)
	, m_RingAcquisition(0)
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
	UpdateState();

	//回転処理
	//UpdateRotation(DeltaTime);

	//速度更新処理
	UpdateSpeed(DeltaTime);

	//風の範囲のコリジョン判定更新
	UpdateWindRangeSphereTrace();
}

//【入力バインド】コントローラー入力設定
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//羽の加速度更新処理
void ADroneBase::UpdateWingAccle()
{
}

//ステート更新処理
void ADroneBase::UpdateState()
{

}

//回転処理
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
}

//速度更新処理
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

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
	//重力を抜いた移動量を保持する
	m_Velocity = Propulsion;
	//重力を加算
	Propulsion.Z += UpdateGravity(DeltaTime);

	m_Speed = Propulsion.Size();

	//移動処理
	AddActorWorldOffset(Propulsion * MOVE_CORRECTION, true);

#ifdef DEGUG_ACCEL
	UE_LOG(LogTemp, Warning, TEXT("Move:%s"), *Propulsion.ToString());
#endif

}

//羽の回転更新処理
void ADroneBase::UpdateWingRotation(const float& DeltaTime)
{
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

//風の影響を与える範囲の更新
void ADroneBase::UpdateWindRangeSphereTrace()
{
#ifdef DEBUG_COLLISION_WINDRANGE
	//スフィアトレースの当たり判定を表示するワイヤーフレームの色を設定
	FColor SphereColor = FColor::Blue;
#endif //DEBUG_COLLISION_WINDRANGE

	//球状の当たり判定を作成
	FCollisionShape WindRangeSphere = FCollisionShape::MakeSphere(100.f);
	//ヒット結果を格納する配列
	TArray<FHitResult> OutHits;
	//ドローンの座標を球の座標にする
	FVector SpherePosition = GetActorLocation();

	//当たり判定を無視する項目を決める(自身を無視するように設定)
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	//球状のレイを飛ばし、当たったらtrueを返す
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, SpherePosition, SpherePosition, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, WindRangeSphere, CollisionParams);

	//ヒットしたオブジェクトがある場合
	if (isHit && (int)OutHits.Num() > 0)
	{
		//ヒットしたアクターを追加する
		for (int index = 0; index < (int)OutHits.Num(); ++index)
		{
			//ヒットした結果を格納
			FHitResult hitResult = OutHits[index];

			//今ヒットしたアクターが、格納されていない場合
			if (m_pWindRangeActors.Contains(hitResult.GetActor()) == false)
			{
				//ヒットしたアクターのタグ名がTreeなら
				if (hitResult.GetActor()->ActorHasTag("Tree"))
				{
					//追加で保持する
					m_pWindRangeActors.Add(hitResult.GetActor());

					AStaticMeshActor* pStaticMeshActor = Cast<AStaticMeshActor>(hitResult.GetActor());
					//格納したアクターに風の影響を与える
					if (pStaticMeshActor)
					{
						if (pStaticMeshActor->GetStaticMeshComponent())
						{
							pStaticMeshActor->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), 10.f);

#ifdef DEBUG_COLLISION_WINDRANGE
							//当たっていたらワイヤーフレームを黄色にする
							SphereColor = FColor::Yellow;
#endif //DEBUG_COLLISION_WINDRANGE
						}
					}
				}
			}
		}

		//TArrayの検索をしやすくするため、ヒットしたActorのポインタを配列にまとめる
		TArray<AActor*> pTempWindRangeActors;
		for (int index = 0; index < (int)OutHits.Num(); ++index)
		{
			FHitResult hitResult = OutHits[index];
			//ヒットしたアクターのタグ名がTreeなら
			if (hitResult.GetActor()->ActorHasTag(TEXT("Tree")))
			{
				pTempWindRangeActors.Add(hitResult.GetActor());
			}
		}

		//風の範囲内にいたアクターで、今範囲外にいるアクターがある場合
		if ((int)m_pWindRangeActors.Num() > 0)
		{
			for (int index = 0; index < (int)m_pWindRangeActors.Num(); ++index)
			{
				AActor* pCurrentWindRangeActor = m_pWindRangeActors[index];
				//風の範囲内にいたアクターが、今ヒットしたActorの配列に入っていない場合
				if (pTempWindRangeActors.Contains(pCurrentWindRangeActor) == false)
				{
					//風の影響を元に戻す
					AStaticMeshActor* pStaticMeshActor = Cast<AStaticMeshActor>(pCurrentWindRangeActor);
					if (pStaticMeshActor)
					{
						if (pStaticMeshActor->GetStaticMeshComponent())
						{
							pStaticMeshActor->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), 1.f);
						}
					}

					//風の範囲内の配列から削除
					m_pWindRangeActors.Remove(pCurrentWindRangeActor);
				}
			}
		}
#ifdef DEBUG_COLLISION_WINDRANGE_OVERLAPDETA
		UE_LOG(LogTemp, Warning, TEXT("TempWindRangeActors:%i"), pTempWindRangeActors.Num());
#endif // DEBUG_COLLISION_WINDRANGE_OVERLAPDETA

	}
	//風の範囲内にオブジェクトがない場合
	else
	{
		//範囲内にないため、全開放
		if ((int)m_pWindRangeActors.Num() > 0)
		{
			for (int index = 0; index < (int)m_pWindRangeActors.Num(); ++index)
			{
				AActor* pCurrentWindRangeActor = m_pWindRangeActors[index];
				//風の影響を元に戻す
				AStaticMeshActor* pStaticMeshActor = Cast<AStaticMeshActor>(pCurrentWindRangeActor);
				if (pStaticMeshActor)
				{
					if (pStaticMeshActor->GetStaticMeshComponent())
					{
						pStaticMeshActor->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("WindSpeed"), 1.f);
					}
				}

				//風の範囲内の配列から削除
				m_pWindRangeActors.Remove(pCurrentWindRangeActor);
			}
		}
	}

#ifdef DEBUG_COLLISION_WINDRANGE_OVERLAPDETA
	UE_LOG(LogTemp, Warning, TEXT("WindRangeActors:%i"), m_pWindRangeActors.Num());
#endif // DEBUG_COLLISION_WINDRANGE_OVERLAPDETA
#ifdef DEBUG_COLLISION_WINDRANGE
	//当たり判定の球を描画(当たっていれば黄色、当たっていなければ青)
	DrawDebugSphere(GetWorld(), SpherePosition, 100.f, 12, SphereColor, 0.f, 1.f);
#endif //DEBUG_COLLISION_WINDRANGE
}

//オーバーラップ開始時に呼ばれる処理
void ADroneBase::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		//タグがPlayerだった場合
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

		FVector progressVector = m_AxisAccel;

		//ヒットしたアクターの法線ベクトルを取得
		FVector HitActorNormal = Hit.Normal;

		//進行ベクトルと法線ベクトルの内積を求める
		float dot = progressVector | HitActorNormal;

		//反射ベクトルを求める
		FVector reflectVector = progressVector - dot * 2.f * HitActorNormal;

		//反射ベクトルを進行方向に設定
		m_AxisAccel = reflectVector * 0.5f;
		m_isFloating = true;
	}
}