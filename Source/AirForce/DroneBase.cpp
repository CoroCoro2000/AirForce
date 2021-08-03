//------------------------------------------------------------------------
// ファイル名		:DroneBase.cppAcceleration
// 概要				:ドローンのベースクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/04/19 プレイヤーとエネミーの共通項の追加
//------------------------------------------------------------------------

//インクルード
#include "DroneBase.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

//コンストラクタ
ADroneBase::ADroneBase()
	: m_pBodyMesh(NULL)
	, m_rpsMax(10.f)
	, m_WingAccele(0.f)
	, m_WingOldAccele(0.f)
	, m_WingAccelMin(0.75f)
	, m_WingAccelMax(1.5f)
	, m_pDroneBoxComp(NULL)
	, m_MoveDirectionFlag(0)
	, m_StateFlag(0)
	, m_CurrentLocation(FVector::ZeroVector)
	, m_PrevCurrentLocation(FVector::ZeroVector)
	, m_OldRotation(FRotator::ZeroRotator)
	, m_SpeedPerSecondMax(50.f)
	, m_Acceleration(0.f)
	, m_DroneWeight(0.15f)
	, m_Velocity(FVector::ZeroVector)
	, Centrifugalforce(FVector::ZeroVector)
	, m_AngularVelocity(FVector::ZeroVector)
	, Gravity(FVector::ZeroVector)
	, m_GravityScale(9.8f)
	, m_DescentTime(0.f)
	, m_CenterOfGravity(FVector::ZeroVector)
	, m_isControl(false)
	, m_RingAcquisition(0)
{
	//自身のTick()を毎フレーム呼び出すかどうか
	PrimaryActorTick.bCanEverTick = true;

	//ボディのメッシュアセットを探索
	ConstructorHelpers::FObjectFinder<UStaticMesh> pBodyMesh(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Body/CGAXR_BODY.CGAXR_BODY'"));
	//ボディメッシュ生成
	m_pBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	//ボディメッシュの検索、生成に成功したら
	if (m_pBodyMesh && pBodyMesh.Succeeded())
	{
		//メッシュのセットアップ
		RootComponent = m_pBodyMesh;
		m_pBodyMesh->SetStaticMesh(pBodyMesh.Object);
	}


	//羽のメッシュアセットを探索
	//右ねじと左ねじの羽を取得する
	ConstructorHelpers::FObjectFinder<UStaticMesh> pRightTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_RIGHT_TWIST.CGAXR_FAN_RIGHT_TWIST'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> pLeftTwistWing(TEXT("StaticMesh'/Game/Model/Drone/Drone_Mesh/CGAXR_2021_07_31/Wing/CGAXR_FAN_LEFT_TWIST.CGAXR_FAN_LEFT_TWIST'"));
	
	//羽のメッシュ検索に成功したら羽の生成処理
	if (!pRightTwistWing.Succeeded() || !pLeftTwistWing.Succeeded()) { return; }
	for (int index = 0; index < WING_ARRAY_MAX; ++index)
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
		m_pWings.Add(new FWing(index, CreateDefaultSubobject<UStaticMeshComponent>(WingName)));

		if (!m_pWings[index]) { return; }
		if (!m_pWings[index]->GetWingMesh()) { return; }

		//羽のメッシュを設定
		m_pWings[index]->GetWingMesh()->SetStaticMesh((isRightTrun ? pLeftTwistWing.Object : pRightTwistWing.Object));
		//ボディのメッシュにアタッチする
		m_pWings[index]->GetWingMesh()->SetupAttachment(m_pBodyMesh);
		//ソケットの位置に羽をアタッチ
		m_pWings[index]->GetWingMesh()->AttachToComponent(m_pBodyMesh, FAttachmentTransformRules::KeepRelativeTransform, WingName);
		m_pWings[index]->GetWingMesh()->SetRelativeRotation(InitRotaion);
	}
}

//デストラクタ
ADroneBase::~ADroneBase()
{
	for (FWing* pWing : m_pWings)
	{
		//領域の開放
		if (pWing)
		{
			delete pWing;
		}
	}
}

//ゲーム開始時に1度だけ処理
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();
	
	m_pDroneBoxComp->OnComponentBeginOverlap.AddDynamic(this,&ADroneBase::OnOverlapBegin);

	//質量*重力加速度を重力に設定
	Gravity = FVector(0.f, 0.f, m_DroneWeight * m_GravityScale * -1.f);
}

//このオブジェクトが破棄されるときに呼び出される関数
void ADroneBase::BeginDestory()
{
	Super::BeginDestroy();

	for (FWing* pWing : m_pWings)
	{
		//領域の開放
		if (pWing)
		{
			delete pWing;
		}
	}
}

//毎フレーム処理
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ステート更新処理
	UpdateState();

	//重心移動処理
	UpdateCenterOfGravity(DeltaTime);

	//回転処理
	UpdateRotation(DeltaTime);

	//速度更新処理
	UpdateSpeed(DeltaTime);

	//移動処理
	UpdateMove(DeltaTime);

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


//重心移動処理
void ADroneBase::UpdateCenterOfGravity(const float& DeltaTime)
{

}

//回転処理
void ADroneBase::UpdateRotation(const float& DeltaTime)
{
}

//速度更新処理
void ADroneBase::UpdateSpeed(const float& DeltaTime)
{
	//現在と1フレーム前の移動量から１フレーム間の移動量を計算
	m_PrevCurrentLocation = m_CurrentLocation;
	m_CurrentLocation = GetActorLocation();
}

//移動処理
void ADroneBase::UpdateMove(const float& DeltaTime)
{
	//ドローンの傾きを向きベクトルに変換
	const FVector unitDirection = GetActorRotation().Vector();

	//UE_LOG(LogTemp, Warning, TEXT("GetWingNormalizeAccele%f"), GetWingNormalizeAccele());
}

//羽の回転更新処理
void ADroneBase::UpdateWingRotation(const float& DeltaTime)
{
}
//オーバーラップ開始時に呼ばれる処理
void ADroneBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	  //タグがPlayerだった場合
	if (OtherActor->ActorHasTag(TEXT("Ring")))
	{
		m_RingAcquisition++;
	}
}