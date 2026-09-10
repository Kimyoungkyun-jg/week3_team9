#pragma once

#include "Runtime/Core/TArray.h"
#include "Runtime/Core/TFunction.h"

class UObject;
class FReferenceCollector;

class FGarbageCollector final {
public:
	static FGarbageCollector& Get() {
		static FGarbageCollector Instance;
		return Instance;
	}

	void AddRoot(UObject* Object);
	void RemoveRoot(UObject* Object);

	void CollectGarbage(const TFunction<void(const FReferenceCollector&)>& BeforeSweep = {}); 
	// TODO: BeforeSweep 콜백함수는 유연성도 매우 떨어지고 일일이 제어권이 없는 약한포인터들이 있을때마다 콜백함수에 포인터 = nullptr을 추가해줘야 하므로 향후 Weak포인터 시스템으로 알아서 BeforeSweep에 감지되게...
	// EX: FEditor의 SelectedObject를 SceneManager가 해제해 줄 책임은 없다. 왜냐하면 SceneManager는 에디터 전용 클래스가 아니기 때문. 그래서 SceneManager가 에디터 밖의 경로에서 Scene을 교체하는 경우 FEditor는 곤란해진다-> Weak.
	FGarbageCollector(const FGarbageCollector&) = delete;
	FGarbageCollector& operator=(const FGarbageCollector&) = delete;

	FGarbageCollector(FGarbageCollector&&) = delete;
	FGarbageCollector& operator=(FGarbageCollector&&) = delete;

private:
	FGarbageCollector() = default;
	~FGarbageCollector() = default;

	TArray<UObject*> RootObjects;
};