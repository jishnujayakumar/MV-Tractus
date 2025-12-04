/*
 * Convert from OpenCV image and write movie with FFmpeg
 *
 * Copyright (c) 2018 Jishnu Jaykumar P
 */
#include <iostream>
#include <vector>
#include <cstring> //http://cs.stmarys.ca/~porter/csc/ref/c_cpp_strings.html

// FFmpeg
extern "C" {
  #include <libavutil/motion_vector.h>
// #include "../include/libavutil/motion_vector.h"
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
// #include "include/zeromq-4.2.2/include/zmq.h"
}

// OpenCV
#include <opencv2/opencv.hpp>
#include <zmq>
// #include "include/zhelpers.hpp"

using namespace std;
using namespace cv;

class FMV{
private:
  AVFormatContext *pFormatCtx = NULL;
  // static AVCodecContext *video_dec_ctx = NULL;
  AVStream *video_stream = NULL;
  const char *src_filename = NULL;
  AVCodecContext    *pCodecCtxOrig = NULL;
  AVCodecContext    *pCodecCtx = NULL;
  AVCodec           *pCodec = NULL;

  int video_stream_idx = -1;
  AVFrame *pFrame = NULL;
  AVFrame *pFrameRGB = NULL;
  int video_frame_count = 0;
  int               frameFinished;
  int               numBytes;
  uint8_t           *buffer = NULL;
  struct SwsContext *sws_ctx = NULL;
  int argc;
  bool gotFrame;
  Mat cv_mat;
  // compatibility with newer API
  #if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
  #define av_frame_alloc avcodec_alloc_frame
  #define av_frame_free avcodec_free_frame
  #endif

public:
  FMV(int argc, char* videopath){
    cout<<"line 50: FMV constructor called\n";
    this->argc = argc;
    this->src_filename=videopath;
  };

  const char* getSrcFilename(){
    return this->src_filename;
  }

  void setFrameFlag(bool flag){
    this->gotFrame=flag;
  }

  bool getFrameFlag(){
    return this->gotFrame;
  }



  void connectRTMP(){
    cout<<"line 58: connectRTMP() called\n";
    int ret = 0;
    AVPacket pkt = { 0 };

    if (argc != 2) {
        if (this->getSrcFilename()==NULL) {
          cout<< "Error: No input file given\n";
          exit(1);
        }else{
        fprintf(stderr, "Usage: %s <video>\n", this->getSrcFilename());
        exit(1);
      }
    }

    // src_filename = videopath;

    if (avformat_open_input(&(this->pFormatCtx), this->getSrcFilename(), NULL, NULL) < 0) {
        fprintf(stderr, "Error:Could not open source file %s\n", this->getSrcFilename());
        exit(1);
    }

    if (avformat_find_stream_info(this->pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    open_codec_context(this->pFormatCtx, AVMEDIA_TYPE_VIDEO);
  }

  int open_codec_context(AVFormatContext *pFormatCtx, enum AVMediaType type)
  {
      int ret;
      AVStream *st;
      AVCodecContext *dec_ctx = NULL;
      AVCodec *dec = NULL;
      AVDictionary *opts = NULL;

      ret = av_find_best_stream(this->pFormatCtx, type, -1, -1, &dec, 0);
      if (ret < 0) {
          fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                  av_get_media_type_string(type), this->getSrcFilename());
          return ret;
      } else {
          int stream_idx = ret;
          st = this->pFormatCtx->streams[stream_idx];

          dec_ctx = avcodec_alloc_context3(dec);
          if (!dec_ctx) {
              fprintf(stderr, "Failed to allocate codec\n");
              return AVERROR(EINVAL);
          }

          ret = avcodec_parameters_to_context(dec_ctx, st->codecpar);
          if (ret < 0) {
              fprintf(stderr, "Failed to copy codec parameters to codec context\n");
              return ret;
          }

          /* Init the video decoder */
          av_dict_set(&opts, "flags2", "+export_mvs", 0);
          if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
              fprintf(stderr, "Failed to open %s codec\n",
                      av_get_media_type_string(type));
              return ret;
          }

          this->video_stream_idx = stream_idx;
          this->video_stream = this->pFormatCtx->streams[this->video_stream_idx];
          this->pCodecCtx = dec_ctx;
      }

      return 0;
  }

  void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "./v2/output/frames/frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
      return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
      fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
  }

  void debug_print(char* str){
    bool value=true;//set to false to unprint
    if(value)
      cout<<str<<endl;
  }

  // zmq::socket_t fifo_connect(){
  //   //  Prepare our context and publisher
  //   zmq::context_t context(1);
  //   zmq::socket_t publisher(context, ZMQ_PUB);
  //   publisher.bind("tcp://*:5563");
  //   // return publisher;
  // }

  int fifo_write(zmq::socket_t publisher, Mat mat) {

    // while (1) {

      //  Write two messages, each with an envelope and content
      // s_sendmore (publisher, "A");
      // s_send (publisher, "We don't want to see this");
      s_sendmore (publisher, "MAT");
      s_send (publisher, "We would like to see this");
      // sleep (1);
    // }
    return 0;
  }

  int decode_packet(const AVPacket *pkt)
  {
    cout<<"line 134: decode_packet() called\n";
    cout<<this->pCodecCtx;
    int ret = avcodec_send_packet(this->pCodecCtx, pkt);

    char szFileName[255] = {0};
    // debug_print("2");
    FILE *file=NULL;
    if (ret < 0) {
        fprintf(stderr, "Error while sending a packet to the decoder\n");
        return ret;
    }
    // debug_print("3");
    while (ret >= 0)  {
        ret = avcodec_receive_frame(this->pCodecCtx, this->pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            fprintf(stderr, "Error while receiving a frame from the decoder: \n");
            return ret;
        }

        if (ret >= 0) {
            int i;
            AVFrameSideData *sd;
	          this->video_frame_count++;
            sws_scale(this->sws_ctx, (uint8_t const * const *)this->pFrame->data,
                this->pFrame->linesize, 0, this->pCodecCtx->height,
                this->pFrameRGB->data, this->pFrameRGB->linesize);

            //get convert frame to Mat
            this->cv_mat=Mat(this->pCodecCtx->height, this->pCodecCtx->width, CV_8UC3, this->pFrameRGB->data[0], this->pFrameRGB->linesize[0]);
            // this->fifo_write(cv_mat);
            // imshow("frame", cv_mat);
            // waitKey(10);

            // cout<<cv_mat;

            // Save the frame to disk
            // SaveFrame(this->pFrameRGB, this->pCodecCtx->width, this->pCodecCtx->height,video_frame_count);

            sprintf(szFileName, "./v2/output/mv/%d.json", this->video_frame_count);
            file = fopen(szFileName,"w");
            if (file == NULL)
            {
                fprintf(stderr, "Couldn't open file for reading\n");
                exit(1);
            }
            fprintf(file, "[\n");
            sd = av_frame_get_side_data(this->pFrame, AV_FRAME_DATA_MOTION_VECTORS);
            if (sd) {
                const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
                for (i = 0; i < sd->size / sizeof(*mvs); i++) {
                    const AVMotionVector *mv = &mvs[i];
                    // print_motion_vectors_data(mv, video_frame_count);
                    if(i==(sd->size / sizeof(*mvs))-1){
                      fprintf(file, "\t{\n");
                      fprintf(file, "\t\t\"source\" : %d,\n", mv->source);
                      fprintf(file, "\t\t\"width\" : %d,\n", mv->w);
                      fprintf(file, "\t\t\"height\" : %d,\n", mv->h);
                      if(mv->source<0){
                        fprintf(file, "\t\t\"src_x\" : %d,\n", ((mv->src_x)/abs(mv->source)));
                        fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->src_y)/abs(mv->source));
                        fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->dst_x - mv->src_x)/abs(mv->source)));
                        fprintf(file, "\t\t\"dy\" : %d\n", ((mv->dst_y - mv->src_y)/abs(mv->source)));
                      }else{
                        fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
                        fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->src_x/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->src_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->src_x - mv->dst_x)/abs(mv->source)));
                        fprintf(file, "\t\t\"dy\" : %d\n", ((mv->src_y - mv->dst_y)/abs(mv->source)));
                      }
                      fprintf(file, "\t}\n");
                    }else{
                      fprintf(file, "\t{\n");
                      fprintf(file, "\t\t\"source\" : %d,\n", mv->source);
                      fprintf(file, "\t\t\"width\" : %d,\n", mv->w);
                      fprintf(file, "\t\t\"height\" : %d,\n", mv->h);
                      fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->src_x/abs(mv->source)));
                      fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->src_y/abs(mv->source)));
                      fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
                      fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
                      if(mv->source<0){
                        fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->src_x/abs(mv->source)));
                        fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->src_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->dst_x - mv->src_x)/abs(mv->source)));
                        fprintf(file, "\t\t\"dy\" : %d\n", ((mv->dst_y - mv->src_y)/abs(mv->source)));
                      }else{
                        fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
                        fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->src_x/abs(mv->source)));
                        fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->src_y/abs(mv->source)));
                        fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->src_x - mv->dst_x)/abs(mv->source)));
                        fprintf(file, "\t\t\"dy\" : %d\n", ((mv->src_y - mv->dst_y)/abs(mv->source)));
                      }
                      fprintf(file, "\t},\n");
                    }
                }
                // printf("%s\n", strcat( "./output/", strcat(video_frame_count, ".json")));
            }
            fprintf(file, "]\n");
            fclose(file);
	    printf("\rTotal Processed Frames:%d", this->video_frame_count);
	    fflush(stdout);
            //Print frame data
            // print_frame_data(frame);
            av_frame_unref(this->pFrame);
        }
    }
    // this->setFrameFlag(true);
    return this->video_frame_count;
    // return 0;
  }

  void code_meta_data(){
    printf("\n");
    printf("**************************************************************************************\n");
    printf("*       Tool : MV-Tractus                                                            *\n");
    printf("*     Author : Jishnu Jaykumar Padalunkal (https://jishnujayakumar.github.io)        *\n");
    printf("*  Used Libs : FFmpeg                                                                *\n");
    printf("*Description : A simple tool to extract motion vectors from MPEG videos              *\n");
    printf("**************************************************************************************\n");
    printf("\n");
    printf("--------------------------------------------------------------------------------------\n");
    printf("framenum,source,blockw,blockh,srcx,srcy,dstx,dsty,motion_x,motion_y,motion_scale,flags\n");
    printf("--------------------------------------------------------------------------------------\n");
  }

  int extract_motion_vectors(bool dump_av_format){
      cout<<"line 139: extract_motion_vectors() called\n";
      int ret = 0;
      AVPacket pkt = { 0 };
      debug_print("1");
      if(dump_av_format)
        av_dump_format(this->pFormatCtx, 0, this->src_filename, 0);
      debug_print("2");
      if (!this->video_stream) {
          fprintf(stderr, "Could not find video stream in the input, aborting\n");
          ret = 1;
          goto end;
      }

      debug_print("3");

      this->pFrame = av_frame_alloc();
      debug_print("4");
      if (!this->pFrame) {
          fprintf(stderr, "Could not allocate frame\n");
          ret = AVERROR(ENOMEM);
          goto end;
      }

      debug_print("5");

      // this->code_meta_data();

      // Allocate an AVFrame structure
      this->pFrameRGB=av_frame_alloc();
      if(this->pFrameRGB==NULL)
        return -1;
        debug_print("6");
      // Determine required buffer size and allocate buffer
      this->numBytes=avpicture_get_size(AV_PIX_FMT_BGR24, this->pCodecCtx->width,
    			      this->pCodecCtx->height);
      // this->numBytes=av_image_get_buffer_size(AV_PIX_FMT_BGR24, this->pCodecCtx->width,
      //         	this->pCodecCtx->height, pFrame->linesize[0]);
                debug_print("7");
      this->buffer=(uint8_t *)av_malloc(this->numBytes*sizeof(uint8_t));
      debug_print("8");
      // Assign appropriate parts of buffer to image planes in pFrameRGB
      // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
      // of AVPicture
      avpicture_fill((AVPicture *)this->pFrameRGB, this->buffer, AV_PIX_FMT_RGB24,
    		 this->pCodecCtx->width, this->pCodecCtx->height);
      // initialize SWS context for software scaling
      this->sws_ctx = sws_getContext(
             this->pCodecCtx->width,
    			   this->pCodecCtx->height,
    			   this->pCodecCtx->pix_fmt,
    			   this->pCodecCtx->width,
    			   this->pCodecCtx->height,
    			   AV_PIX_FMT_RGB24,
    			   SWS_BILINEAR,
    			   NULL,
    			   NULL,
    			   NULL
    			   );



      /* read frames from the file */
      while (av_read_frame(this->pFormatCtx, &pkt) >= 0) {
          if (pkt.stream_index == this->video_stream_idx){
                // printf("\nDecoding Packets\n");
                ret=this->decode_packet(&pkt);
                // ret = decode_packet(&pkt);
                // avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &p);

          }
          av_packet_unref(&pkt);
          if (ret < 0)
              break;
      }

      /* flush cached frames */
      this->decode_packet(NULL);
      printf("\n--------------------------------------------------------------------------------------\n");

  end:
      avcodec_free_context(&(this->pCodecCtx));
      avformat_close_input(&(this->pFormatCtx));
      av_frame_free(&(this->pFrame));
      return ret < 0;
  }

};

int main(int argc, char **argv){
  cout << "line 21: main() called\n";
  FMV fmv(argc, argv[1]);
  fmv.connectRTMP();
  // fmv.fifo_connect();

  // int i=0;
  // while(i++<10){
    // cout<<"i:"<<i<<endl;
    fmv.extract_motion_vectors(false); // Give true to dump_av_format
  // }
  //Pointer test
  // int* i=0;
  // cout<<i<<" | "<<&i<< " | ";//<< *i;
  return 0;
}
