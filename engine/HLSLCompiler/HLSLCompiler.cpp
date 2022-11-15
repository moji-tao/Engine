#include <iostream>
#include <filesystem>
#include <fstream>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "cxxopts.hpp"

using namespace std;

void prase(int argc, const char* argv[]) {
    try {
        cxxopts::Options options("MyTest", "a test program"); //搭建Option类

        options.positional_help("[Positional help]").show_positional_help();

        options.set_width(70).allow_unrecognised_options().add_options("myopt")
            ("i,int", "a integer", cxxopts::value<int>())
            ("f,float", "a float", cxxopts::value<float>())
            ("vector", "a list of double", cxxopts::value<std::vector<double>>());

        auto result = options.parse(argc, argv);
        if (result.count("int")) {
            std::cout << "int = " << result["int"].as<int>() << std::endl;
        }
        if (result.count("float")) {
            std::cout << "float = " << result["float"].as<float>() << std::endl;
        }
        if (result.count("vector")) {
            std::cout << "vector = ";
            const auto res = result["vector"].as<std::vector<double>>();
            for (const auto v : res) {
                std::cout << v << " , ";
            }
            std::cout << std::endl;
        }
        const auto res = result.unmatched(); //收取那些没有被定义的字符串
        std::cout << "unmatched opt: ";
        for (auto vs : res) {
            std::cout << vs << " , ";
        }
        std::cout << std::endl;
        std::cout << "Saw " << result.arguments().size() << " arguments" << std::endl;
    }
    catch (cxxopts::OptionException& e) {
        std::cout << "parse error " << e.what() << std::endl;
    }
}

bool isDebug = false;
string entryPoint;
string inputFile;
string outputFile;
string version;


void Init(cxxopts::Options& options)
{
    options.positional_help("[Positional help]").show_positional_help();

    options.set_width(200).add_options("Compiler")
        ("i,input", "需要编译的Shader文件", cxxopts::value<string>())
        ("o,out", "输出文件", cxxopts::value<string>())
        ("d,debug", "以调试模式进行编译", cxxopts::value<bool>())
        ("e,entry", "着色器入口点", cxxopts::value<string>())
        ("v,version", "着色器版本", cxxopts::value<string>());
}

bool Parse(cxxopts::Options& options, int argc, const char** argv)
{
    try
    {
        auto result = options.parse(argc, argv);

        if (result.count("input"))
        {
            inputFile = result["input"].as<string>();
        }
        else
        {
            throw cxxopts::OptionException("没有输入文件");
        }

        if (result.count("out"))
        {
            outputFile = result["out"].as<string>();
        }
        else
        {
            outputFile = inputFile + ".cso";
        }

        if (result.count("debug"))
        {
            isDebug = result["debug"].as<bool>();
        }

        if (result.count("entry"))
        {
            entryPoint = result["entry"].as<string>();
        }
        else
        {
            cout << "WARNING: 没有指定入口点, 将入口点设为默认值 main " << endl;
            entryPoint = "main";
        }

        if (result.count("version"))
        {
            version = result["version"].as<string>();
        }
        else
        {
            throw cxxopts::OptionException("没有输入着色器版本");
        }
    }
    catch (cxxopts::OptionException& e)
    {
        cout << "参数错误 " << e.what() << endl;
        return false;
    }
    return true;
}

void OutputFile(const filesystem::path& outputFilePath, char* data, size_t length)
{
    ofstream oFile(outputFilePath, ios::binary | ios::trunc);
    if(oFile.is_open())
    {
        oFile.write(data, length);
        oFile.close();
    }
    else
    {
        cout << outputFilePath << " 文件打开失败" << endl;
    }
}

void Run(int argc, const char** argv)
{
    cxxopts::Options options("123");
    Init(options);
    if (!Parse(options, argc, argv))
    {
        return;
    }

    filesystem::path inputFilePath = filesystem::current_path() / inputFile;
    filesystem::path outputFilePath = filesystem::current_path() / outputFile;

    if (!(filesystem::exists(inputFilePath) && filesystem::is_regular_file(inputFilePath)))
    {
        cout << "Shader文件不存在 " << inputFilePath << endl;
        return;
    }

    unsigned compileFlags = 0;
    if (isDebug)
    {
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    }
    Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    D3DCompileFromFile(inputFilePath.generic_wstring().c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), version.c_str(),
        compileFlags, 0, byteCode.GetAddressOf(), errors.GetAddressOf());
    if (errors != nullptr)
    {
        cout << "Shader文件编译出错" << endl;
        cout << (char*)errors->GetBufferPointer() << endl;
        return;
    }

    OutputFile(outputFilePath, (char*)byteCode->GetBufferPointer(), byteCode->GetBufferSize());

    cout << "输入: " << inputFile
        << "\n输出: " << outputFile
        << "\n代码入口点: " << entryPoint
        << "\n着色器版本: " << version << "\n"
        << (isDebug ? "Debug版本" : "Release版本")
        << endl;
}

int main(int argc, const char** argv)
{
    Run(argc, argv);

    system("pause");
    return 0;
}