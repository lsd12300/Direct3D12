#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime()const; // 单位秒
	float DeltaTime()const; // 单位秒

	void Reset(); // 消息循环前 调用
	void Start(); // 取消暂停时 调用
	void Stop();  // 暂停时 调用
	void Tick();  // 每帧调用

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