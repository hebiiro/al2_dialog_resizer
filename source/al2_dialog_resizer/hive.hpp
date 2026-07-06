#pragma once

namespace apn::dialog_resizer
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t : hive_base_t
	{
		//
		// aviutl2のログハンドルです。
		//
		LOG_HANDLE* logger = {};
	} hive;
}
