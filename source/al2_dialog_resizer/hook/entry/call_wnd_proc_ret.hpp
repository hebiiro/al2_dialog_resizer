#pragma once

namespace apn::dialog_resizer::hook
{
	//
	// このクラスはWH_CALLWNDPROCRETフックを担当します。
	//
	inline struct cwpr_t : entry_t
	{
		my::win_hook::unique_ptr<> win_hook;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			win_hook.reset(::SetWindowsHookEx(WH_CALLWNDPROCRET, win_hook_proc, nullptr, ::GetCurrentThreadId()));

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			win_hook.reset();

			return TRUE;
		}

		//
		// WH_CALLWNDPROCRETのフックプロシージャです。
		//
		static LRESULT CALLBACK win_hook_proc(int code, WPARAM w_param, LPARAM l_param)
		{
			auto cs = (CWPRETSTRUCT*)l_param;
#ifdef _DEBUG
			if (0)
			{
				// デバッグ用のコードです。

				auto hwnd = cs->hwnd;
				auto message = cs->message;
				auto w_param = cs->wParam;
				auto l_param = cs->lParam;
				auto l_result = cs->lResult;
				auto class_name = my::get_class_name(hwnd);

				MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}, {/} => {/hex}",
					hwnd, my::message_to_string(message), w_param, l_param, class_name, l_result);
			}
#endif
			switch (cs->message)
			{
			case WM_CREATE:
				{
					if (cs->lResult < 0) break;

					// 作成されたウィンドウのハンドルを取得します。
					auto hwnd = cs->hwnd;

					// ウィンドウを作成したインスタンスを取得します。
					auto instance = (HINSTANCE)::GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);

					// インスタンスがaviutl2ではない場合は除外します。
					if (instance != ::GetModuleHandleW(nullptr)) break;

					// 作成されたウィンドウのクラス名を取得します。
					auto class_name = my::get_class_name(hwnd);

					// ダイアログではない場合は除外します。
					if (class_name != L"#32770") break;

					// ダイアログのタイトルを取得します。
					auto window_name = my::get_window_text(hwnd);

					MY_TRACE_FUNC("{/hex}, {/}, {/}", hwnd, class_name, window_name);

					// ダイアログのタイトルを翻訳します。
					auto title = tr(L"Dialog", window_name);

					// ダイアログデザインを取得します。
					auto it = table::dialog_design.nodes.find(title);
					if (it == table::dialog_design.nodes.end()) break;

					// ここでダイアログをサブクラス化します。
					if (std::make_shared<dialog::resizable_dialog_t>(hwnd, it->second)->subclass(hwnd)) break;

					break;
				}
			}

			return ::CallNextHookEx(nullptr, code, w_param, l_param);
		}
	} cwpr;
}
