//------------------------------------------------------------------------
// ファイル名		:DroneBullet.cpp
// 概要				:ドローンの弾のクラス
// 作成日			:2021/05/17
// 作成者			:19CU0104 池田翔一郎
// 更新内容			:
//------------------------------------------------------------------------


#include "DroneBullet.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
//コンストラクタ
ADroneBullet::ADroneBullet()
	: m_pBulletMesh(NULL)
	, m_pBulletBoxComp(NULL)
	, m_BulletSpeed(0.f)
	, m_DeleteTimeCount(0.f)
	, m_DeleteTimeCountMax(0.f)
	, Start(FVector::ZeroVector)
	,End(FVector::ZeroVector)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//コリジョン生成
	m_pBulletBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BulletComp"));
	if (m_pBulletBoxComp)
	{
		RootComponent = m_pBulletBoxComp;

		// 接触時に行うイベント関数を登録
		m_pBulletBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ADroneBullet::OnOverlapBegin);
	}

	//メッシュ生成
	m_pBulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	if (m_pBulletMesh)
		m_pBulletMesh->SetupAttachment(RootComponent);

	//デバッグ確認用
	FString str = this->GetName();
	UE_LOG(LogTemp, Warning, TEXT("%s: Generated"), *str);
}

//ゲーム開始時に1度だけ処理
void ADroneBullet::BeginPlay()
{
	Super::BeginPlay();
}

//毎フレーム処理
void ADroneBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//	現在のFPS値を取得
	float FPS = 1.0f / DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("%fFPS"), FPS);
	//	処理落ちしても一定速度で弾が移動するように補正
	float moveCorrection = 60.0f / FPS;
	FVector Pos = FMath::Lerp(Start, End, m_DeleteTimeCount/ m_DeleteTimeCountMax);

	//UE_LOG(LogTemp, Warning, TEXT("Strat%s"), *Start.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("End%s"), *End.ToString());

	SetActorLocation(Pos * moveCorrection, true);
	//AddActorWorldOffset(GetActorForwardVector() * m_BulletSpeed * moveCorrection);
	//SetActorRotation(FRotator(GetActorRotation().Roll,GetActorRotation().Yaw + 1.0f, GetActorRotation().Pitch));

	//	一定時間たったら削除する
	m_DeleteTimeCount += DeltaTime;
	if (m_DeleteTimeCount > m_DeleteTimeCountMax)
	{
		//デバッグ確認用
		FString str = this->GetName();
		//UE_LOG(LogTemp, Warning, TEXT("%s: Time elapsed"),*str);
		//m_BulletSpeed = 0.0f;

		Destroy();
	}
}

void ADroneBullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Tags == Tags)
		return;

	else if (OtherActor->ActorHasTag("DestructibleActor"))
	{
		m_pBulletBoxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Block);
		UE_LOG(LogTemp, Error, TEXT("DestructibleActor"));
	}
	///デバッグ確認用
		FString str = GetName();
		FString str2 = OtherActor->GetName();
		UE_LOG(LogTemp, Error, TEXT("%s: Hit a %s"), *str,*str2);

	//Destroy();
}

