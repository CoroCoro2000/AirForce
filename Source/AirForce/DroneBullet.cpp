//------------------------------------------------------------------------
// �t�@�C����		:DroneBullet.cpp
// �T�v				:�h���[���̒e�̃N���X
// �쐬��			:2021/05/17
// �쐬��			:19CU0104 �r�c�Ĉ�Y
// �X�V���e			:
//------------------------------------------------------------------------


#include "DroneBullet.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
//�R���X�g���N�^
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

	//�R���W��������
	m_pBulletBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BulletComp"));
	if (m_pBulletBoxComp)
	{
		RootComponent = m_pBulletBoxComp;

		// �ڐG���ɍs���C�x���g�֐���o�^
		m_pBulletBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ADroneBullet::OnOverlapBegin);
	}

	//���b�V������
	m_pBulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	if (m_pBulletMesh)
		m_pBulletMesh->SetupAttachment(RootComponent);

	//�f�o�b�O�m�F�p
	FString str = this->GetName();
	UE_LOG(LogTemp, Warning, TEXT("%s: Generated"), *str);
}

//�Q�[���J�n����1�x��������
void ADroneBullet::BeginPlay()
{
	Super::BeginPlay();
}

//���t���[������
void ADroneBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//	���݂�FPS�l���擾
	float FPS = 1.0f / DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("%fFPS"), FPS);
	//	�����������Ă���葬�x�Œe���ړ�����悤�ɕ␳
	float moveCorrection = 60.0f / FPS;
	FVector Pos = FMath::Lerp(Start, End, m_DeleteTimeCount/ m_DeleteTimeCountMax);

	//UE_LOG(LogTemp, Warning, TEXT("Strat%s"), *Start.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("End%s"), *End.ToString());

	SetActorLocation(Pos * moveCorrection, true);
	//AddActorWorldOffset(GetActorForwardVector() * m_BulletSpeed * moveCorrection);
	//SetActorRotation(FRotator(GetActorRotation().Roll,GetActorRotation().Yaw + 1.0f, GetActorRotation().Pitch));

	//	��莞�Ԃ�������폜����
	m_DeleteTimeCount += DeltaTime;
	if (m_DeleteTimeCount > m_DeleteTimeCountMax)
	{
		//�f�o�b�O�m�F�p
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
	///�f�o�b�O�m�F�p
		FString str = GetName();
		FString str2 = OtherActor->GetName();
		UE_LOG(LogTemp, Error, TEXT("%s: Hit a %s"), *str,*str2);

	//Destroy();
}

