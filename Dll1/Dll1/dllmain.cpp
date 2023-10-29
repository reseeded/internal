// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <thread>
#include <string>
#include "includes.h"
#include <iostream>
#include <cmath>

namespace offset {
	constexpr int dwEntityList = 0x16AA850;
	constexpr int localPlayer = 0x187CFC8;
	constexpr int m_fflags = 0x100; //maybe wrong 0x104
	constexpr int forceJump = 0x1695130;
	constexpr int localOrigin = 0xC; //origin vector
	constexpr int m_ViewOrigin = 0xD4; // Vector
	constexpr int m_vecOrigin = 0x80; //CNetworkOriginCellCoordQuantizedVector
}
	
namespace EntityList
{
	constexpr int m_iHealth = 0x32C; 
	constexpr int x = 0xCD8;
	constexpr int y = 0xCDC;
	constexpr int z = 0xCE0;
}

void BunnyHop(const HMODULE instance) noexcept
{
	const auto client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));
	//loop
	while (!GetAsyncKeyState(VK_END)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!GetAsyncKeyState(VK_SPACE))
			continue;
		
		//get local player, instead of memory read we cast to pointer 
		const auto localPlayer = *reinterpret_cast<std::uintptr_t*>(client + offset::localPlayer);

		if (!localPlayer)
			continue;
		
		const auto health = *reinterpret_cast<std::int32_t*>(localPlayer + EntityList::m_iHealth);

		//is alive

		if (!health)
			continue;

		const auto flags = *reinterpret_cast<std::int32_t*>(localPlayer + offset::m_fflags);

		// on ground
		
		(flags & (1 << 0)) ?
			*reinterpret_cast<std::uintptr_t*>(client + offset::forceJump) = 6 : //force jump
			*reinterpret_cast<std::uintptr_t*>(client + offset::forceJump) = 4; //reset

		
	}
	if (GetAsyncKeyState(VK_F9)) {
		FreeLibraryAndExitThread(instance, 0);
		
	}
	//uninject
		
}
 


class Vec3
{
public:
	float pi = 3.141592;
	float x, y, z;
	float a, b, c; //position
	//float magnitude = sqrt(a ^ 2 + b ^ 2 + c ^ 2); //distance & hypotenuse
	int origin = 0;
	
	
	static float magnitude(float a, float b, float c)
	{
		return sqrtf(a * a + b * b + c * c);
	}
	
	void calculateAngles(double& pitch, double& yaw) const {
		float mag = magnitude(a, b, c);
		if (mag > 0) {
			pitch = std::acos(c / mag);
			yaw = std::atan2(b, a);  // atan2 handles a == 0 case
		}
		else {
			// Handle zero magnitude case, if necessary
			pitch = 0;
			yaw = 0;
		}
	}

	//double pitch = acos(c / magnitude);
	//double yaw = atan(b / a); // think sin = opp/hyp, cos = adj/hyp, tan = sin/cos

	double radToDeg(double angleInRadians) {
		return angleInRadians * (180.0 / pi);
	}
};



class ViewAngles
{
public:

	//vector subtraction
	int local[3] = { 1,1,1 }; int enemy[3] = { -1, -2, -1 }; int enemyVector[3]; //placeholder coordinates
	ViewAngles() {
		for (int i = 0; i < 3; ++i) {
			enemyVector[i] = local[i] - enemy[i];
		}
	}


	
};

void aim(const HMODULE instance)
{
	const auto client = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));

	const auto localOrigin = *reinterpret_cast<std::uintptr_t*>(client + offset::localOrigin);

	const auto m_ViewOrigin = *reinterpret_cast<std::uintptr_t*>(client + offset::m_ViewOrigin);

	const auto dwEntityList = *reinterpret_cast<std::uintptr_t*>(client + offset::dwEntityList);

	int enemy = dwEntityList + 0x10;

	float x = dwEntityList + 0xCD8; float y = dwEntityList + 0xCDC; float z = dwEntityList + 0xCE0; //local player coordinates
	float a = enemy + 0xCD8; float b = enemy + 0xCDC; float c = enemy + 0xCE0; //non-local player coordinates

	float enemyMagnitude = Vec3::magnitude(a, b, c);

	float vectorTo[3] = { a - x, b - y, c - z };

	
	float localEyePos = localOrigin + m_ViewOrigin; //local player eye position = origin vector + view offset

	Vec3 vec;
	vec.a = a;
	vec.b = b;
	vec.c = c;

	double pitch, yaw;
	vec.calculateAngles(pitch, yaw);
	

	//vector to enemy head = targets head position - local player eye pos 
	//You can get the coordinates of enemies by reading their entity's data:
	//3) Read a vector3 at PointerToEntity + m_vecOrigin
}



BOOL APIENTRY DllMain( HMODULE instance, const std::uintptr_t reason, const void* reserved)
{
	//dll attach
	if (reason == 1)
	{
		DisableThreadLibraryCalls(instance);

		const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(BunnyHop), instance, 0, nullptr);
		if (thread)
			CloseHandle(thread);
	}
	return TRUE;

	
}
