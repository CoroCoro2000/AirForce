//------------------------------------------------------------------------
// ファイル名		:DroneBase.h
// 概要				:ドローンのベースクラス
// 作成日			:2021/04/19
// 作成者			:19CU0105 池村凌太
// 更新内容			:2021/04/19 プレイヤーとエネミーの共通項の追加
//------------------------------------------------------------------------

//インクルードガード
#pragma once

//インクルード
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "DroneBase.generated.h"

//前方宣言
class UStaticMeshComponent;
class USphereComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class USpotLightComponent;
class FName;

//羽の番号の列挙
UENUM(BlueprintType)
namespace EWING
{
	enum Type
	{
		LEFT_FORWARD						UMETA(DisplayName = "LEFT_FORWARD"),
		RIGHT_FORWARD					UMETA(DisplayName = "RIGHT_FORWARD"),
		LEFT_BACKWARD					UMETA(DisplayName = "LEFT_BACKWARD"),
		RIGHT_BACKWARD				UMETA(DisplayName = "RIGHT_BACKWARD"),
		NUM										UMETA(Hidden)
	};
}

//羽の情報を管理する構造体
struct FWing
{
public:
	//コンストラクタ
	FWing(const uint32 wingNum, UStaticMeshComponent* wingMesh)
		: WingNumber(wingNum)
		, pWingMesh(wingMesh)
		, AccelState(0.f)
	{}
	//デストラクタ
	~FWing() {}

public:
	uint32 GetWingNumber()const { return WingNumber; }							//羽番号取得
	UStaticMeshComponent* GetWingMesh()const { return pWingMesh; }		//羽のメッシュ取得

private:
	uint32 WingNumber;																				//識別番号(1:左前、2:右前、3:左後ろ、4:右後ろ)
	UStaticMeshComponent* pWingMesh;														//メッシュ

public:
	float AccelState;																						//加速度の段階(-1:最小の加速度、0:加速度なし、1:加速度あり、2:最大の加速度)
};

//defineマクロ
//現在のFPSを計測
#define GetFPS (1.f / DeltaTime)
//フレームレートが低下しても移動量に影響が無いよう補正する値
#define MOVE_CORRECTION (60.f / GetFPS)
#define SLOPE_MIN 0.f
#define SPEED_MIN -10.5f
#define SPEED_MAX 10.5f
#define VECTOR3_COMPONENT_NUM 3
#define VECTOR4_COMPONENT_NUM 4

UCLASS()
class AIRFORCE_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	//コンストラクタ
	ADroneBase();
	//デストラクタ
	virtual ~ADroneBase();

protected:
	//ゲーム開始時に1度だけ処理
	virtual void BeginPlay() override;

	//レプリケートを登録
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
public:
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;

	//【入力バインド】コントローラー入力設定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	入力フラグの設定
	void SetisControl(const bool _isControl) { m_isControl = _isControl; }

protected:
	//ドローンの当たり判定にオブジェクトがオーバーラップした時呼ばれるイベント関数を登録
	UFUNCTION()
		virtual void OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//ドローンの当たり判定にオブジェクトがヒットした時呼ばれるイベント関数を登録
	UFUNCTION()
		void OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	//操作可能フラグ取得
	UFUNCTION(BlueprintCallable, Category = "Drone")
		bool GetisControl() const { return  m_isControl; }

	//ボディメッシュの回転設定
	UFUNCTION(BlueprintCallable, Category = "Drone")
		void SetBodyMeshRotation(const FRotator& NewRotator);
	//ボディメッシュの回転設定
	void SetBodyMeshRotation(const FQuat& NewRotator);
	//ボディメッシュの回転量取得
	UFUNCTION(BlueprintCallable, Category = "Drone")
		FRotator GetBodyMeshRotation()const;
	//ボディメッシュの回転量取得
	UFUNCTION(BlueprintCallable, Category = "Drone")
		FRotator GetBodyMeshRelativeRotation()const;

	//	ドローンの時速(kilometers per hour)取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		float GetKPH(const float DeltaTime)const { return m_Velocity.Size() * (60.f / (1.f / DeltaTime)) / 100000.f / DeltaTime * 3600.f * 2.f; }
	
	//	ドローンの加速フラグ取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		bool GetIsOverAccle()const { return m_bIsPassedRing; }

	//ステージパス取得
	UFUNCTION(BlueprintCallable, Category = "SaveFilePath")
		void SetStagePath(FString _StagePath) { m_SaveStageFolderPath = _StagePath; }
	//ローカル軸を取得
	FVector GetLocalAxis()const { return m_LocalAxis; }

	//入力値の設定
	UFUNCTION(BlueprintCallable)
		void SetAxisValue(const FVector4& NewAxisValue) { m_AxisValuePerFrame = NewAxisValue; }
	//入力値の取得
	UFUNCTION(BlueprintCallable)
		FVector4 GetAxisValue()const { return m_AxisValuePerFrame; }

protected:
	//メッシュアセットのセットアップ
	virtual void MeshAssetSetup();
	//コリジョンの初期設定
	virtual void InitializeCollision();
	//メッシュの初期設定
	virtual void InitializeMesh();
	UFUNCTION(Client, Reliable)
		virtual void Client_InitializeMesh();
	//ライトの初期設定
	virtual void InitializeLight();
	UFUNCTION(Client, Reliable)
		virtual void Client_InitializeLight();

	//羽の加速度更新処理
	virtual void UpdateWingAccle(const float& DeltaTime);
	//回転処理
	virtual void UpdateRotation(const float& DeltaTime);
	//速度更新処理
	virtual void UpdateSpeed(const float& DeltaTime);
	//羽の回転更新処理
	virtual void UpdateWingRotation(const float& DeltaTime);
	//風のエフェクト更新処理
	virtual void UpdateWindEffect(const float& DeltaTime);

	//進行軸と入力軸が逆向きか確認
	bool IsReverseInput(const float& _movingAxis, const float& _axisValue)const { return (_movingAxis < 0.f && 0.f < _axisValue) || (_movingAxis > 0.f && 0.f > _axisValue); }
	//高度の上限をを超えているか確認
	void UpdateAltitudeCheck(const float& DeltaTime);

	//砂埃のエフェクトの表示切替
	void UpdateCloudOfDustEffect();

protected:
	//-------------------------------------------------------------------------------------------------------
	//BODY
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMesh* m_BodyMesh;													//機体のメッシュ
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;
	//ドローンのコリジョン
	UPROPERTY(EditAnywhere, Category = "Collision")
		USphereComponent* m_pDroneCollision;
	//羽
	UPROPERTY(EditAnywhere, EditFixedSize, Category = "Mesh|Wing")
		TArray<UStaticMesh*> m_WingMesh;													//羽のメッシュ
	TArray<TSharedPtr<FWing>>  m_pWings;												//羽を管理する構造体
	//1秒間の羽の最大回転数
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_RPSMax;
	//羽の加速度
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccele;
	//最小の加速度の倍率
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMin;
	//最大の加速度の倍率
	UPROPERTY(EditAnywhere, Category = "Wing")
		float m_WingAccelMax;

	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_TiltLimit;									//傾きの上限
	UPROPERTY(EditAnywhere, Category = "Physical")
		float  m_Speed;									//ドローンの秒速(m)

	UPROPERTY(VisibleAnywhere, Category = "Physical")
		float m_SpeedPerSecondMax;						//ドローンの最大秒速(m)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector4 m_AxisAccel;						//各軸の加速度
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Acceleration;							//加速度
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Deceleration;							//減速度
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Turning;								//逆入力した時の減速率
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_Attenuation;							//衝突時の速度減衰率
	UPROPERTY(EditAnywhere, Category = "Physical")
		float m_DroneWeight;							//ドローンの重量(kg)
	UPROPERTY(VisibleAnywhere, Category = "Physical")
		FVector m_Velocity;								//このドローンにかかっている力の量
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* m_pWingRotationSE;			//羽の回転SE

	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_HeightMax;															//ドローンが飛ぶことのできる地面からの高さの範囲
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_HeightFromGround;												//地面からの高さ
	UPROPERTY(EditAnywhere, Category = "Drone")
		float m_DistanceToSlope;													//斜面までの距離

	UPROPERTY(EditAnywhere, Category = "Drone", Replicated/*, ReplicatedUsing = OnRep_m_isControl*/)
		bool m_isControl;								//操作可能フラグ

	UPROPERTY(EditAnywhere, Category = "Drone")
		FVector4 m_AxisValuePerFrame;													//毎フレーム更新される入力の値

	FVector m_LocalAxis;																//ドローンのローカル軸
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraSystem* m_pWindEffect;										//風のエフェクト
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraComponent* m_pWindEmitter;									//風のエフェクト
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindRotationSpeed;											//風のエフェクトの回転速度
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindOpacity;
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_WindNoise;

	TArray<TArray<FString>> m_SaveVelocityText;			//読み込んだ毎フレームの移動量を格納する配列
	TArray<TArray<FString>> m_SaveQuatText;				//読み込んだ毎フレームの回転量を格納する配列
	UPROPERTY(EditAnywhere, Category = "Drone")
		int m_PlaybackFlame;
	UPROPERTY(EditAnywhere, Category = "Drone")
		int m_PlayableFramesNum;

	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		FString m_SaveRecordFolderPath;									//レコードをたどるパスを設定
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		FString m_SaveStageFolderPath;									//ステージをたどるパスを設定
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveVelocityLoadPath;							//移動量が書きこまれたファイルをたどるパスを設定
	UPROPERTY(EditAnywhere, Category = "SaveFilePath")
		TArray<FString> m_SaveQuatLoadPath;								//回転量が書きこまれたファイルをたどるパスを設定

	UPROPERTY(VisibleAnywhere, Category = "Ring")
		bool m_bIsPassedRing;															//リングをくぐったかどうか
	UPROPERTY(VisibleAnywhere, Category = "Ring")
		float m_SincePassageCount;													//リングをくぐってからの経過時間
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_CountLimitTime;															//リングをくぐってから数える時間の上限	
	UPROPERTY(EditAnywhere, Category = "Ring")
		float m_OverAccelerator;														//リングをくぐったときの加速倍率

	UPROPERTY(EditAnywhere, Category = "Light")
		USpotLightComponent* m_pLeftSpotLight;
	UPROPERTY(EditAnywhere, Category = "Light")
		USpotLightComponent* m_pRightSpotLight;
	UPROPERTY(EditAnywhere, Category = "Effect")
		TMap<FString, UNiagaraSystem*> m_pDroneEffects;				//ドローンのエフェクトを格納する配列
	UPROPERTY(EditAnywhere, Category = "Effect")
		UNiagaraComponent* m_pCloudOfDustEmitter;								//砂煙のエフェクト
	UPROPERTY(EditAnywhere, Category = "Effect")
		float m_ShowEffectDistance;															//エフェクトを表示する距離
	UPROPERTY(EditAnywhere, Category = "Effect")
		FString m_GroundMaterialName;														//レイがヒットした地面のマテリアル名
};
