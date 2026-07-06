#pragma once

namespace apn::dialog_resizer::table
{
	//
	// このクラスはダイアログデザインのテーブルです。
	//
	inline struct dialog_design_t : entry_t
	{
		//
		// このクラスはダイアログデザインのノードです。
		//
		struct node_t {
			//
			// ダイアログの初期サイズです。
			//
			SIZE default_size = { -1, -1 };

			//
			// ダイアログの中心座標です。
			//
			POINT center_pos = { -1, -1 };
		};

		//
		// ノードのコレクションです。
		//
		std::unordered_map<std::wstring, node_t> nodes;

		//
		// テーブルファイルのファイル名を返します。
		//
		virtual std::filesystem::path on_get_file_name() override
		{
			return L"dialog_design.tsv";
		}

		//
		// テーブルをリロードします。
		//
		virtual BOOL on_reload(const reader_t& reader) override
		{
			// すべてのノードを消去します。
			nodes.clear();

			// テーブルファイルを読み込みます。
			reader([&](int index, const std::vector<std::wstring>& vec)
			{
				// 先頭行は除外します。
				if (index == 0) return FALSE;

				// 要素数が無効の場合は除外します。
				if (vec.empty()) return FALSE;

				// ダイアログ名が無効の場合は除外します。
				if (vec[0].empty()) return FALSE;

				// コメント行は除外します。
				if (vec[0].starts_with(L"//")) return FALSE;

				// コレクションに新しいノードを追加します。
				auto& node = nodes[vec[0]] = {};

				//
				// この関数は配列に整数値が存在する場合は変数にセットします。
				//
				const auto set_int = [&](size_t i, auto& output) {
					if (vec.size() > i && vec[i].length()) output = wcstol(vec[i].c_str(), nullptr, 0);
				};

				set_int(1, node.default_size.cx);
				set_int(2, node.default_size.cy);
				set_int(3, node.center_pos.x);
				set_int(4, node.center_pos.y);

				return TRUE;
			});

			return TRUE;
		}
	} dialog_design;
}
