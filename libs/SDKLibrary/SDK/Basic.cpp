#pragma once

// Dumped with Dumper-7!
#include "../pch.h"
#include "../../utils/memory.h"
#include <fstream>

namespace SDK
{

void(*FName::AppendString)(const FName*, FString&) = nullptr;

void InitGObjects()
{
	// UObject::GObjects = reinterpret_cast<TUObjectArray*>(uintptr_t(GetModuleHandle(0)) + Offsets::GObjects);
	auto gObjects = signature("48 8B 05 ? ? ? ? 48 8B 0C C8 4C 8D 04 D1 EB 03").instruction(3).add(7);
	auto appendString = signature("48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F2");
	UObject::GObjects = reinterpret_cast<TUObjectArray*>(gObjects.GetPointer());
	FName::AppendString = reinterpret_cast<decltype(FName::AppendString)>(appendString.GetPointer());
	/*std::ofstream out("C:/Users/An/Downloads/offsets.txt");
	out << "real gobjects: " << std::hex << (uintptr_t(GetModuleHandle(0)) + Offsets::GObjects) << std::endl;
	out << "gobjects: " << std::hex << gobjects.GetPointer() << std::endl;
	out << "real appendString: " << std::hex << (uintptr_t(GetModuleHandle(0)) + Offsets::AppendString) << std::endl;
	out << "appendString: " << std::hex << addr_appendString << std::endl;*/
}

FString FSoftObjectPtr::GetSubPathString()
{
	return ObjectID.SubPathString;
}

std::string FSoftObjectPtr::GetSubPathStringStr()
{
	return ObjectID.SubPathString.ToString();
}

template<class SoftObjectPath>
SoftObjectPath& FSoftObjectPtr::GetObjectPath()
{
	static_assert(std::is_same_v<SoftObjectPath, FSoftObjectPath>, "Only use this with FSoftObjectPath. This function is only templated as a workaround to C++ type-checks.");

	return reinterpret_cast<FSoftObjectPath&>(ObjectID);
}

void Dummy() { FSoftObjectPtr().GetObjectPath(); }


class UObject* FWeakObjectPtr::Get() const
{
	return UObject::GObjects->GetByIndex(ObjectIndex);
}

class UObject* FWeakObjectPtr::operator->() const
{
	return UObject::GObjects->GetByIndex(ObjectIndex);
}

bool FWeakObjectPtr::operator==(const FWeakObjectPtr& Other) const
{
	return ObjectIndex == Other.ObjectIndex;
}
bool FWeakObjectPtr::operator!=(const FWeakObjectPtr& Other) const
{
	return ObjectIndex != Other.ObjectIndex;
}

bool FWeakObjectPtr::operator==(const class UObject* Other) const
{
	return ObjectIndex == Other->Index;
}
bool FWeakObjectPtr::operator!=(const class UObject* Other) const
{
	return ObjectIndex != Other->Index;
}
}