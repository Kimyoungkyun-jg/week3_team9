#pragma once

#include "FUObjectArray.h"
#include <concepts>

// TODO: 참조를 확실하게 관리하려면 TObjectPtr<TObject>를 반환하도록 바꿔야 함
template <typename TObject, typename ... TArgs>
	requires std::derived_from<TObject, UObject>
TObject* NewObject(TArgs&&... Args)
{
	TObject* Object = new TObject(std::forward<TArgs>(Args)...);
	try {
		FUObjectArray::Get().AddObject(Object);
	}
	catch (...) {
		delete Object;
		throw;
	}
	return Object;
}
