//------------------------------------------------------------------------
// ファイル名		:DroneBase.cpp
// 概要				:ドローンのベースクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/04/19 プレイヤーとエネミーの共通項の追加
//------------------------------------------------------------------------

//インクルード
#include "DroneBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameUtility.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif // WITH_EDITOR

//コンストラクタ
ADroneBase::ADroneBase()
	: m_pBodyMesh(NULL)
	, m_pDroneCollision(NULL)
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
	, m_AxisValuePerFrame(FVector4(0.f, 0.f, 0.f, 0.f))
	, m_LocalAxis(FVector::ZeroVector)
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
	, m_pCloudOfDustEmitter(NULL)
	, m_ShowEffectDistance(50.f)
	, m_GroundMaterialName(TEXT(""))
{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	//メッシュアセットのセットアップ
	MeshAssetSetup();
	//コリジョン初期設定
	InitializeCollision();
	//メッシュの初期設定
	(GetLocalRole() == ENetRole::ROLE_Authority) ? InitializeMesh() : Client_InitializeMesh_Implementation();
	
	//タグを追加
	Tags.Add(TEXT("Drone"));

	//プレイヤーが持つコントロール権
	Role = ROLE_Authority;
	//同期対象フラグ
	bReplicates = true;
	//所有権を持つクライアントのみに同期する
	bOnlyRelevantToOwner = false;
}

//デストラクタ
ADroneBase::~ADroneBase()
{
	for (int32 index = m_pWings.Num() - 1; index >= 0; --index)
	{
		if (m_pWings.IsValidIndex(index))
		{
			m_pWings[index].Reset();
		}
	}
}

//ゲーム開始時に1度だけ処理
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();

	//ライトの初期設定
	(GetLocalRole() == ENetRole::ROLE_Authority) ? InitializeLight() : Client_InitializeLight_Implementation();

	if (m_pDroneCollision)
	{
		//オーバーラップ、ヒット時のイベント関数をバインド
		m_pDroneCollision->OnComponentBeginOverlap.AddDynamic(this, &ADroneBase::OnDroneCollisionOverlapBegin);
		m_pDroneCollision->OnComponentHit.AddDynamic(this, &ADroneBase::OnDroneCollisionHit);
	}
}

//レプリケートを登録
void ADroneBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADroneBase, m_isControl);
}

//毎フレーム処理
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//【入力バインド】コントローラー入力設定
void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//メッシュアセットのセットアップ
void ADroneBase::MeshAssetSetup()
{
	m_BodyMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/NewDrone/Drone.Drone"))).LoadSynchronous();

	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST"))).LoadSynchronous());
	m_WingMesh.Add(TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST"))).LoadSynchronous());
}

//コリジョンの初期設定
void ADroneBase::InitializeCollision()
{
	//ドローンの当たり判定生成
	m_pDroneCollision = CreateDefaultSubobject<USphereComponent>(TEXT("DroneCollision"));
	if (m_pDroneCollision)
	{
		RootComponent = m_pDroneCollision;
		m_pDroneCollision->SetSphereRadius(8.f);
	}
}

//メッシュの初期設定
void ADroneBase::InitializeMesh()
{
	if (!m_pDroneCollision) { return; }

	//ボディメッシュ生成
	m_pBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	if (m_pBodyMesh && m_BodyMesh)
	{
		//メッシュのアタッチ
		m_pBodyMesh->AttachToComponent(m_pDroneCollision, FAttachmentTransformRules::KeepRelativeTransform);
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

//クライアント用　InitializeMesh
void ADroneBase::Client_InitializeMesh_Implementation()
{
	InitializeMesh();
}

//ライトの初期設定
void ADroneBase::InitializeLight()
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

//クライアント用 ライトの初期設定
void ADroneBase::Client_InitializeLight_Implementation()
{
	InitializeLight();
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

//回転処理
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
	//NULLチェック
	if (!m_pBodyMesh) { return; }

	//羽の回転量からドローンの角速度の最大値を設定
	FVector AngularVelocity;
	AngularVelocity = FVector(
		(m_pWings[EWING::LEFT_FORWARD]->AccelState + m_pWings[EWING::LEFT_BACKWARD]->AccelState) - (m_pWings[EWING::RIGHT_FORWARD]->AccelState + m_pWings[EWING::RIGHT_BACKWARD]->AccelState),
		(m_pWings[EWING::LEFT_BACKWARD]->AccelState + m_pWings[EWING::RIGHT_BACKWARD]->AccelState) - (m_pWings[EWING::LEFT_FORWARD]->AccelState + m_pWings[EWING::RIGHT_FORWARD]->AccelState),
		(m_pWings[EWING::RIGHT_FORWARD]->AccelState + m_pWings[EWING::LEFT_BACKWARD]->AccelState) - (m_pWings[EWING::LEFT_FORWARD]->AccelState + m_pWings[EWING::RIGHT_BACKWARD]->AccelState));
	AngularVelocity.Z = FMath::Abs(AngularVelocity.Z) * m_AxisAccel.W;

	FRotator BodyRotation = m_pBodyMesh->GetRelativeRotation();

	float deg = 25.f;
	float RotationSpeed = FMath::Clamp(DeltaTime * 3.f, 0.f, 1.f);
	BodyRotation.Pitch = FMath::Lerp(BodyRotation.Pitch, m_AxisValuePerFrame.Y * deg, RotationSpeed);
	float HorizontalAxis = (FMath::Abs(m_AxisValuePerFrame.X) > FMath::Abs(m_AxisValuePerFrame.W) ? m_AxisValuePerFrame.X : m_AxisValuePerFrame.W);
	BodyRotation.Roll = FMath::Lerp(BodyRotation.Roll, HorizontalAxis * deg, RotationSpeed);

	FRotator NewRotation = BodyRotation;
	NewRotation.Yaw += AngularVelocity.Z * 0.7f;
	m_pBodyMesh->SetRelativeRotation(NewRotation.Quaternion() * MOVE_CORRECTION, true);
}

//速度更新処理
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	if (!m_pBodyMesh) { return; }

	float RotYaw = m_pBodyMesh->GetComponentRotation().Yaw;
	FQuat BodyQuat = FRotator(0.f, RotYaw, 0.f).Quaternion();

	m_LocalAxis = FVector::ZeroVector;
	FVector NormalizeValue = FVector(m_AxisAccel).GetAbs().GetSafeNormal();
	m_LocalAxis += BodyQuat.GetRightVector() * m_AxisAccel.X;
	m_LocalAxis += BodyQuat.GetForwardVector() * -m_AxisAccel.Y;
	m_LocalAxis += BodyQuat.GetUpVector() * m_AxisAccel.Z;

	//ドローンにかかる力の設定
	m_Velocity = CGameUtility::SetDecimalTruncation(m_LocalAxis * m_Speed, 3);

	//高度上限を超えていたら自動的に高度を下げる
	if (m_HeightFromGround >= m_HeightMax)
	{
		m_Velocity.Z = -3.f;
	}
}

//風のエフェクト更新処理
void ADroneBase::UpdateWindEffect(const float& DeltaTime)
{
	if (!m_pWindEmitter || !m_pBodyMesh) { return; }

	//エフェクトとドローンの座標を取得
	FVector EffectLocation = m_pWindEmitter->GetComponentLocation();
	FVector  DroneLocation = m_pBodyMesh->GetComponentLocation();
	//エフェクトが進行方向へ向くようにする
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(DroneLocation - EffectLocation).Rotator();
	//移動量の大きさからエフェクトの不透明度を設定
	float AxisValue = FVector2D(m_AxisValuePerFrame.X, m_AxisValuePerFrame.Y).GetSafeNormal().Size();
	//加速率を計算
	float AccelRate = FMath::Clamp(m_AxisAccel.Size3() / m_WingAccelMax, 0.f, 1.f);
	float Opacity = (m_AxisValuePerFrame.Y < 0.f) ? AxisValue * (m_bIsPassedRing ? 1.f : 0.8f) : 0.f;
	m_WindOpacity = FMath::Lerp(m_WindOpacity, Opacity, DeltaTime * 5.f);
	float WindNoise = (AxisValue != 0.f ? (m_bIsPassedRing ? 8.f : 18.f): 40.f);
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
void ADroneBase::UpdateAltitudeCheck()
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

					if(pHitActor->ActorHasTag(TEXT("LandScape")))
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
void ADroneBase::UpdateCloudOfDustEffect()
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
					m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pDroneCollision, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
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
				m_pCloudOfDustEmitter = UNiagaraFunctionLibrary::SpawnSystemAttached(pNiagaraSystem, m_pDroneCollision, NAME_None, EmitterLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
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

//ボディメッシュの回転設定
void ADroneBase::SetBodyMeshRotation(const FRotator& NewRotator)
{
	if (!m_pBodyMesh) { return; }

	 m_pBodyMesh->SetRelativeRotation(NewRotator); 
}

//ボディメッシュの回転設定
void ADroneBase::SetBodyMeshRotation(const FQuat& NewRotator)
{
	if (!m_pBodyMesh) { return; }

	m_pBodyMesh->SetRelativeRotation(NewRotator);
}

//ボディメッシュの回転量取得
FRotator ADroneBase::GetBodyMeshRotation()const
{
	if (!m_pBodyMesh) { return FRotator::ZeroRotator; }

	return m_pBodyMesh->GetComponentRotation(); 
}

//ボディメッシュの回転量取得
FRotator ADroneBase::GetBodyMeshRelativeRotation()const
{
	if (!m_pBodyMesh) { return FRotator::ZeroRotator; }

	return m_pBodyMesh->GetRelativeRotation();
}

//オーバーラップ開始時に呼ばれる処理
void ADroneBase::OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
#ifdef DEBUG_CollisionOverlap_Begin
	UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));
#endif // DEBUG_OVERLAP_BEGIN
}

//ドローンの当たり判定にオブジェクトがヒットした時呼ばれるイベント関数を登録
void ADroneBase::OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (m_pBodyMesh && OtherActor && OtherActor != this)
	{
		//ドローンの向きを取得
		FQuat Quat = FRotator(0.f, m_pBodyMesh->GetComponentRotation().Yaw + 90.f, 0.f).Quaternion();

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