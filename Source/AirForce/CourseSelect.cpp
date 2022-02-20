// Fill out your copyright notice in the Description page of Project Settings.


#include "CourseSelect.h"
#include "Components/InputComponent.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetTextLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/FileHelper.h"
#include "GameUtility.h"

#if WITH_EDITOR
#include "Kismet/KismetSystemLibrary.h"
#endif // WITH_EDITOR


// Sets default values
ACourseSelect::ACourseSelect()
	: m_pDummyComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Dummy")))
	, m_CourseNumber(0)
	, m_CourseTotal(0)
	, m_bInputEnable(false)
	, m_bCourseSelectCompleted(false)
	, m_CurrentRotation(0.f)
	, m_TargetRotation(0.f)
	, m_pLevelDataTable(NULL)
	, m_pMinimapDefaultMesh(NULL)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	if (m_pDummyComponent)
	{
		RootComponent = m_pDummyComponent;
		m_pDummyComponent->SetMobility(EComponentMobility::Movable);
		m_pDummyComponent->bHiddenInGame = true;
	}
}

// Called when the game starts or when spawned
void ACourseSelect::BeginPlay()
{
	Super::BeginPlay();
	

	//データテーブルの検索
	if (m_pLevelDataTable)
	{
		TArray<FName> OutRowName;
		UDataTableFunctionLibrary::GetDataTableRowNames(m_pLevelDataTable, OutRowName);

		if (OutRowName.Num() > 0)
		{
			//0番目はタイトルなので除外
			OutRowName.RemoveAt(0);
			//コースの総数を保持
			m_CourseTotal = OutRowName.Num();
		}
	}
	//メッシュ初期化
	InitializeMesh();

	//
	InitializeCourseBestTimeText();
}

// Called every frame
void ACourseSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//メッシュの回転更新
	UpdateMeshRotation(DeltaTime);

	//コース全体の回転更新
	UpdateLocation(DeltaTime);
}

//メッシュ初期化
void ACourseSelect::InitializeMesh()
{
	if (m_CourseTotal <= 0) { return; }
	if (!m_pDummyComponent) { return; }

	//メッシュの初期設定
	float Radius = 1000.f;
	FVector Center = GetActorLocation();
	FVector Dir = GetActorForwardVector() * Radius;
	FVector Start = Center + Dir;
	FRotator Rotation = FRotator(0.f, 360.f / m_CourseTotal, 0.f);
	FTransform NewTransform(FRotator(0.f, 180.f, 45.f), Start, FVector(3.f, 6.f, 1.f));

	//選択コースの数だけメッシュを生成
	for (int i = 0; i < m_CourseTotal; ++i)
	{
		UStaticMeshComponent* pMesh = NewObject<UStaticMeshComponent>(this);
		if (pMesh && m_pMinimapDefaultMesh && m_pMinimapMaterials.IsValidIndex(i))
		{
			pMesh->RegisterComponent();
			pMesh->SetStaticMesh(m_pMinimapDefaultMesh);
			pMesh->SetMaterial(0, m_pMinimapMaterials[i]);
			pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			pMesh->SetMobility(EComponentMobility::Movable);
			pMesh->AttachToComponent(m_pDummyComponent, FAttachmentTransformRules::KeepRelativeTransform);
			pMesh->SetWorldTransform(NewTransform);
			m_pMinimapMeshes.Add(pMesh);

			//生成軸を回転
			Dir = Rotation.RotateVector(Dir);
			NewTransform.SetLocation(Center + Dir);
		}
	}
}

//メッシュの回転
void ACourseSelect::UpdateMeshRotation(const float& DeltaTime)
{
	for (int i = 0; i < m_CourseTotal; ++i)
	{
		if (m_pMinimapMeshes.IsValidIndex(i))
		{
			m_pMinimapMeshes[i]->AddWorldRotation(FRotator(0.f, 1.f, 0.f) * CGameUtility::MoveCorrection(DeltaTime));
		}
	}
}

//メッシュの移動
void ACourseSelect::UpdateLocation(const float& DeltaTime)
{
	if (m_CourseTotal <= 0) { return; }

	float difference = FMath::Abs(FMath::Abs(m_CurrentRotation) - FMath::Abs(m_TargetRotation));
	if (difference > 0.05f)
	{
		m_CurrentRotation = FMath::Lerp(m_CurrentRotation, m_TargetRotation, FMath::Clamp(DeltaTime * m_TargetRotationSpeed, 0.f, 1.f));
		SetActorRotation(FRotator(0.f, m_CurrentRotation, 0.f));
	}
	else
	{
		float Rotation = GetActorRotation().Yaw;
		m_CurrentRotation = Rotation;
		m_TargetRotation = Rotation;
	}
}

//選択中のコース名取得
FName ACourseSelect::GetSelectCourseName()const
{
	if (!m_pLevelDataTable) { return FName("None"); }
	TArray<FName> OutRowName;
	UDataTableFunctionLibrary::GetDataTableRowNames(m_pLevelDataTable, OutRowName);
	if (OutRowName.Num() > 0)
	{
		OutRowName.RemoveAt(0);
	}
	else
	{
		return FName("None");
	}

	return OutRowName[m_CourseNumber];
}


//選択中のコース名取得
FString ACourseSelect::GetSelectCourseBestTime(int _CourseNumber)const
{
	return m_pCourseBestTimeText[_CourseNumber] != TEXT("") ? m_pCourseBestTimeText[_CourseNumber]:TEXT("--------------");
}

//コース別ベストタイムの初期化
void ACourseSelect::InitializeCourseBestTimeText()
{
	TArray<FName> OutRowName;
	UDataTableFunctionLibrary::GetDataTableRowNames(m_pLevelDataTable, OutRowName);
	OutRowName.RemoveAt(0);

	TArray<FString> BestTimeText;
	//ステージの数だけ繰り返す
	for (int i = 0; i < OutRowName.Num(); i++)
	{
		//ファイル読み込み
		bool b = FFileHelper::LoadFileToStringArray(BestTimeText, *(FPaths::ProjectDir() + "Record/" + OutRowName[i].ToString() + "/Record.txt"));

#if WITH_EDITOR
		FString path = TEXT("Path::") + (FPaths::ProjectDir() + "Record/" + OutRowName[i].ToString() + "/Record.txt");
		FString str = b ? (FPaths::ProjectDir() + "Record/" + OutRowName[i].ToString() + "/Record.txt") : (TEXT("No best times found for ") + OutRowName[i].ToString());
		UKismetSystemLibrary::PrintString(this, str, true, false);
#endif // WITH_EDITOR

		//1位のタイムだけ取り出す
		if (BestTimeText.IsValidIndex(0))
		{
			m_pCourseBestTimeText.Add(BestTimeText[0]);
		}
		else
		{
			m_pCourseBestTimeText.Add("");
		}

	}
}

// Called to bind functionality to input
void ACourseSelect::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//軸マッピング
	InputComponent->BindAction(TEXT("InputRight"), EInputEvent::IE_Pressed, this, &ACourseSelect::Input_Right);
	InputComponent->BindAction(TEXT("InputLeft"), EInputEvent::IE_Pressed, this, &ACourseSelect::Input_Left);
	InputComponent->BindAction(TEXT("InputA"), EInputEvent::IE_Pressed, this, &ACourseSelect::Input_Decide);
	InputComponent->BindAction(TEXT("InputB"), EInputEvent::IE_Pressed, this, &ACourseSelect::Input_Cansel);
}

void ACourseSelect::Input_Right()
{
	if (m_bInputEnable && !m_bCourseSelectCompleted)
	{
		m_bInputEnable = false;
		m_CourseNumber = (m_CourseNumber + m_CourseTotal - 1) % m_CourseTotal;
		m_TargetRotation += 360.f / (float)m_CourseTotal;

		//入力があったら選択コースの3Dミニマップを正面に向ける
		m_pMinimapMeshes[m_CourseNumber]->SetWorldRotation(FRotator(0.f, 0.f, 45.f));
		m_bInputEnable = true;
	}
}
 
void ACourseSelect::Input_Left()
{
	if (m_bInputEnable && !m_bCourseSelectCompleted)
	{
		m_bInputEnable = false;
		m_CourseNumber = (m_CourseNumber + 1) % m_CourseTotal;
		m_TargetRotation -= 360.f / (float)m_CourseTotal;

		//入力があったら選択コースの3Dミニマップを正面に向ける
		m_pMinimapMeshes[m_CourseNumber]->SetWorldRotation(FRotator(0.f, 0.f, 45.f));
		m_bInputEnable = true;
	}
}

void ACourseSelect::Input_Decide()
{
	if (m_bInputEnable)
	{
		//コース選択を終了する
		m_bCourseSelectCompleted = true;
	}
}

void ACourseSelect::Input_Cansel()
{
	if (m_bInputEnable)
	{

	}
}