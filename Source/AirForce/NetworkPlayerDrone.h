//------------------------------------------------------------------------
// ファイル名		:NetworkPlayerDrone.h
// 概要				:ネットワーク用のプレイヤードローンクラス
// 作成日			:2022/02/28
// 作成者			:19CU0105 池村凌太
// 更新内容			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DroneBase.h"
#include "PlayerDrone.h"
#include "NetworkPlayerDrone.generated.h"

//前方宣言
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class USpotLightComponent;
class FName;

UCLASS()
class AIRFORCE_API ANetworkPlayerDrone : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetworkPlayerDrone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//ドローンの当たり判定にオブジェクトがオーバーラップした時呼ばれるイベント関数を登録
	UFUNCTION()
		virtual void OnDroneCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//ドローンの当たり判定にオブジェクトがヒットした時呼ばれるイベント関数を登録
	UFUNCTION()
		void OnDroneCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	//レプリケートを登録
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//動いているか判定する(引数：入力軸)
	UFUNCTION(BlueprintCallable, Category = "PlayerDrone")
		bool IsMoving(const FVector _axisValue)const { return  (!_axisValue.IsZero() ? true : false); }
	//プレイヤーのID設定
	UFUNCTION(BlueprintCallable, Category = "Network")
		void SetPlayerId(const int32& PlayerId) { m_PlayerId = PlayerId; }
	//プレイヤーのID取得
	UFUNCTION(BlueprintCallable, Category = "Network")
		int32 GetPlayerId()const { return m_PlayerId; }
	//	入力フラグの設定
	UFUNCTION(BlueprintCallable, Category = "Control")
		void SetisControl(const bool _isControl) { m_isControl = _isControl; }
	//	ドローンの加速フラグ取得
	UFUNCTION(BlueprintCallable, Category = "Drone|Speed")
		bool GetIsOverAccle()const { return m_bIsPassedRing; }

private:
	//【入力バインド】各スティックの入力
	void Input_Throttle(float _axisValue);
	void Input_Elevator(float _axisValue);
	void Input_Aileron(float _axisValue);
	void Input_Ladder(float _axisValue);

	//入力量の取得
	UFUNCTION(BlueprintCallable, Category = "InputAxis")
		float GetInputValue(const TEnumAsByte<EINPUT_AXIS::Type> _Axis)const { return m_AxisValue[_Axis]; }

	//入力値を羽の加速度に変換する処理
	float RightInputValueToWingAcceleration(const int _arrayIndex);
	float LeftInputValueToWingAcceleration(const int _arrayIndex);
	//羽の加速度更新処理
	virtual void UpdateWingAccle(const float& DeltaTime);
	//羽の回転更新処理
	virtual void UpdateWingRotation(const float& DeltaTime);
	//入力の加速度更新処理
	virtual void UpdateAxisAcceleration(const float& DeltaTime);
	//回転処理
	void UpdateRotation(const float& DeltaTime);
	//移動処理
	void UpdateSpeed(const float& DeltaTime);
	//カメラ更新処理
	void UpdateCamera(const float& DeltaTime);
	//カメラとの遮蔽物のコリジョン判定
	void UpdateCameraCollsion(const float& DeltaTime);
	//風のエフェクトの更新処理
	void UpdateWindEffect(const float& DeltaTime);
	//進行軸と入力軸が逆向きか確認
	bool IsReverseInput(const float& _movingAxis, const float& _axisValue)const { return (_movingAxis < 0.f && 0.f < _axisValue) || (_movingAxis > 0.f && 0.f > _axisValue); }
	//高度の上限をを超えているか確認
	void UpdateAltitudeCheck();
	//砂埃のエフェクトの表示切替
	void UpdateCloudOfDustEffect();
	//メッシュアセットのセットアップ
	virtual void MeshAssetSetup();
	//メッシュの初期設定
	virtual void InitializeMesh();
	//ライトの初期設定
	virtual void InitializeLight();
	//エフェクトの初期設定
	void InitializeEmitter();
	//カメラの初期設定
	void InitializeCamera();

private:
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMesh* m_BodyMesh;													//機体のメッシュ
	UPROPERTY(EditAnywhere, Category = "Mesh|Body")
		UStaticMeshComponent* m_pBodyMesh;
	//羽
	UPROPERTY(EditAnywhere, EditFixedSize, Category = "Mesh|Wing")
		TArray<UStaticMesh*> m_WingMesh;													//羽のメッシュ
	TArray<TSharedPtr<FWing>>  m_pWings;												//羽を管理する構造体
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		USpringArmComponent* m_pSpringArm;									//スプリングアーム
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		UCameraComponent* m_pCamera;										//カメラ
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraTargetLength;											//プレイヤーとカメラの距離
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_FieldOfView;														//カメラの視野角
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffset;										//カメラの位置
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraSocketOffsetMax;									//カメラが追従する範囲の上限
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FVector m_CameraMoveLimit;											//カメラの移動できる上限
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		FRotator m_CameraRotationAttenRate;							//カメラが傾く速さ
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_MotionBlurAmount;											//モーションブラーの強度
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_MotionBlurMax;													//モーションブラーの最大歪み
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		int32 m_MotionBlurTargetFPS;											//モーションブラーのターゲットFPS
	UPROPERTY(VisibleAnywhere, Category = "Drone|Input")
		FVector4 m_AxisValue;												//各軸の入力値(0:AILERON、1:ELEVATOR、2:THROTTLE、3:LADDER)
	UPROPERTY(VisibleAnywhere, Category = "UI")
		FVector m_StartLocation;
	UPROPERTY(VisibleAnywhere, Category = "UI")
		FQuat m_StartQuaternion;
	UPROPERTY(EditAnywhere, Category = "PlayerCamera")
		float m_CameraRotationYaw;
	UPROPERTY(EditAnywhere, Category = "Network")
		int32 m_PlayerId;
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
	UPROPERTY(EditAnywhere, Category = "Drone")
		bool m_isControl;								//操作可能フラグ
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
