/*
 * Convert from OpenCV image and write movie with FFmpeg
 *
 * Copyright (c) 2016 yohhoy
 */
#include <iostream>
#include <vector>
// FFmpeg
extern "C" {
#include "include/libavutil/motion_vector.h"
#include "include/libavformat/avformat.h"
#include "include/libswscale/swscale.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/avutil.h"
#include "include/libavutil/pixdesc.h"
#include "include/libswscale/swscale.h"
}
// OpenCV
#include "include/opencv2/opencv.hpp"
#include "include/opencv2/highgui.hpp"


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: cv2ff <outfile>" << std::endl;
        return 1;
    }
    const char* outfile = argv[1];

    // initialize FFmpeg library
    av_register_all();
//  av_log_set_level(AV_LOG_DEBUG);
    int ret;

    const int dst_width = 640;
    const int dst_height = 480;
    const AVRational dst_fps = {30, 1};

    // initialize OpenCV capture as input frame generator
    cv::VideoCapture cvcap(0);
    if (!cvcap.isOpened()) {
        std::cerr << "fail to open cv::VideoCapture";
        return 2;
    }
    cvcap.set(cv::CAP_PROP_FRAME_WIDTH, dst_width);
    cvcap.set(cv::CAP_PROP_FRAME_HEIGHT, dst_height);

    // allocate cv::Mat with extra bytes (required by AVFrame::data)
    std::vector<uint8_t> imgbuf(dst_height * dst_width * 3 + 16);
    cv::Mat image(dst_height, dst_width, CV_8UC3, imgbuf.data(), dst_width * 3);

    // open output format context
    AVFormatContext* outctx = nullptr;
    ret = avformat_alloc_output_context2(&outctx, nullptr, nullptr, outfile);
    if (ret < 0) {
        std::cerr << "fail to avformat_alloc_output_context2(" << outfile << "): ret=" << ret;
        return 2;
    }

    // open output IO context
    ret = avio_open2(&outctx->pb, outfile, AVIO_FLAG_WRITE, nullptr, nullptr);
    if (ret < 0) {
        std::cerr << "fail to avio_open2: ret=" << ret;
        return 2;
    }

    // create new video stream
    AVCodec* vcodec = avcodec_find_encoder(outctx->oformat->video_codec);
    AVStream* vstrm = avformat_new_stream(outctx, vcodec);
    if (!vstrm) {
        std::cerr << "fail to avformat_new_stream";
        return 2;
    }
    avcodec_get_context_defaults3(vstrm->codec, vcodec);
    vstrm->codec->width = dst_width;
    vstrm->codec->height = dst_height;
    vstrm->codec->pix_fmt = vcodec->pix_fmts[0];
    vstrm->codec->time_base = vstrm->time_base = av_inv_q(dst_fps);
    vstrm->r_frame_rate = vstrm->avg_frame_rate = dst_fps;
    if (outctx->oformat->flags & AVFMT_GLOBALHEADER)
        vstrm->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // open video encoder
    ret = avcodec_open2(vstrm->codec, vcodec, nullptr);
    if (ret < 0) {
        std::cerr << "fail to avcodec_open2: ret=" << ret;
        return 2;
    }

    std::cout
        << "outfile: " << outfile << "\n"
        << "format:  " << outctx->oformat->name << "\n"
        << "vcodec:  " << vcodec->name << "\n"
        << "size:    " << dst_width << 'x' << dst_height << "\n"
        << "fps:     " << av_q2d(dst_fps) << "\n"
        << "pixfmt:  " << av_get_pix_fmt_name(vstrm->codec->pix_fmt) << "\n"
        << std::flush;

    // initialize sample scaler
    SwsContext* swsctx = sws_getCachedContext(
        nullptr, dst_width, dst_height, AV_PIX_FMT_BGR24,
        dst_width, dst_height, vstrm->codec->pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
    if (!swsctx) {
        std::cerr << "fail to sws_getCachedContext";
        return 2;
    }

    // allocate frame buffer for encoding
    AVFrame* frame = av_frame_alloc();
    std::vector<uint8_t> framebuf(avpicture_get_size(vstrm->codec->pix_fmt, dst_width, dst_height));
    avpicture_fill(reinterpret_cast<AVPicture*>(frame), framebuf.data(), vstrm->codec->pix_fmt, dst_width, dst_height);
    frame->width = dst_width;
    frame->height = dst_height;
    frame->format = static_cast<int>(vstrm->codec->pix_fmt);

    // encoding loop
    avformat_write_header(outctx, nullptr);
    int64_t frame_pts = 0;
    unsigned nb_frames = 0;
    bool end_of_stream = false;
    int got_pkt = 0;
    do {
        if (!end_of_stream) {
            // retrieve source image
            cvcap >> image;
            cv::imshow("press ESC to exit", image);
            if (cv::waitKey(33) == 0x1b)
                end_of_stream = true;
        }
        if (!end_of_stream) {
            // convert cv::Mat(OpenCV) to AVFrame(FFmpeg)
            const int stride[] = { static_cast<int>(image.step[0]) };
            sws_scale(swsctx, &image.data, stride, 0, image.rows, frame->data, frame->linesize);
            frame->pts = frame_pts++;
        }
        // encode video frame
        AVPacket pkt;
        pkt.data = nullptr;
        pkt.size = 0;
        av_init_packet(&pkt);
        ret = avcodec_encode_video2(vstrm->codec, &pkt, end_of_stream ? nullptr : frame, &got_pkt);
        if (ret < 0) {
            std::cerr << "fail to avcodec_encode_video2: ret=" << ret << "\n";
            break;
        }
        if (got_pkt) {
            // rescale packet timestamp
            pkt.duration = 1;
            av_packet_rescale_ts(&pkt, vstrm->codec->time_base, vstrm->time_base);
            // write packet
            av_write_frame(outctx, &pkt);
            std::cout << nb_frames << '\r' << std::flush;  // dump progress
            ++nb_frames;
        }
        av_free_packet(&pkt);
    } while (!end_of_stream || got_pkt);
    av_write_trailer(outctx);
    std::cout << nb_frames << " frames encoded" << std::endl;

    av_frame_free(&frame);
    avcodec_close(vstrm->codec);
    avio_close(outctx->pb);
    avformat_free_context(outctx);
    return 0;
}
