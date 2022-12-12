#include "../../../Source/GacUI.h"
#if defined VCZH_MSVC
#include <Windows.h>
#endif

using namespace vl;

#if defined VCZH_MSVC
using namespace vl::filesystem;
#endif

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
	vint pos = -1;
	vint index = 0;
	while (buffer[index])
	{
		if (buffer[index] == L'\\')
		{
			pos = index;
		}
		index++;
	}
	return WString::CopyFrom(buffer, pos + 1);
}
#endif

WString GetTestResourcePath()
{
#if defined VCZH_MSVC
#ifdef _WIN64
	return GetExePath() + L"..\\..\\..\\Resources\\CompilerErrorTests";
#else
	return GetExePath() + L"..\\..\\Resources\\CompilerErrorTests";
#endif
#elif defined VCZH_GCC
	return L"../../Resources/CompilerErrorTests";
#endif
}

WString GetTestBaselinePath()
{
#if defined VCZH_MSVC
#ifdef _WIN64
	return GetExePath() + L"..\\..\\..\\Resources\\CompilerErrorTests\\Baseline_x64";
#else
	return GetExePath() + L"..\\..\\Resources\\CompilerErrorTests\\Baseline_x86";
#endif
#elif defined VCZH_GCC
	return L"../../Resources/CompilerErrorTests/Baseline_x64";
#endif
}

WString GetTestOutputPath()
{
#if defined VCZH_MSVC
#ifdef _WIN64
	return GetExePath() + L"..\\..\\..\\Output\\x64";
#else
	return GetExePath() + L"..\\..\\Output\\x86";
#endif
#elif defined VCZH_GCC
	return L"../../Output/";
#endif
}

int UT_result = 0;
int UT_argc = 0;
#if defined VCZH_MSVC
wchar_t** UT_argv = nullptr;
#elif defined VCZH_GCC
char** UT_argv = nullptr;
#endif

#if defined VCZH_MSVC
TEST_FILE
{
	Folder folder(GetTestOutputPath());
	if (!folder.Exists())
	{
		TEST_CASE_ASSERT(folder.Create(false) == true);
	}
}
#endif

void GuiMain()
{
	UT_result = unittest::UnitTest::RunAndDisposeTests(UT_argc, UT_argv);
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
{
	UT_argc = argc;
	UT_argv = argv;
	SetupWindowsDirect2DRenderer();
#if defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return UT_result;
}
#elif defined VCZH_GCC
extern int SetupGacGenNativeController();
int main(int argc, char* argv[])
{
	UT_argc = argc;
	UT_argv = argv;
	SetupGacGenNativeController();
	return UT_result;
}
#endif