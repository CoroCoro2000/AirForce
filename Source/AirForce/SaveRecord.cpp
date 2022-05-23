
#include "SaveRecord.h"

#define LOCTEXT_NAMESPACE "Record"

//�L�^���鎞�Ԃ̏��(10:00;00)
const float FRecordTime::RECORDTICKS_MAX = 600.f;
const FRecordTime FRecordTime::RECORDTIME_MAX = FRecordTime(RECORDTICKS_MAX);

FRecordTime::FRecordTime()
	: Minutes(0)
	, Second(0)
	, Millisecond(0)
	, Ticks(0)
{
	Initialize();
}

FRecordTime::FRecordTime(const FRecordTime& recordTime)
	: Minutes(recordTime.Minutes)
	, Second(recordTime.Second)
	, Millisecond(recordTime.Millisecond)
	, Ticks(recordTime.Ticks)
	, MinutesTextFormat(recordTime.MinutesTextFormat)
	, SecondTextFormat(recordTime.SecondTextFormat)
	, MillisecondTextFormat(recordTime.MillisecondTextFormat)
{

}

FRecordTime::FRecordTime(const float& ticks)
	: Ticks(ticks)
{
	Initialize();

	if (this->Ticks >= RECORDTICKS_MAX)
	{
		Ticks = RECORDTICKS_MAX;
		Minutes = Ticks / 60;
		Second = 0;
		Millisecond = 0;
	}
	else
	{
		//�o�ߎ��Ԃ�/�b/�~���b�ɕ���
		Minutes = Ticks / 60;
		Second = (int32)Ticks % 60;
		Millisecond = (Ticks - (int32)ticks) * (10 ^ MillisecondTextFormat.MaximumIntegralDigits);
	}
}

void FRecordTime::UpdateTime(const float& deltaTime)
{
	Ticks += deltaTime;

	if (this->Ticks >= RECORDTICKS_MAX)
	{
		Ticks = RECORDTICKS_MAX;
		Minutes = Ticks / 60;
		Second = 0;
		Millisecond = 0;
	}
	else
	{
		//�o�ߎ��Ԃ�/�b/�~���b�ɕ���
		Minutes = Ticks / 60;
		Second = (int32)Ticks % 60;
		Millisecond = (Ticks - (int32)Ticks) * 1000;
	}
}

FText FRecordTime::ToText()const
{
	//�����l�ߐݒ�
	FText minutesText = FText::AsNumber(Minutes, &MinutesTextFormat);
	FText secondText = FText::AsNumber(Second, &SecondTextFormat);
	FText millisecondText = FText::AsNumber(Millisecond, &MillisecondTextFormat);
	
	return FText::Format(LOCTEXT("Record", "{minutesText}:{secondText}.{millisecondText}"), minutesText, secondText, millisecondText);
}

#undef LOCTEXT_NAMESPACE

FString FRecordTime::ToString()const
{
	return 	ToText().ToString();
}

bool FRecordTime::operator>(const FRecordTime& Time)const
{
	return Ticks > Time.Ticks;
}

bool FRecordTime::operator>=(const FRecordTime& Time)const
{
	return Ticks >= Time.Ticks;
}

bool FRecordTime::operator<(const FRecordTime& Time)const
{
	return Ticks < Time.Ticks;
}

bool FRecordTime::operator<=(const FRecordTime& Time)const
{
	return Ticks <= Time.Ticks;
}

bool FRecordTime::operator==(const FRecordTime& Time)const
{
	return Ticks == Time.Ticks;
}

bool FRecordTime::operator!=(const FRecordTime& Time)const
{
	return Ticks != Time.Ticks;
}

void FRecordTime::Initialize()
{
	//�����A�J���}��؂�͕\�����Ȃ��ݒ�
	MinutesTextFormat.AlwaysSign = false;
	MinutesTextFormat.UseGrouping = false;
	SecondTextFormat.AlwaysSign = false;
	SecondTextFormat.UseGrouping = false;
	MillisecondTextFormat.AlwaysSign = false;
	MillisecondTextFormat.UseGrouping = false;

	//�\�����錅���̏���Ɖ�����ݒ�
	MinutesTextFormat.MinimumIntegralDigits = 2;
	MinutesTextFormat.MaximumIntegralDigits = 2;
	SecondTextFormat.MinimumIntegralDigits = 2;
	SecondTextFormat.MaximumIntegralDigits = 2;
	MillisecondTextFormat.MinimumIntegralDigits = 3;
	MillisecondTextFormat.MaximumIntegralDigits = 3;
}

//-------------------------------------------------------------------------------------------------


//�R���X�g���N�^
USaveRecord::USaveRecord(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}