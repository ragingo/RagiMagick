﻿#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include "ragii/image/Bitmap.h"
#include "ragii/image/BitmapConverter.h"
#include "ragii/text/text.h"


using namespace std;
using namespace ragii::image;

struct CommandOption
{
	string_view name;
	string_view value;
};

int convert(vector<CommandOption>& opts);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		return EXIT_FAILURE;
	}

	vector<CommandOption> opts;

	for (int i = 1; i < argc; )
	{
		auto opt = string_view(argv[i]);
		if (opt[0] == '-')
		{
			opts.emplace_back(CommandOption { argv[i], argv[i + 1] });
			i += 2;
		}
		else
		{
			opts.emplace_back(CommandOption { argv[i], "" });
			i++;
		}
	}

	for (auto i : opts)
	{
		cout << i.name.data() << ": " << i.value.data() << endl;
	}

	if (opts.empty())
	{
		cout << "コマンドを指定してください。 convert, etc." << endl;
		return EXIT_FAILURE;
	}

	auto command = opts[0].name;
	opts.erase(opts.begin());

	if (command == "convert")
	{
		return convert(opts);
	}

	return EXIT_SUCCESS;
}


int convert(vector<CommandOption>& opts)
{
	if (opts.empty())
	{
		cout << "変換方法を指定してください。 negative, grayscale, etc." << endl;
		return EXIT_FAILURE;
	}

	auto in_file = find_if(opts.begin(), opts.end(), [&](auto i) { return i.name == "-i"; });
	if (in_file == opts.end())
	{
		cout << "入力ファイル名を指定してください。" << endl;
		return EXIT_FAILURE;
	}

	auto out_file = find_if(opts.begin(), opts.end(), [&](auto i) { return i.name == "-o"; });
	if (out_file == opts.end())
	{
		cout << "出力ファイル名を指定してください。" << endl;
		return EXIT_FAILURE;
	}

	if (!ragii::text::ends_with(in_file->value.data(), ".bmp"))
	{
		cout << ".bmp 以外は非対応です。" << endl;
		return EXIT_FAILURE;
	}

	auto bmp = Bitmap::loadFromFile(in_file->value.data());

	if (!bmp)
	{
		cout << "ファイルのロードに失敗しました。" << endl;
		return EXIT_FAILURE;
	}

	cout << "width: " << bmp->getWidth() << ", height: " << bmp->getHeight() << ", depth: " << bmp->getBitCount() / 8 << endl;

	auto filter = opts[0].name;

	if (filter == "negative")
	{
		BitmapConverter::applyFilter(bmp.get(), FilterType::Negative);
	}
	else if (filter == "binary")
	{
		BitmapConverter::applyFilter(bmp.get(), FilterType::Binary);
	}
	else if (filter == "grayscale")
	{
		BitmapConverter::applyFilter(bmp.get(), FilterType::Grayscale);
	}
	else if (filter == "laplacian")
	{
		BitmapConverter::applyFilter(bmp.get(), FilterType::Grayscale);
		BitmapConverter::applyFilter(bmp.get(), FilterType::Laplacian);
	}

	bmp->save(out_file->value.data());
	cout << "converted." << endl;

	return EXIT_SUCCESS;
}
