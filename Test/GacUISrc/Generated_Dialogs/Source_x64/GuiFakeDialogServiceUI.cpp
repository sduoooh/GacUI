﻿/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

GacGen.exe Resource.xml

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "GuiFakeDialogServiceUIIncludes.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, GuiFakeDialogServiceUIReflection.h) */
#include "GuiFakeDialogServiceUIReflection.h"
#endif
/* CodePack:EndIgnore() */

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::GuiFakeDialogServiceUI::
#define GLOBAL_NAME ::vl_workflow_global::GuiFakeDialogServiceUI::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::GuiFakeDialogServiceUI::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_GuiFakeDialogServiceUI)
	vl_workflow_global::GuiFakeDialogServiceUI instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_GuiFakeDialogServiceUI)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	GuiFakeDialogServiceUI& GuiFakeDialogServiceUI::Instance()
	{
		return Getvl_workflow_global_GuiFakeDialogServiceUI().instance;
	}
}

/***********************************************************************
Class (::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindowConstructor)
***********************************************************************/

namespace vl
{
	namespace presentation
	{
		namespace controls
		{
			namespace fake_dialog_service
			{
				void GuiMessageBoxWindowConstructor::__vwsn_vl_presentation_controls_fake_dialog_service_GuiMessageBoxWindow_Initialize(::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow* __vwsn_this_)
				{
					(this->self = __vwsn_this_);
					{
						::vl::__vwsn::This(this->self)->SetClientSize([&](){ ::vl::presentation::Size __vwsn_temp__; __vwsn_temp__.x = static_cast<::vl::vint64_t>(240L); __vwsn_temp__.y = static_cast<::vl::vint64_t>(120L); return __vwsn_temp__; }());
					}
					{
						::vl::__vwsn::This(this->self)->SetText(::vl::WString::Unmanaged(L"MessageBox"));
					}
				}

				GuiMessageBoxWindowConstructor::GuiMessageBoxWindowConstructor()
					: self(static_cast<::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow*>(nullptr))
				{
				}

/***********************************************************************
Class (::vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow)
***********************************************************************/

				GuiMessageBoxWindow::GuiMessageBoxWindow()
					: ::vl::presentation::controls::GuiWindow(::vl::presentation::theme::ThemeName::Window)
				{
					auto __vwsn_resource_ = ::vl::__vwsn::This(::vl::presentation::GetResourceManager())->GetResourceFromClassName(::vl::WString::Unmanaged(L"vl::presentation::controls::fake_dialog_service::GuiMessageBoxWindow"));
					auto __vwsn_resolver_ = ::vl::Ptr<::vl::presentation::GuiResourcePathResolver>(new ::vl::presentation::GuiResourcePathResolver(__vwsn_resource_, ::vl::__vwsn::This(__vwsn_resource_.Obj())->GetWorkingDirectory()));
					::vl::__vwsn::This(this)->SetResourceResolver(__vwsn_resolver_);
					::vl::__vwsn::This(this)->__vwsn_vl_presentation_controls_fake_dialog_service_GuiMessageBoxWindow_Initialize(this);
				}

				GuiMessageBoxWindow::~GuiMessageBoxWindow()
				{
					this->FinalizeInstanceRecursively(static_cast<::vl::presentation::controls::GuiControlHost*>(this));
				}

			}
		}
	}
}
#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
