#include "EasyOpt.hpp"

#include <iostream>

int main(int argc, const char** argv)
{
	ArgumentParser opt(argc,argv);
	//概要说明
	opt.overview = "sbi:Make the Soil brightness index product.";
	//语法,不做说明则将自动生成
	//opt.syntax = "cli [-h] [-e|-f]  [-t modis|fy3a] -i Arg1,[ArgN] -o Arg";
	//实例
	opt.example = "./sbi.exe -i input1.h5,input2.h5,input3.h5 output.hdf";
	opt.footer = "version:0.9,LIC:See MIT-LICENSE.";
	////设置命令行选项  -h,--usage,--help 命令将自动设置，无需再设置
	opt.add(
		"-s,--short",       // 选项的前导符，可以有多个，使用逗号隔开，
							//    如果前导符中没有"-"或者 "--"，则认为是无标识的选项
							//    即不需要前导符，直接传入
		false,                  // 是否为必须?是则为1
		1,                  // 指定参数个数，-1用于传入列表,指定-1时最少需要1个参数,界面时建议不用-1.
		"Batch One Time", // 帮助说明
		ez::EZ_INT16,           //指定数据类型，指定后将自动判断类型的最大最小值,此后的参数为非必需参数
		"10",               // 默认值,可以设置为空.
		"1",                //指定最小值，若不需要则为""
		"",                 //指定最大值，若不需要则为""
		"1,10,15,20"        //设定允许值，默认为空，都允许
	);
	//无参数选项,命令行中，可以使用"-mc"同时设置两个无参数的选项,默认为unknown类型,用于设置true/false
	opt.add("-m", false, 0, "Merge Files or Not");
	//添加uint
	opt.add("--int",false,1,"Uint ID",EZ_UINT32);
	//添加float参数，要求输入两个
	opt.add("--float", false, 2, u8"宽度,高度", EZ_FLOAT, "33.5,44.2");
	//添加double参数,限制输入范围,添加默认参数
	opt.add("-d,--double", false, 4, "Longitude ranges", ez::EZ_DOUBLE, "1,2,3,4", "-180.0", "180.0");
	//添加text
	opt.add("-t,--type", false, 1, "Input Image Type", ez::EZ_TEXT, "fy3a", "", "", "modis,fy3a,zy,landsat");

	//无前导符的参数,输入文件,必须存在,将会放在所有参数后面
	opt.add("input", true, 1, "Input File.", ez::EZ_FILE);
	//输出文件
	opt.add("-o,--output", true, 1, "Output file argument.", ez::EZ_OUTPUT);

	//输出文件夹测试
	opt.add("-D,--dir", false, 1, "test dir arguments", ez::EZ_DIR);
	//互斥参数设置：使用逗号隔开一串第一个flag设置，互斥参数必须为可选，否则将出现逻辑问题
	//opt.xorAdd("-b,-f");

	//带参数传入,则使用CLI解析
	if (argc > 1 && !opt.parse())
	{
		return -1;
	}
	else
	{
	//不带参数传入,则弹出GUI解析
		opt.run();
		if (!opt.parse())
			return -1;
	}
	//get args
	short batchtime;
	opt.get("-s")->get(batchtime);

	bool bMerge = opt.get("-m")->isSet;

	unsigned int nID;
	opt.get("--int")->get(nID);

	std::vector< float > vWH;
	opt.get("--float")->getVector(vWH);

	std::vector<double> vLong;
	opt.get("--double")->getVector(vLong);

	std::string type;
	opt.get("-t")->get(type);

	std::string inputType;
	opt.get("input")->get(inputType);

	std::string outputfile;
	opt.get("-o")->get(outputfile);

	std::string folder;
	opt.get("-D")->get(folder);


	//processing
	try {
		//start processing
		//end processing
	}
	catch (...) {

	}

	return 0;
}