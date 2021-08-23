//----------------------------------------------------------------------------------------------
// �t�@�C����		:GameUtility.cpp
// �T�v				:�ǂ�����ł��Ăяo����֐��̏������܂Ƃ߂����[�e�B���e�B�N���X
// �쐬��			:2021/07/26
// �쐬��			:19CU0105 �r������
// �X�V���e			:
//----------------------------------------------------------------------------------------------

//�C���N���[�h
#include "GameUtility.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Containers/UnrealString.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"

//#define DEBUG_LOG

//������n�ʖ����؂�̂�
float CGameUtility::SetDecimalTruncation(float value, int n)
{
    value = value * FMath::Pow(10, n);
    value = floor(value);
    value /= FMath::Pow(10, n);
    return value;
}

//�^�O����A�N�^�[���擾����֐�(��1�����F���̊֐����Ăяo���A�N�^�[�A��2�����F��������A�N�^�[�̃^�O��)
AActor* CGameUtility::GetActorFromTag(AActor* _pOwnerActor, const FName& _tag)
{
    //NULL�`�F�b�N
    if (!_pOwnerActor) { return NULL; }

    //�����Ώۂ͑S�Ă�Actor
    TSubclassOf<AActor> findClass;
    findClass = AActor::StaticClass();
    TArray<AActor*> actors;
    UGameplayStatics::GetAllActorsOfClass(_pOwnerActor->GetWorld(), findClass, actors);

    //�������ʁAActor�������
    if (actors.Num() > 0)
    {
        //����Actor�̒������ԂɌ���
        for (AActor* pActor : actors)
        {
            //�^�O���Ŕ��ʂ���
            if (pActor->ActorHasTag(_tag))
            {
#ifdef DEBUG_LOG
                //�m�F�p���b�Z�[�W�o��
                FString message = FString("Founded Actor�F") + pActor->GetName();
                UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
#endif // DEBUG_LOG
                return pActor;
            }
        }
    }
    return NULL;
}