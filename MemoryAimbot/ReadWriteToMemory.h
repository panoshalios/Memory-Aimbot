#pragma once
#include <Windows.h>

//Reads From Memory
float FindCurrentValue(int numOfPointers, HANDLE hProcHandle, const DWORD *offsets, DWORD baseAddress)
{
	DWORD temp_Value;
	DWORD final_addr;
	float valueToReturn;
	if (numOfPointers > 0)
	{
		// < only to find the address but <= to find the value
		for (int index = 0; index <= numOfPointers; index++)
		{
			if (index == 0)
			{
				ReadProcessMemory(hProcHandle, (LPCVOID)baseAddress, &temp_Value, sizeof(temp_Value), NULL);
			}
			final_addr = temp_Value + offsets[index];
			ReadProcessMemory(hProcHandle, (LPCVOID)final_addr, &temp_Value, sizeof(temp_Value), NULL);
		}
		ReadProcessMemory(hProcHandle, (LPCVOID)final_addr, &valueToReturn, sizeof(float), NULL);
	}
	return valueToReturn;
}

//Writes To Memory
void WriteToMemory()
{

}