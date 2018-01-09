#pragma once
#include <Windows.h>
#include <WinBase.h>
#include <CommCtrl.h>
#include <Pdh.h>
#include <assert.h>
#include <dxgi.h>
#include <d3d9.h>
#include <stdio.h>
#include <locale.h>
#include <intrin.h>

PDH_FMT_COUNTERVALUE counterVal;
PDH_HQUERY cpuQuery;
PDH_HCOUNTER cpuTotal;
VOID Init() {
	PdhOpenQuery(NULL, NULL, &cpuQuery);
	PdhAddCounter(cpuQuery, L"\\Processor(_TOTAL)\\% Processor Time", NULL, &cpuTotal);
	PdhCollectQueryData(cpuQuery);
}

VOID GetCPUUsage() {
	PdhCollectQueryData(cpuQuery);
	PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	Computer.CenterProcessUnit.CPUUsage = counterVal.doubleValue;
}

VOID GetCPUClock()
{
	INT CPUInfo[4] = { 0 };
	__cpuid(CPUInfo, 0);
	if (CPUInfo[0] >= 0x16) {
		__cpuid(CPUInfo, 0x16);
		Computer.CenterProcessUnit.CPUFrequency = CPUInfo[0];
		Computer.CenterProcessUnit.CPUMaxFrequency = CPUInfo[1];
		Computer.CenterProcessUnit.BusFrequency = CPUInfo[2];
	}
	else {
		Computer.CenterProcessUnit.CPUFrequency = 0;
		Computer.CenterProcessUnit.CPUMaxFrequency = 0;
		Computer.CenterProcessUnit.BusFrequency = 0;
	}
}