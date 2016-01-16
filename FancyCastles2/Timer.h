#pragma once

#include <ctime>

#include "GameObject.h"

struct TimerResult
{
	TimerResult()
		: mResultObjectType(GameObjectType::INVALID), mResultLocation(-1), mQuantity(-1), mPlayerID(-1) {}

	TimerResult(GameObjectType type, int location, int quantity, int playerID)
		: mResultObjectType(type), mResultLocation(location), mQuantity(quantity), mPlayerID(playerID) {}

	GameObjectType mResultObjectType;
	int mResultLocation;
	int mQuantity;
	int mPlayerID;  //?
};

class TimerObject : public GameObject
{
public:
	TimerObject(const TimerObject& other) = delete;
	TimerObject& operator=(const TimerObject& rhs) = delete;

	TimerObject(int timerID);

	void OnTimerStart(TimerResultPtr result);
	void OnTimerFinished();

	void Tick();
	void Cancel();

	bool IsBusy() const;

	int GetObjectID() const override;
	GameObjectType GetObjectType() const override;

private:
	struct TimerState
	{
		bool mIsBusy;
		time_t mStartTime;
		TimerResultPtr mResult;

		TimerState() : mIsBusy(false), mStartTime(std::clock()) { }
	};

	const double mTimeoutSec;
	TimerState mTimerState;

	int mTimerID;
};