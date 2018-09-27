#include <math.h>
#include <algorithm>
#include "OpenProcess.h"
#include "ReadWriteToMemory.h"
using namespace std;

int NumOfPlayers = 32;
//Change accordingly
const DWORD dw_PlayerCount = 0x5A3890B4;//ENGINE
const DWORD dw_PlayerBase = 0x28B4B6A8;//client
const DWORD dw_TeamOffset[] = { 0x98 };
const DWORD dw_Health[] = { 0x90 };
const DWORD dw_PosX[] = { 0x274 };
const DWORD dw_PosY[] = { 0x278 };
const DWORD dw_PosZ[] = { 0x27c };
const DWORD EntityPlayerBase = 0x28B4B6A8;
const DWORD EntityLoopDistance = 0x10;
const DWORD dw_angRotation = 0x6DFD4A1C;//ENGINE


struct MyPlayer_t
{
	DWORD CLocalPlayer;
	int Team;
	int Health;
	float Position[3];

	void ReadInformation()
	{
		CLocalPlayer = dw_PlayerBase;
		Team = (int)FindCurrentValue(1, hProc, dw_TeamOffset, CLocalPlayer);
		Health = (int)FindCurrentValue(1, hProc, dw_Health, CLocalPlayer);
		Position[0] = FindCurrentValue(1, hProc, dw_PosX, CLocalPlayer);
		Position[1] = FindCurrentValue(1, hProc, dw_PosY, CLocalPlayer);
		Position[2] = FindCurrentValue(1, hProc, dw_PosZ, CLocalPlayer);
		ReadProcessMemory(hProc, (LPCVOID)dw_PlayerCount, &NumOfPlayers, sizeof(int), NULL);
	}
}MyPlayer;

struct TargetList_t
{
	float Distance;
	float AimbotAngle[3];

	TargetList_t()
	{

	}

	TargetList_t(float aimbotAngle[], float myCoords[], float enemyCoords[])
	{
		Distance = Get3dDistance(myCoords[0], myCoords[1], myCoords[2],
			enemyCoords[0], enemyCoords[1], enemyCoords[2]);

		AimbotAngle[0] = aimbotAngle[0];
		AimbotAngle[1] = aimbotAngle[1];
		AimbotAngle[2] = aimbotAngle[2];
	}

	float Get3dDistance(float myCoordsX, float myCoordsZ, float myCoordsY,
						float enX, float enZ, float enY)
	{
		return sqrt(pow((double)enX - myCoordsX, 2) + 
					pow((double)enY - myCoordsY, 2) + 
					pow((double)enZ - myCoordsZ, 2));
	}
};

struct PlayerList_t
{
	DWORD CbaseEntity;
	int Team;
	int Health;
	float Position[3];
	float AimbotAngle[3];
	
	void ReadInformation(int player)
	{
		CbaseEntity = EntityPlayerBase + (player * EntityLoopDistance);
		ReadProcessMemory(hProc, (LPCVOID)(CbaseEntity + dw_TeamOffset), &Team, sizeof(int), NULL);
		ReadProcessMemory(hProc, (LPCVOID)(CbaseEntity + dw_Health), &Health, sizeof(int), NULL);
		ReadProcessMemory(hProc, (LPCVOID)(CbaseEntity + dw_PosX), &Position, sizeof(float[3]), NULL);
	}
}PlayerList[32];

struct CompareTargetEnArray
{
	bool operator () (TargetList_t & lhs, TargetList_t & rhs)
	{
		return lhs.Distance < rhs.Distance;
	}
};

//World to Screen
void CalcAngle(float *src, float *dst, float *angles)
{
	double opposite[3] = { (src[0] - dst[0]), (src[1] - dst[1]), (src[2] - dst[2]) };
	double hyp = sqrt(opposite[0] * opposite[0] + opposite[1] * opposite[1]);
	//57.295779513082 to convert to degrees or 180 / PI
	angles[0] = (float)(asinf(opposite[2] / hyp) * 57.295779513082);
	angles[1] = (float)(atanf(opposite[1] / opposite[0]) * 57.295779513082);
	angles[2] = 0;

	if (opposite[0] >= 0.0)
	{
		angles[1] += 180.0f;
	}
}

void Aimbot()
{
	TargetList_t *TargetList = new TargetList_t[NumOfPlayers];

	int targetLoop = 0;
	for (int i = 0; i < NumOfPlayers; i++)
	{
		PlayerList[i].ReadInformation(i);
		if (PlayerList[i].Team == MyPlayer.Team)
		{
			continue;
		}
		if (PlayerList[i].Health < 2)
		{
			continue;
		}
		CalcAngle(MyPlayer.Position, PlayerList[i].Position, PlayerList[i].AimbotAngle);
		TargetList[targetLoop] = TargetList_t(PlayerList[i].AimbotAngle, MyPlayer.Position, PlayerList[i].Position);
		targetLoop++;
	}

	if (targetLoop > 0)
	{
		sort(TargetList, TargetList + targetLoop, CompareTargetEnArray());

		if (!GetAsyncKeyState(0x2))
		{
			WriteProcessMemory(hProc, (LPVOID)dw_angRotation, TargetList[0].AimbotAngle, 12, NULL);
		}
	}
	targetLoop = 0;
	delete[] TargetList;
}

void main()
{
	SearchForGame();
	cout << "Running Aimbot!! Right Mouse to aim" << endl;

	while (!GetAsyncKeyState(VK_F6))
	{
		MyPlayer.ReadInformation();
		Aimbot();
	}
}