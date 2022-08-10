#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime()const; // ��λ��
	float DeltaTime()const; // ��λ��

	void Reset(); // ��Ϣѭ��ǰ ����
	void Start(); // ȡ����ͣʱ ����
	void Stop();  // ��ͣʱ ����
	void Tick();  // ÿ֡����

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

#endif // GAMETIMER_H