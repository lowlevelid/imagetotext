#ifndef HEADER_CV_H
#define HEADER_CV_H

#include <napi.h>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "Type.hpp"
#include "Base.hpp"
#include "Logger.hpp"

namespace itt 
{
namespace ncv
{

static const int MAX_KERNEL_LENGTH = 31;

int MatFromRaw(const std::vector<itt::type::Byte>& src, cv::Mat& dst);

static int AddBorder(const cv::Mat& img, cv::Mat& dst);
static int GammaCorrection(const cv::Mat& img, cv::Mat& dst, const double gamma);
static int ConvertToBlackWhiteAndAddBorder(const cv::Mat& img, cv::Mat& dst, const double threshold);
static int CropBounding(const cv::Mat& img, cv::Mat& dst);

int MatFromRaw(const std::vector<itt::type::Byte>& src, cv::Mat& dst)
{
    try
    {
        cv::Mat raw_mat(1, src.size(), CV_8UC1, (void*) src.data());
        dst = cv::imdecode(raw_mat, cv::IMREAD_COLOR|cv::IMREAD_ANYDEPTH);
    }
    catch(const cv::Exception& e)
    {
        itt::Logger(itt::LogLevel::ERROR) << e.what();
        return -1;
    }
    
    return 0;
}

static int AddBorder(const cv::Mat& img, cv::Mat& dst)
{
    int border_type = cv::BORDER_CONSTANT;

    int top, bottom, left, right;

    top = (int) (0.003 * img.rows);
    bottom = top;

    left = (int) (0.03 * img.cols);
    right = left;

    cv::Scalar value(159, 158, 157);

    try
    {
        cv::copyMakeBorder(img, dst, top, bottom, left, right, border_type, value);
    }
    catch(const cv::Exception& e)
    {
        itt::Logger(itt::LogLevel::ERROR) << e.what();
        return -1;
    }
    

    return 0;
}

static int GammaCorrection(const cv::Mat& img, cv::Mat& dst, const double gamma)
{
    if (gamma < 0.0 || gamma > 200.0)
        return -0;
    
    cv::Mat look_up_table(1, 256, CV_8U);

    try
    {
        uchar* p = look_up_table.ptr();

        for (int i = 0; i < 256; i++)
            p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
        
        dst = img.clone();

        cv::LUT(img, look_up_table, dst);
    }
    catch(const cv::Exception& e)
    {
        itt::Logger(itt::LogLevel::ERROR) << e.what();
        return -1;
    }
    

    return 0;
}

static int ConvertToBlackWhiteAndAddBorder(const cv::Mat& img, cv::Mat& dst, const double threshold)
{
    cv::Mat src_gray;
    cv::Mat dst_gray;
    cv::Mat dst_inverted;
    cv::Mat dst_morp;

    try
    {
        cv::Mat kernel = cv::Mat::ones(2, 2, CV_8U);

        cv::cvtColor(img, src_gray, cv::COLOR_BGR2GRAY);
        cv::threshold(src_gray, dst_gray, 130, 255, cv::THRESH_BINARY);
        // cv::adaptiveThreshold(src_gray, dst_gray, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 2);
        cv::bitwise_not(dst_gray, dst_inverted);
        
        cv::morphologyEx(dst_inverted, dst_morp, cv::MORPH_OPEN, kernel);
    }
    catch(const cv::Exception& e)
    {
        itt::Logger(itt::LogLevel::ERROR) << e.what();
        return -1;
    }

    if (AddBorder(dst_morp, dst) != 0)
        return -1;

    return 0;
}

static int CropBounding(const cv::Mat& img, cv::Mat& dst)
{
    int safe_dist = 25;
    cv::Mat src_gray;
    cv::Mat dst_gray;
    cv::Mat dst_inverted;
    cv::Mat dst_non_zero;
    cv::Mat dst_morp;

    cv::Mat kernel = cv::Mat::ones(2, 2, CV_8U);
    
    try
    {
        cv::cvtColor(img, src_gray, cv::COLOR_BGR2GRAY);
        cv::threshold(src_gray, dst_gray, 130, 255, cv::THRESH_BINARY);
        cv::bitwise_not(dst_gray, dst_inverted);

        cv::morphologyEx(dst_inverted, dst_morp, cv::MORPH_OPEN, kernel);
        cv::findNonZero(dst_morp, dst_non_zero);

        cv::Rect boundedRect = cv::boundingRect(dst_non_zero);
        dst = img(cv::Range(boundedRect.y-safe_dist, boundedRect.y+boundedRect.height+safe_dist), 
            cv::Range(boundedRect.x-safe_dist, boundedRect.x+boundedRect.width+safe_dist));
    }
    catch(const cv::Exception& e)
    {
        itt::Logger(itt::LogLevel::ERROR) << "error execute CropBounding, code: " << e.code;
        itt::Logger(itt::LogLevel::ERROR) << "err: " << e.err;
        itt::Logger(itt::LogLevel::ERROR) << e.what();
        if (e.code == -215)
            return 1;

        return -1;
    }
    
    return 0;
}

class NCv
{
    public:
    NCv()
    {
        
    }

    NCv(const cv::Mat& _mat)
    {
        this->mat = _mat;
    }

    NCv(itt::type::Byte* buffer, size_t buffer_size)
    {
        cv::Mat raw_mat(1, buffer_size, CV_8UC1, (void*) buffer);
        cv::Mat _mat = cv::imdecode(raw_mat, cv::IMREAD_COLOR|cv::IMREAD_ANYDEPTH);
        this->mat = _mat;
    }

    NCv(const std::vector<itt::type::Byte>& buffer)
    {
        MatFromRaw(buffer, this->mat);
    }

    ~NCv()
    {
        
    }

    public:
    cv::Mat Mat() const
    {
        return mat;
    }

    int GaussianBlur(const std::string& format, std::vector<itt::type::Byte>& out)
    {
        itt::Logger() << "performing GaussianBlur";
        
        cv::Mat dst = mat.clone();

        for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
            cv::GaussianBlur(mat, dst, cv::Size( i, i ), 0, 0 );
        
        // cv::imwrite("dst.jpg", dst);

        // int size = dst.total() * dst.elemSize();
        // *out = new itt::type::Byte[size];
        // memcpy(*out, dst.data, size * sizeof(itt::type::Byte));

        // *out = new itt::type::Byte[data.Length()];
        // memcpy(*out, datas, data.Length() * sizeof(itt::type::Byte));

        // encode Mat to memory buffer
        // opencv supported format https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
        cv::imencode(format, dst, out);

        return 0;
    }

    int DrawElipse(const std::string& format, const itt::type::Color& color, 
        int w, int h, double angle, std::vector<itt::type::Byte>& out)
    {
        int thickness = 2;
        int lineType = 8;

        cv::Mat img = cv::Mat::zeros(w, h, CV_8UC3);
        cv::ellipse(img,
            cv::Point( w/2, h/2 ),
            cv::Size( w/4, h/16 ),
            angle,
            0,
            360,
            cv::Scalar( color.B, color.G, color.R ),
            thickness,
            lineType );

        cv::imencode(format, img, out);

        return 0;
    }

    int Preprocess(cv::Mat& out)
    {
        cv::Mat cloned_original = this->mat;
        // cv::Mat corrected_gama;
        // cv::Mat dst_cropped;

        // double alpha = 1.0;
        // double beta = 50;
        // cv::Mat new_image = cv::Mat::zeros(this->mat.size(), this->mat.type());
        // this->mat.convertTo(new_image, -1, alpha, beta);

        // if (GammaCorrection(new_image, corrected_gama, 0.4) != 0)
        //     return -1;

        int cropped_result = CropBounding(cloned_original, out);
        itt::Logger() << "cropped_result: " << cropped_result;
        if (cropped_result == -1)
            return -1;
        
        if (cropped_result == 1) 
        {
            out = this->mat;
            return 0;
        }
        
        // if (ConvertToBlackWhiteAndAddBorder(dst_cropped, out, 100))
        //     return -1;

        // if (AddBorder(this->mat, out) != 0)
        //     return -1;

        
        return 0;
    }

    int Preprocess(const std::string& format, std::vector<itt::type::Byte>& out)
    {
        cv::Mat dst;
        if (Preprocess(dst) != 0)
            return -1;

        cv::imencode(format, dst, out);

        return 0;
    }

    int SetContrast(const std::string& format, std::vector<itt::type::Byte>& out, 
        double alpha, double beta)
    {
        if (alpha < 1.0 || alpha > 3.0)
            return -1;
        if (beta < 0 || beta > 100.0)
            return -1;

        cv::Mat new_image = cv::Mat::zeros(this->mat.size(), this->mat.type());

        try
        {
            this->mat.convertTo(new_image, -1, alpha, beta);
        }
        catch(const cv::Exception& e)
        {
            itt::Logger(itt::LogLevel::ERROR) << e.what();
            return -1;
        }
        

        cv::Mat corrected_gama;

        if (GammaCorrection(new_image, corrected_gama, 0.4) != 0)
            return -1;

        cv::imencode(format, corrected_gama, out);
        return 0;
    }

    public:
    static NCv* FromBuffer(const Napi::Buffer<itt::type::Byte>& input)
    {
        itt::type::Byte* raw_data = reinterpret_cast<itt::type::Byte*>(input.Data());
        const size_t size_data = input.Length();

        NCv* out = new NCv(raw_data, size_data);

        return out;
        
    }

    private:
    cv::Mat mat;
    
};

} // ncv
} // itt

#endif