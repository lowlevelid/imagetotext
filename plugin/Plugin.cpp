#include <napi.h>
#include <stdio.h>
#include <stdlib.h>
#include "Type.hpp"
#include "ITTAsyncWorker.hpp"

namespace itt 
{

static Napi::Value ScanText(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsBuffer())
    {
        Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String tessdata_path = info[0].As<Napi::String>();
    std::string tessdata_path_str = tessdata_path.Utf8Value();

    auto buf = info[1].As<Napi::Buffer<itt::type::Byte>>();

    ITTAsyncWorker* aw = new ITTAsyncWorker(env, tessdata_path_str, buf);
    auto promise = aw->GetPromise();
    aw->Queue();
    
    return promise;
}

static void Preprocess(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 3)
    {
        Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsBuffer())
    {
        Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
        return;
    }

    if (!info[2].IsFunction())
    {
        Napi::TypeError::New(env, "third argument should be function").ThrowAsJavaScriptException();
        return;
    }

    Napi::Function callback = info[2].As<Napi::Function>();

    Napi::String format = info[0].As<Napi::String>();
    auto buf = info[1].As<Napi::Buffer<itt::type::Byte>>();
    std::vector<itt::type::Byte> out;

    itt::ncv::NCv* ncv = itt::ncv::NCv::FromBuffer(buf);
    if (ncv == nullptr)
        callback.Call(env.Global(), { Napi::String::New(env, "error initialize ncv"), env.Null() });

    int blur_ok = ncv->Preprocess(format, out);
    if (blur_ok != 0)
        callback.Call(env.Global(), { Napi::String::New(env, "error execute preprocess data"), env.Null() });

    // free memory
    delete ncv;

    callback.Call(env.Global(), {
        env.Null(), 
        Napi::Buffer<itt::type::Byte>::Copy(env, out.data(), out.size())
    });
}

static void SetContrast(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 5)
    {
        Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsBuffer())
    {
        Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
        return;
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "third argument should be number").ThrowAsJavaScriptException();
        return;
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "fourth argument should be number").ThrowAsJavaScriptException();
        return;
    }

    if (!info[4].IsFunction())
    {
        Napi::TypeError::New(env, "fifth argument should be function").ThrowAsJavaScriptException();
        return;
    }

    Napi::Function callback = info[4].As<Napi::Function>();

    Napi::String format = info[0].As<Napi::String>();
    auto buf = info[1].As<Napi::Buffer<itt::type::Byte>>();
    double alpha = info[2].As<Napi::Number>().DoubleValue();
    double beta = info[2].As<Napi::Number>().DoubleValue();

    std::vector<itt::type::Byte> out;

    itt::ncv::NCv* ncv = itt::ncv::NCv::FromBuffer(buf);
    if (ncv == nullptr)
        callback.Call(env.Global(), { Napi::String::New(env, "error initialize ncv"), env.Null() });

    int blur_ok = ncv->SetContrast(format, out, alpha, beta);
    if (blur_ok != 0)
        callback.Call(env.Global(), { Napi::String::New(env, "error setting contrast"), env.Null() });

    // free memory
    delete ncv;

    callback.Call(env.Global(), {
        env.Null(), 
        Napi::Buffer<itt::type::Byte>::Copy(env, out.data(), out.size())
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "scanText"), Napi::Function::New(env, ScanText));
    exports.Set(Napi::String::New(env, "preprocess"), Napi::Function::New(env, Preprocess));
    exports.Set(Napi::String::New(env, "setContrast"), Napi::Function::New(env, SetContrast));

    return exports;
}

NODE_API_MODULE(itt, Init);

}