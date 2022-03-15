
#include "SaveRecord.h"

const int32 FSaveTopRankingTime::RankingMax = 5;

//指定順位のレコードを取得
float FSaveTopRankingTime::GetRankRecord(const int& rank)const
{
	//指定順位の記録がある場合は値を返す
	if (RecordTimes.IsValidIndex(rank - 1))
	{
		return RecordTimes[rank - 1];
	}

	//引数の順位の記録がない場合はログ出力
	UE_LOG(LogTemp, Error, TEXT("FSaveTop5Time::GetRankRecord = No record of this rank [%d]."), rank);

	return -1.f;
}

//コンストラクタ
USaveRecord::USaveRecord(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//1位のTransformを設定
void USaveRecord::SetBestTimeTransform(const FName CourseName, const TArray<FVector>& RecordLocations, const TArray<FQuat>& RecordRotations)
{
	//既にデータがあれば上書き
	if (m_BestTimeTransform.Contains(CourseName))
	{
		m_BestTimeTransform[CourseName].BestRecordLocation = RecordLocations;
		m_BestTimeTransform[CourseName].BestRecordRotation = RecordRotations;
	}
	//データがなければ新しく作る
	else
	{
		m_BestTimeTransform.Add(CourseName, FSaveBestTimeTransform(RecordLocations, RecordRotations));
	}
}

//ランキング入りしたレコードを設定
void USaveRecord::SetRecordTime(const FName CourseName, const float& RecordTime)
{
	//ランキングデータがある場合は記録を追加
	if (m_TopRankingTimes.Contains(CourseName))
	{
		m_TopRankingTimes[CourseName].RecordTimes.Add(RecordTime);
		//タイムが速い順に並び替える
		m_TopRankingTimes[CourseName].RecordTimes.Sort();
		//ランキング外のタイムを削除
		int32 RankingNum = m_TopRankingTimes[CourseName].RecordTimes.Num();
		if (RankingNum > FSaveTopRankingTime::RankingMax)
		{
			for (int32 Lastindex = RankingNum - 1; Lastindex >= FSaveTopRankingTime::RankingMax; --Lastindex)
			{
				m_TopRankingTimes[CourseName].RecordTimes.RemoveAt(Lastindex);
			}
		}
	}
	//ランキングがなかった場合は新しく作る
	else
	{
		m_TopRankingTimes.Add(CourseName);
		if (m_TopRankingTimes.Contains(CourseName))
		{
			m_TopRankingTimes[CourseName].RecordTimes.Add(RecordTime);
		}
	}
}

//1位のTransformを取得
FSaveBestTimeTransform USaveRecord::GetBestTimeTransform(const FName CourseName)const
{
	//1位の記録があれば取り出す
	if (m_BestTimeTransform.Contains(CourseName))
	{
		return m_BestTimeTransform[CourseName];
	}

	//データがない場合はログを出す
	UE_LOG(LogTemp, Error, TEXT("USaveRecord::GetBestTimeTransform = No records for [%s]."), *(CourseName.ToString()));
	return FSaveBestTimeTransform();
}

//上位5位のランキング取得
FSaveTopRankingTime USaveRecord::GetTopRankingTime(const FName CourseName)const
{
	//ランキングデータが見つかったら取り出す
	if (m_TopRankingTimes.Contains(CourseName))
	{
		return m_TopRankingTimes[CourseName];
	}

	//データがない場合はログを出す
	UE_LOG(LogTemp, Error, TEXT("USaveRecord::GetTop5Time = No ranking data for [%s]."), *(CourseName.ToString()));
	return FSaveTopRankingTime();
}