#include "pch.h"
#include "resource.h"
#include "version.hpp"
#include "app_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "debug.hpp"
#include "table/reader.hpp"
#include "table/entry.hpp"
#include "table/manager.hpp"
#include "table/entry/dialog_design.hpp"
#include "dialog/manager.hpp"
#include "dialog/entity/resizable_dialog.hpp"
#include "hook/entry.hpp"
#include "hook/manager.hpp"
#include "hook/entry/call_wnd_proc_ret.hpp"
#include "config_dialog.hpp"
#include "config_io.hpp"
#include "app.hpp"

namespace apn::dialog_resizer
{
	//
	// aviutl2から呼ばれます。
	//
	EXTERN_C void InitializeLogger(LOG_HANDLE* logger)
	{
		hive.logger = logger;
	}
}
