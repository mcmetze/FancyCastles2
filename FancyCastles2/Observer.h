#pragma once

#include <memory>
#include <vector>

class Command;
struct TimerResult;

using TimerResultPtr = std::shared_ptr < TimerResult > ;

class Observer
{
public:
	virtual ~Observer() {}
	virtual void OnNotify(TimerResultPtr) = 0;
	virtual void OnNotify(Command*) = 0;
};

using ObserverPtr = std::shared_ptr < Observer > ;
using ObserverList = std::vector < std::weak_ptr<Observer> > ;

class Observable
{
public:
	void AddObserver(ObserverPtr obs);

private:
	void RemoveDeadObservers();

	ObserverList mObservers;

protected:
	void Notify(TimerResultPtr);
	void Notify(Command*);
};