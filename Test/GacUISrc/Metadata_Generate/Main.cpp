#include "../../../Source/Reflection/TypeDescriptors/GuiReflectionPlugin.h"
#ifdef VCZH_MSVC
#include <Windows.h>
#endif

using namespace vl;
using namespace vl::filesystem;
using namespace vl::stream;
using namespace vl::reflection;
using namespace vl::reflection::description;

namespace vl
{
	namespace presentation
	{
		void GuiInitializeUtilities() {}
		void GuiFinalizeUtilities() {}
	}
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			extern bool LoadGuiBasicTypes();
			extern bool LoadGuiElementTypes();
			extern bool LoadGuiCompositionTypes();
			extern bool LoadGuiEventTypes();
			extern bool LoadGuiTemplateTypes();
			extern bool LoadGuiControlTypes();
		}
	}
}

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
	vint pos = -1;
	vint index = 0;
	while (buffer[index])
	{
		if (buffer[index] == L'\\' || buffer[index] == L'/')
		{
			pos = index;
		}
		index++;
	}
	return WString::CopyFrom(buffer, pos + 1);
}

WString GetTestOutputPath()
{
#ifdef _WIN64
	return GetExePath() + L"../../../Resources/Metadata/";
#else
	return GetExePath() + L"../../Resources/Metadata/";
#endif
}
#elif defined VCZH_GCC
WString GetTestOutputPath()
{
	return L"../../Resources/Metadata/";
}
#endif

#ifdef VCZH_64
#define REFLECTION_BIN L"Reflection64.bin"
#define REFLECTION_OUTPUT L"Reflection64.txt"
#define REFLECTION_BASELINE L"Reflection64.txt"
#else
#define REFLECTION_BIN L"Reflection32.bin"
#define REFLECTION_OUTPUT L"Reflection32.txt"
#define REFLECTION_BASELINE L"Reflection32.txt"
#endif

void GuiMain()
{
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	LoadPredefinedTypes();
	LoadParsing2Types();
	XmlAstLoadTypes();
	JsonAstLoadTypes();
	WfLoadLibraryTypes();
	LoadGuiBasicTypes();
	LoadGuiElementTypes();
	LoadGuiCompositionTypes();
	LoadGuiEventTypes();
	LoadGuiTemplateTypes();
	LoadGuiControlTypes();
	GetGlobalTypeManager()->Load();
	{
		FileStream fileStream(GetTestOutputPath() + REFLECTION_BIN, FileStream::WriteOnly);
		GenerateMetaonlyTypes(fileStream);
	}
	{
		FileStream fileStream(GetTestOutputPath() + REFLECTION_OUTPUT, FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf8);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);
		LogTypeManager(writer);
	}
	return 0;
}