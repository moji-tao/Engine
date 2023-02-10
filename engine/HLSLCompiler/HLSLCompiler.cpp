#include <iostream>
#include <filesystem>
#include <fstream>
#include <d3dcompiler.h>
#include <Windows.h>
#include <wrl/client.h>
#include "cxxopts.hpp"
#include "json/json.h"

using namespace std;

struct ShaderInfo
{
    bool isDebug = false;
    string entryPoint;
    string inputFile;
    string outputFile;
    string version;
};

ShaderInfo gInfo;
string jsonF;

void Init(cxxopts::Options& options)
{
    options.positional_help("[Positional help]").show_positional_help();

    options.set_width(200).add_options("Compiler")
        ("i,input", "需要编译的Shader文件", cxxopts::value<string>())
        ("o,out", "输出文件", cxxopts::value<string>())
        ("d,debug", "以调试模式进行编译", cxxopts::value<bool>())
        ("e,entry", "着色器入口点", cxxopts::value<string>())
        ("v,version", "着色器版本", cxxopts::value<string>())
        ("j,json", "Json批处理文件", cxxopts::value<string>());
}

bool Parse(cxxopts::Options& options, int argc, const char** argv)
{
    try
    {
        auto result = options.parse(argc, argv);

        if (result.count("json"))
        {
	        jsonF = result["json"].as<string>();
            return true;
        }

        if (result.count("input"))
        {
            gInfo.inputFile = result["input"].as<string>();
        }
        else
        {
            throw cxxopts::OptionException("没有输入文件");
        }

        if (result.count("out"))
        {
            gInfo.outputFile = result["out"].as<string>();
        }
        else
        {
            gInfo.outputFile = gInfo.inputFile + ".cso";
        }

        if (result.count("debug"))
        {
            gInfo.isDebug = result["debug"].as<bool>();
        }

        if (result.count("entry"))
        {
            gInfo.entryPoint = result["entry"].as<string>();
        }
        else
        {
            cout << "WARNING: 没有指定入口点, 将入口点设为默认值 main " << endl;
            gInfo.entryPoint = "main";
        }

        if (result.count("version"))
        {
            gInfo.version = result["version"].as<string>();
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

bool CompilerShader(const std::filesystem::path& inputFilePath, const std::filesystem::path& outputFilePath, const std::string& entryPoint, const std::string& version, bool isDebug)
{
    if (!(filesystem::exists(inputFilePath) && filesystem::is_regular_file(inputFilePath)))
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY |
            FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
            FOREGROUND_RED);
        cout << "Shader文件不存在 " << inputFilePath << endl;
        return true;
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
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY |
            FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
            FOREGROUND_RED);
        cout << "Shader文件编译出错" << endl;
        cout << (char*)errors->GetBufferPointer() << endl;
        return false;
    }

    OutputFile(outputFilePath, (char*)byteCode->GetBufferPointer(), byteCode->GetBufferSize());

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << "输入: " << inputFilePath
        << "\n输出: " << outputFilePath
        << "\n代码入口点: " << entryPoint
        << "\n着色器版本: " << version << "\n"
        << (isDebug ? "Debug版本\n" : "Release版本\n")
        << endl;

    return true;
}

//void LoadJson(Json::Value)

void Run(int argc, const char** argv)
{
    cxxopts::Options options("123");
    Init(options);
    if (!Parse(options, argc, argv))
    {
        return;
    }

    if (!jsonF.empty())
    {
        Json::Reader reader;
        Json::Value root;
        auto jsonPath = filesystem::current_path() / jsonF;
        if (true)
        {
            if (!(filesystem::exists(jsonPath) && filesystem::is_regular_file(jsonPath)))
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY |
                    FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
                    FOREGROUND_RED);
                cout << "批处理Json信息不存在" << endl;
                return;
            }
            // 创建一个暂存容器
            Json::CharReaderBuilder builder;
            // 接收数据
            Json::Value root;
            // 接收错误数据
            JSONCPP_STRING errs;
            // 打开文件流
            ifstream ifs;
            ifs.open(jsonPath);

            // 核心代码
            bool parse_ok = Json::parseFromStream(builder, ifs, &root, &errs);
            ifs.close();
            if (!parse_ok)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY |
                    FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE |
                    FOREGROUND_RED);
                cout << "打开文件出错" << endl;
                return;
            }

            filesystem::path inputRootPath = jsonPath.parent_path();

            filesystem::path outputRootPath = jsonPath.parent_path() / root["OutPathRoot"].asString();

            bool defaultDebug = root["DefaultDebug"].asBool();

            for (auto& it : root["ShaderInfo"])
            {
                string shaderName = it["ShaderName"].asString();
                string entryPoint = it["EntryPoint"].asString();
                string version = it["Version"].asString();
                
                Json::Value dv = it["Debug"];
                bool isDebug;
                if (dv.empty())
                {
                    isDebug = defaultDebug;
                }
                else
                {
                    isDebug = dv.asBool();
                }

                filesystem::path inputPath = inputRootPath / (shaderName + ".hlsl");
                filesystem::path outPath = outputRootPath / (shaderName + entryPoint + ".cso");
                CompilerShader(inputPath, outPath, entryPoint, version, isDebug);
            }
        }
    }
    else
    {
        
        filesystem::path inputFilePath = filesystem::current_path() / gInfo.inputFile;
        filesystem::path outputFilePath = filesystem::current_path() / gInfo.outputFile;

        CompilerShader(inputFilePath, outputFilePath, gInfo.entryPoint, gInfo.version, gInfo.isDebug);
    }
}

int main(int argc, const char** argv)
{
    Run(argc, argv);
	return 0;
}