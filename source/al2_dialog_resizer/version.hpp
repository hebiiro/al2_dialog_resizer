#pragma once

namespace apn::dialog_resizer
{
	//
	// このクラスはこのプログラムのバージョンを管理します。
	//
	inline struct version_t : version_base_t {
		//
		// コンストラクタです。
		//
		version_t() : version_base_t(L"ダイアログサイズ変更MOD", L"🐍ダイアログサイズ変更MOD🔖", L"r1") {}
	} version;
}
