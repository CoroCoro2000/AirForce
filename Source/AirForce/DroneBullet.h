//------------------------------------------------------------------------
// ファイル名		:DroneBullet.h
// 概要				:ドローンの弾のクラス
// 作成日			:2021/05/17
// 作成者			:19CU0104 池田翔一郎
// 更新内容			:
//------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DroneBullet.generated.h"

UCLASS()
class AIRFORCE_API ADroneBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	//コンストラクタ
	ADroneBullet();

protected:
	//ゲーム開始時に1度だけ処理
	virtual void BeginPlay() override;

	//	オーバーラップ接触し始めた時に呼ばれるイベントイベント関数を登録
	UFUNCTION(BlueprintCallable, Category = "Overlap")
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	//毎フレーム処理
	virtual void Tick(float DeltaTime) override;

	//	弾の当たり判定の取得
	UBoxComponent* GetBulletBoxComp() const { return m_pBulletBoxComp; };

	//	ドローンの速度を加算
	void AddDroneSpeed(const float speed) { m_BulletSpeed += speed;}
private:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pBulletMesh;		//弾のメッシュ
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UBoxComponent* m_pBulletBoxComp;			//弾の当たり判定

	UPROPERTY(EditAnywhere, Category = "Speed")
		float m_BulletSpeed;							//	弾のスピード

	UPROPERTY(VisibleAnywhere, Category = "Time")
		float m_DeleteTimeCount;							//	弾の削除時間

	UPROPERTY(EditAnywhere, Category = "Time")
		float m_DeleteTimeCountMax;					//	弾の削除時間

public:
	UPROPERTY(VisibleAnywhere)
		FVector Start;										//弾の発射開始座標
	UPROPERTY(VisibleAnywhere)
		FVector End;											//弾の着弾座標
};