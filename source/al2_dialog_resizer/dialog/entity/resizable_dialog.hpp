#pragma once

namespace apn::dialog_resizer::dialog
{
	//
	// このクラスはaviutl2の一部のダイアログをサブクラス化してリサイズ可能にします。
	//
	struct resizable_dialog_t : entity_t
	{
		//
		// このクラスは座標です。
		//
		struct pos_t
		{
			//
			// 座標のアラインです。
			//
			inline static constexpr struct align_t {
				inline static constexpr size_t k_near = 0;
				inline static constexpr size_t k_far = 1;
			} k_align;

			//
			// アラインです。
			//
			size_t align;

			//
			// アラインからの相対座標です。
			//
			int relative_pos;

			//
			// コンストラクタです。
			//
			pos_t(int default_pos, int near_pos, int far_pos, int center_pos)
			{
				// 左上に近い場合は
				if (default_pos < center_pos)
				{
					// 左上を原点にします。
					align = k_align.k_near;
					relative_pos = default_pos - near_pos;
				}
				// 右下に近い場合は
				else
				{
					// 右下を原点にします。
					align = k_align.k_far;
					relative_pos = default_pos - far_pos;
				}
			}

			//
			// X座標を算出して返します。
			//
			int calc_x(const RECT& rc)
			{
				return (align == k_align.k_near ? rc.left : rc.right) + relative_pos;
			}

			//
			// Y座標を算出して返します。
			//
			int calc_y(const RECT& rc)
			{
				return (align == k_align.k_near ? rc.top : rc.bottom) + relative_pos;
			}
		};

		//
		// このクラスはコントロールです。
		//
		struct control_t
		{
			//
			// コントロールのウィンドウハンドルです。
			//
			HWND hwnd;

			//
			// コントロールの座標です。
			//
			pos_t left, top, right, bottom;
#ifdef _DEBUG
			std::wstring class_name;
			std::wstring window_name;
#endif
			//
			// コンストラクタです。
			//
			control_t(HWND hwnd, const RECT& rc, const RECT& client_rc, const POINT& client_center)
				: hwnd(hwnd)
				, left(rc.left, client_rc.left, client_rc.right, client_center.x)
				, top(rc.top, client_rc.top, client_rc.bottom, client_center.y)
				, right(rc.right, client_rc.left, client_rc.right, client_center.x)
				, bottom(rc.bottom, client_rc.top, client_rc.bottom, client_center.y)
#ifdef _DEBUG
				, class_name(my::get_class_name(hwnd))
				, window_name(my::get_window_text(hwnd))
#endif
			{
			}

			//
			// コントロールを再配置します。
			//
			BOOL arrange(my::DeferWindowPos& dwp, const RECT& client_rc)
			{
				auto rc = RECT {
					left.calc_x(client_rc),
					top.calc_y(client_rc),
					right.calc_x(client_rc),
					bottom.calc_y(client_rc),
				};

				return dwp.set_window_pos(hwnd, nullptr, &rc, SWP_NOZORDER | SWP_NOCOPYBITS);
			}

			//
			// ウィンドウハンドルを返します。
			//
			operator HWND() const { return hwnd; }
		};

		//
		// コントロールのコレクションです。
		//
		std::vector<control_t> controls;

		//
		// ダイアログデザインです。
		//
		const table::dialog_design_t::node_t& design;

		//
		// コンストラクタです。
		//
		resizable_dialog_t(HWND hwnd, const table::dialog_design_t::node_t& design)
			: design(design)
		{
			// ダイアログをリサイズ可能に変更します。
			// WM_INITDIALOGのタイミングでは遅いので、ここで実行します。
			my::modify_style(hwnd, 0, WS_THICKFRAME | WS_MAXIMIZEBOX);
		}

		//
		// コントロール配列を初期化します。
		//
		BOOL init_controls(HWND hwnd)
		{
			MY_TRACE_FUNC("{/hex}", hwnd);

			// 既存のコントロールを取得します。
			{
				//
				// このクラスはパラメータです。
				//
				struct param_t {
					//
					// thisポインタです。
					//
					resizable_dialog_t* p;

					//
					// クライアント矩形です。
					//
					RECT client_rc;

					//
					// クライアント矩形の中心座標です。
					//
					POINT client_center;

					//
					// コンストラクタです。
					//
					param_t(resizable_dialog_t* p) {
						this->p = p;
						this->client_rc = my::get_client_rect(*p);
						this->client_center.x = (p->design.center_pos.x >= 0 ) ?
							p->design.center_pos.x : my::get_center_x(this->client_rc);
						this->client_center.y = (p->design.center_pos.y >= 0 ) ?
							p->design.center_pos.y : my::get_center_y(this->client_rc);
					}
				} param = { this };

				// 既存のコントロール(子ウィンドウ)を列挙します。
				::EnumChildWindows(hwnd,
					[](HWND control_hwnd, LPARAM l_param) -> BOOL
				{
					MY_TRACE_HWND(control_hwnd);

					// パラメータを取得します。
					auto param = (param_t*)l_param;

					// 直接の子ウィンドウではない場合は除外します。
					if (::GetParent(control_hwnd) != *param->p) return TRUE;

					// 既存のコントロールの矩形を取得します。
					auto control_rc = my::get_window_rect(control_hwnd);
					my::map_window_points(nullptr, *param->p, &control_rc);

					// 既存のコントロールをコレクションに追加します。
					auto& control = param->p->controls.emplace_back(
						control_hwnd, control_rc, param->client_rc, param->client_center);

					// 列挙を続けます。
					return TRUE;
				},
				(LPARAM)&param);
			}

			// ダイアログの初期サイズを変更します。
			{
				auto [ w, h ] = design.default_size;
				auto rc = my::get_window_rect(*this);
				if (w <= 0) w = my::get_width(rc);
				if (h <= 0) h = my::get_height(rc);
				auto x = my::get_center_x(rc) - w / 2;
				auto y = my::get_center_y(rc) - h / 2;
				::SetWindowPos(*this, nullptr, x, y, w, h,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
				::SendMessage(*this, DM_REPOSITION, 0, 0);
			}

			return TRUE;
		}

		//
		// コントロールを再配置します。
		//
		BOOL arrange_controls(HWND hwnd)
		{
			MY_TRACE_FUNC("{/hex}", hwnd);

			// クライアント矩形を取得します。
			auto client_rc = my::get_client_rect(hwnd);

			// コントロールを一度に全部再配置できるようにします。
			my::DeferWindowPos dwp((int)controls.size());

			// コントロールを走査します。
			for (auto& control : controls)
			{
				// コントロールを再配置します。
				control.arrange(dwp, client_rc);
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", hwnd, my::message_to_string(message), w_param, l_param);

			switch (message)
			{
			case WM_INITDIALOG:
				{
					// スコープ終了時(デフォルト処理の後)に実行します。
					my::scope_exit scope_exit([&]()
					{
						// ダイアログをロックします。
						my::locker_t locker(this);

						// コントロールを初期化します。
						init_controls(hwnd);
					});

					return __super::on_wnd_proc(hwnd, message, w_param, l_param);
				}
			case WM_SIZE:
				{
					// コントロールを再配置します。
					arrange_controls(hwnd);

					break;
				}
			case WM_LBUTTONDBLCLK:
				{
					// ウィンドウ名を取得します。
					auto window_name = my::get_window_text(hwnd);

					// クリック座標を取得します。
					auto pt = my::lp_to_pt(l_param);

					// ログを出力します。
					logger.log_format(L"「{/}」内のクリック座標 = ({/}, {/})", window_name, pt.x, pt.y);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	};
}
