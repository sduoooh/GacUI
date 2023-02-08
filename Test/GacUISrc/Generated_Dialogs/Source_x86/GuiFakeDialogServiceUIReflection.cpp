﻿/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

GacGen.exe Resource.xml

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "GuiFakeDialogServiceUIReflection.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

/***********************************************************************
Reflection
***********************************************************************/

namespace vl
{
	namespace reflection
	{
		namespace description
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			IMPL_CPP_TYPE_INFO(vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow)
			IMPL_CPP_TYPE_INFO(vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
#define _ ,
			BEGIN_CLASS_MEMBER(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow)
				CLASS_MEMBER_BASE(::vl::presentation::controls::GuiWindow)
				CLASS_MEMBER_BASE(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor)
				CLASS_MEMBER_CONSTRUCTOR(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow*(), NO_PARAMETER)
			END_CLASS_MEMBER(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow)

			BEGIN_CLASS_MEMBER(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor)
				CLASS_MEMBER_BASE(::vl::reflection::DescriptableObject)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor>(), NO_PARAMETER)
				CLASS_MEMBER_METHOD(__vwsn_vl_presentation_controls_fake_dialog_service_GuiMessageBoxWindow_Initialize, { L"__vwsn_this_" })
				CLASS_MEMBER_FIELD(self)
			END_CLASS_MEMBER(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor)

#undef _
			class GuiFakeDialogServiceUITypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow)
					ADD_TYPE_INFO(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif
#endif

			bool LoadGuiFakeDialogServiceUITypes()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(Ptr(new GuiFakeDialogServiceUITypeLoader));
				}
#endif
				return false;
			}
		}
	}
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
