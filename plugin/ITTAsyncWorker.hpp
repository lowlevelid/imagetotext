#ifndef HEADER_ITT_ASYNC_WORKER_H
#define HEADER_ITT_ASYNC_WORKER_H

#include <napi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "Type.hpp"
#include "NOpenCv.hpp"
#include "Logger.hpp"

namespace itt 
{
class ITTAsyncWorker : public Napi::AsyncWorker
{
public:
    ITTAsyncWorker(Napi::Env& env,
        const std::string& tessdata_path, 
        const Napi::Buffer<itt::type::Byte>& input) : Napi::AsyncWorker(env),
        deferred(Napi::Promise::Deferred::New(env)) {
        this->ncv = itt::ncv::NCv::FromBuffer(input);
        if (ncv == nullptr)
        {
            SetError("error init opencv");
        }

        this->tess_api = new tesseract::TessBaseAPI();
        if (tess_api->Init(tessdata_path.data(), "eng", tesseract::OEM_LSTM_ONLY))
        {
            SetError("error init tesseract base api");
        }

        this->tess_api->SetPageSegMode(tesseract::PSM_AUTO);
    };

    ~ITTAsyncWorker() 
    {
        delete this->tess_api;
        delete this->ncv;
    }

    void Execute() 
    {
        // cv::Mat dst = this->ncv->Mat();

        std::vector<itt::type::Byte> dst_vec;
        cv::Mat dst;
        if (this->ncv->Preprocess(".png", dst_vec) != 0)
            SetError("error execute preprocess data | Preprocess");
        
        if (itt::ncv::MatFromRaw(dst_vec, dst) != 0)
            SetError("error execute preprocess data | MatFromRaw");

        int color_channels = dst.channels();

        this->tess_api->SetImage(dst.data, 
            dst.cols, 
            dst.rows, 
            color_channels, 
            color_channels*dst.cols);

        itt::Logger() << "rows: " << dst.rows;
        itt::Logger() << "cols: " << dst.cols;
        itt::Logger() << "channels: " << color_channels;
        itt::Logger() << "type: " << dst.type();
        
        this->outdata = std::string(this->tess_api->GetUTF8Text());

        tess_api->End();
    }

    void OnOK() 
    {
        Napi::HandleScope scope(Env());
        deferred.Resolve( Napi::String::New(Env(), outdata));
    }

    void OnError(const Napi::Error& e) 
    {
        deferred.Reject(e.Value());
    }

    Napi::Promise GetPromise() { return deferred.Promise(); }

private:
    std::string outdata;
    itt::ncv::NCv* ncv;
    tesseract::TessBaseAPI* tess_api;
    Napi::Promise::Deferred deferred;
}; // ITTAsyncWorker
} // itt

#endif

