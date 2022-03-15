
#include "SaveRecord.h"

const int32 FSaveTopRankingTime::RankingMax = 5;

//�w�菇�ʂ̃��R�[�h���擾
float FSaveTopRankingTime::GetRankRecord(const int& rank)const
{
	//�w�菇�ʂ̋L�^������ꍇ�͒l��Ԃ�
	if (RecordTimes.IsValidIndex(rank - 1))
	{
		return RecordTimes[rank - 1];
	}

	//�����̏��ʂ̋L�^���Ȃ��ꍇ�̓��O�o��
	UE_LOG(LogTemp, Error, TEXT("FSaveTop5Time::GetRankRecord = No record of this rank [%d]."), rank);

	return -1.f;
}

//�R���X�g���N�^
USaveRecord::USaveRecord(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//1�ʂ�Transform��ݒ�
void USaveRecord::SetBestTimeTransform(const FName CourseName, const TArray<FVector>& RecordLocations, const TArray<FQuat>& RecordRotations)
{
	//���Ƀf�[�^������Ώ㏑��
	if (m_BestTimeTransform.Contains(CourseName))
	{
		m_BestTimeTransform[CourseName].BestRecordLocation = RecordLocations;
		m_BestTimeTransform[CourseName].BestRecordRotation = RecordRotations;
	}
	//�f�[�^���Ȃ���ΐV�������
	else
	{
		m_BestTimeTransform.Add(CourseName, FSaveBestTimeTransform(RecordLocations, RecordRotations));
	}
}

//�����L���O���肵�����R�[�h��ݒ�
void USaveRecord::SetRecordTime(const FName CourseName, const float& RecordTime)
{
	//�����L���O�f�[�^������ꍇ�͋L�^��ǉ�
	if (m_TopRankingTimes.Contains(CourseName))
	{
		m_TopRankingTimes[CourseName].RecordTimes.Add(RecordTime);
		//�^�C�����������ɕ��ёւ���
		m_TopRankingTimes[CourseName].RecordTimes.Sort();
		//�����L���O�O�̃^�C�����폜
		int32 RankingNum = m_TopRankingTimes[CourseName].RecordTimes.Num();
		if (RankingNum > FSaveTopRankingTime::RankingMax)
		{
			for (int32 Lastindex = RankingNum - 1; Lastindex >= FSaveTopRankingTime::RankingMax; --Lastindex)
			{
				m_TopRankingTimes[CourseName].RecordTimes.RemoveAt(Lastindex);
			}
		}
	}
	//�����L���O���Ȃ������ꍇ�͐V�������
	else
	{
		m_TopRankingTimes.Add(CourseName);
		if (m_TopRankingTimes.Contains(CourseName))
		{
			m_TopRankingTimes[CourseName].RecordTimes.Add(RecordTime);
		}
	}
}

//1�ʂ�Transform���擾
FSaveBestTimeTransform USaveRecord::GetBestTimeTransform(const FName CourseName)const
{
	//1�ʂ̋L�^������Ύ��o��
	if (m_BestTimeTransform.Contains(CourseName))
	{
		return m_BestTimeTransform[CourseName];
	}

	//�f�[�^���Ȃ��ꍇ�̓��O���o��
	UE_LOG(LogTemp, Error, TEXT("USaveRecord::GetBestTimeTransform = No records for [%s]."), *(CourseName.ToString()));
	return FSaveBestTimeTransform();
}

//���5�ʂ̃����L���O�擾
FSaveTopRankingTime USaveRecord::GetTopRankingTime(const FName CourseName)const
{
	//�����L���O�f�[�^��������������o��
	if (m_TopRankingTimes.Contains(CourseName))
	{
		return m_TopRankingTimes[CourseName];
	}

	//�f�[�^���Ȃ��ꍇ�̓��O���o��
	UE_LOG(LogTemp, Error, TEXT("USaveRecord::GetTop5Time = No ranking data for [%s]."), *(CourseName.ToString()));
	return FSaveTopRankingTime();
}