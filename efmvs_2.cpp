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
#include <libavutil/time.h>
#include <string.h>
// #include <json/json.h>
}

// OpenCV
#include <opencv2/opencv.hpp>
#include <boost/python.hpp>

using namespace std;
using namespace cv;

class FMV{
private:
AVFormatContext   *pFormatCtx = NULL;
// static AVCodecContext *video_dec_ctx = NULL;
AVStream          *video_stream = NULL;
const char        *src_filename = NULL;
AVCodecContext    *pCodecCtxOrig = NULL;
AVCodecContext    *pCodecCtx = NULL;
AVCodec           *pCodec = NULL;

int               video_stream_idx = -1;
AVFrame           *pFrame = NULL;
AVFrame           *pFrameRGB = NULL;
AVPacket          *pkt=NULL;
enum              AVMediaType type;
int               video_frame_count = 0;
int               frameFinished;
int               numBytes;
uint8_t           *buffer = NULL;
struct SwsContext *sws_ctx = NULL;
int               argc;
bool              gotFrame;
Mat               cv_mat;
int               fps;
// char              mv_data[25000];
/*Creating a json array*/
// json_object       *jarray;
//
// /*Creating a json object*/
// json_object       *jobj;
//
// /*Creating a json object*/
json_object       *frame_obj;

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

public:
FMV(int argc, char* videopath){
  // cout<<"line 50: FMV constructor called\n";
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

Mat getMAT(){
  return this->cv_mat;
}
int getheight(){
  return this->pCodecCtx->height;
}
int getwidth(){
  return this->pCodecCtx->width;
}
int getline(){
  return this->pFrameRGB->linesize[0];
}

int64_t getPseudoRealTimeStamp(){
    this->pFormatCtx->start_time_realtime = av_gettime();
    return this->pFormatCtx->start_time_realtime;
}

int64_t getPseudoTimeStamp(){
    // this->pFormatCtx->start_time_realtime = av_gettime();
    return this->pFormatCtx->start_time;
}

int getFPS(){
  return this->fps;
}

// json_object * getFrameJson(){
//   return this->frame_obj;
// }

void connect(bool dump_av_format){
  // cout<<"line 58: connect() called\n";
  int ret = 0;
  AVPacket pkt = { 0 };
  this->code_meta_data();
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

  this->type = AVMEDIA_TYPE_VIDEO;

  open_codec_context();
  if(dump_av_format)
    av_dump_format(this->pFormatCtx, 0, this->src_filename, 0);

}

int open_codec_context()
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(this->pFormatCtx, this->type, -1, -1, &dec, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(this->type), this->getSrcFilename());
        return ret;
    } else {
        int stream_idx = ret;
        st = this->pFormatCtx->streams[stream_idx];
        this->fps=av_q2d(st->r_frame_rate);
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
                    av_get_media_type_string(this->type));
            return ret;
        }

        this->video_stream_idx = stream_idx;
        this->video_stream = this->pFormatCtx->streams[this->video_stream_idx];
        this->pCodecCtx = dec_ctx;
    }

    return 0;
}

int decode_packet()
{
  // cout<<"line 134: decode_packet() called\n";
  int ret = avcodec_send_packet(this->pCodecCtx, this->pkt);
  // char mv_string[1000]={};
  if (ret < 0) {
      fprintf(stderr, "Error while sending a packet to the decoder\n");
      return ret;
  }

  while (ret >= 0)  {
      ret = avcodec_receive_frame(this->pCodecCtx, this->pFrame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          break;
      } else if (ret < 0) {
          fprintf(stderr, "Error while receiving a frame from the decoder: \n");
          return ret;
      }

      // jarray = json_object_new_array();
      // frame_obj = json_object_new_object();

      if (ret >= 0) {
          int i;
          AVFrameSideData *sd;
          // this->video_frame_count++;
          this->increase_current_frame_number();
          sws_scale(this->sws_ctx, (uint8_t const * const *)this->pFrame->data,
              this->pFrame->linesize, 0, this->pCodecCtx->height,
              this->pFrameRGB->data, this->pFrameRGB->linesize);

          //convert frame to Mat
          this->cv_mat=Mat(this->pCodecCtx->height, this->pCodecCtx->width, CV_8UC3, this->pFrameRGB->data[0], this->pFrameRGB->linesize[0]);

          // imshow("frame", cv_mat);
          // waitKey(10);

          // cout<<cv_mat;

          // Save the frame to disk
          // this->SaveFrame(this->pFrameRGB, this->pCodecCtx->width, this->pCodecCtx->height,video_frame_count);

          // sprintf(szFileName, "./v2/output/mv/%d.json", this->video_frame_count);
          // file = fopen(szFileName,"w");
          // if (file == NULL)
          // {
          //     fprintf(stderr, "Couldn't open file for reading\n");
          //     exit(1);
          // // }
          // sprintf(mv_string, "[\n");
          // fprintf(file, "[\n");
          // sd = av_frame_get_side_data(this->pFrame, AV_FRAME_DATA_MOTION_VECTORS);
          // if (sd) {
          //     const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
          //     for (i = 0; i < sd->size / sizeof(*mvs); i++) {
          //         const AVMotionVector *mv = &mvs[i];
          //         // print_motion_vectors_data(mv, video_frame_count);
          //         if(i==(sd->size / sizeof(*mvs))-1){
          //           fprintf(file, "\t{\n");
          //           fprintf(file, "\t\t\"source\" : %d,\n", mv->source);
          //           fprintf(file, "\t\t\"width\" : %d,\n", mv->w);
          //           fprintf(file, "\t\t\"height\" : %d,\n", mv->h);
          //           if(mv->source<0){
          //             fprintf(file, "\t\t\"src_x\" : %d,\n", ((mv->src_x)/abs(mv->source)));
          //             fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->src_y)/abs(mv->source));
          //             fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->dst_x - mv->src_x)/abs(mv->source)));
          //             fprintf(file, "\t\t\"dy\" : %d\n", ((mv->dst_y - mv->src_y)/abs(mv->source)));
          //           }else{
          //             fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->src_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->src_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->src_x - mv->dst_x)/abs(mv->source)));
          //             fprintf(file, "\t\t\"dy\" : %d\n", ((mv->src_y - mv->dst_y)/abs(mv->source)));
          //           }
          //           fprintf(file, "\t}\n");
          //         }else{
          //           fprintf(file, "\t{\n");
          //           fprintf(file, "\t\t\"source\" : %d,\n", mv->source);
          //           fprintf(file, "\t\t\"width\" : %d,\n", mv->w);
          //           fprintf(file, "\t\t\"height\" : %d,\n", mv->h);
          //           fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->src_x/abs(mv->source)));
          //           fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->src_y/abs(mv->source)));
          //           fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
          //           fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
          //           if(mv->source<0){
          //             fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->src_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->src_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->dst_x - mv->src_x)/abs(mv->source)));
          //             fprintf(file, "\t\t\"dy\" : %d\n", ((mv->dst_y - mv->src_y)/abs(mv->source)));
          //           }else{
          //             fprintf(file, "\t\t\"src_x\" : %d,\n", (mv->dst_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"src_y\" : %d,\n", (mv->dst_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_x\" : %d,\n", (mv->src_x/abs(mv->source)));
          //             fprintf(file, "\t\t\"dst_y\" : %d,\n", (mv->src_y/abs(mv->source)));
          //             fprintf(file, "\t\t\"dx\" : %d,\n", ((mv->src_x - mv->dst_x)/abs(mv->source)));
          //             fprintf(file, "\t\t\"dy\" : %d\n", ((mv->src_y - mv->dst_y)/abs(mv->source)));
          //           }
          //           fprintf(file, "\t},\n");
          //         }
          //     }
          //     // printf("%s\n", strcat( "./output/", strcat(video_frame_count, ".json")));
          // }
          // fprintf(file, "]\n");
          // fclose(file);
          // cout << "\rTotal Processed Frames:"<<this->video_frame_count;


    printf("\rTotal Processed Frames:%d", this->video_frame_count);
    fflush(stdout);
    // cout<<std::flush;
    // cout<<"Total Processed Frames:"<<this->video_frame_count<<endl;
          //Print frame data
          // print_frame_data(frame);
          av_frame_unref(this->pFrame);
      }
  }
  this->setFrameFlag(true);
  return this->video_frame_count;
  // return 0;
}

void code_meta_data(){
  printf("\n");
  printf("***********************************************************************************************************************\n");
  printf("*          Tool : MV-Tractus [Mark-I]                                                                                 *\n");
  printf("*        Author : Jishnu Jaykumar Padalunkal (https://jishnujayakumar.github.io)                                      *\n");
  printf("*     Co-Author : Praneet Singh [Python-Boost] (https://github.com/praneet195)                                        *\n");
  printf("*     Used Libs : FFmpeg, OpenCV                                                                                      *\n");
  printf("*   Description : A simple tool [Compatible with Python] to extract motion vectors and frames from H264 video streams *\n");
  printf("***********************************************************************************************************************\n");
  printf("\n");
  // printf("--------------------------------------------------------------------------------------\n");
  // printf("framenum,source,blockw,blockh,srcx,srcy,dstx,dsty,motion_x,motion_y,motion_scale,flags\n");
  // printf("--------------------------------------------------------------------------------------\n");
}

int extract_motion_vectors(){
    // cout<<"line 139: extract_motion_vectors() called\n";
    int ret = 0;
    AVPacket pkt = { 0 };
    // cout<<"1"<<endl;
    if (!this->video_stream) {
        fprintf(stderr, "Could not find video stream in the input, aborting\n");
        ret = 1;
        // cout<<"2"<<endl;
        goto end;
    }

    // cout<<"3"<<endl;
    this->pFrame = av_frame_alloc();
    if (!this->pFrame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        // cout<<"4"<<endl;
        goto end;
    }

    // cout<<"5"<<endl;

    // Allocate an AVFrame structure
    this->pFrameRGB=av_frame_alloc();
    if(this->pFrameRGB==NULL)
      return -1;

      // Determine required buffer size and allocate buffer
      this->numBytes=avpicture_get_size(AV_PIX_FMT_BGR24, this->pCodecCtx->width,
                this->pCodecCtx->height);

      this->buffer=(uint8_t *)av_malloc(this->numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)this->pFrameRGB, this->buffer, AV_PIX_FMT_BGR24,
       this->pCodecCtx->width, this->pCodecCtx->height);
    // initialize SWS context for software scaling
    this->sws_ctx = sws_getContext(
           this->pCodecCtx->width,
           this->pCodecCtx->height,
           this->pCodecCtx->pix_fmt,
           this->pCodecCtx->width,
           this->pCodecCtx->height,
           AV_PIX_FMT_BGR24,
           SWS_BILINEAR,
           NULL,
           NULL,
           NULL
           );



    /* read frames from the file */
    while (av_read_frame(this->pFormatCtx, &pkt) >= 0) {

      // cout<<"6"<<endl;
        this->pkt = &pkt;
        if (this->pkt->stream_index == this->video_stream_idx){
              ret=this->decode_packet();
        }

        av_packet_unref(&pkt);
        av_packet_unref(this->pkt);
        if (ret < 0 || this->getFrameFlag()){
            av_freep(&(this->buffer));
            this->setFrameFlag(false);
            return this->video_frame_count;
            break;
          }
    }

    /* flush cached frames */
    // this->decode_packet(NULL);
    printf("\n--------------------------------------------------------------------------------------\n");

end:
    // cout<<"RET "<<endl;
    av_frame_free(&(this->pFrame));
    return ret < 0;
}

/* flush cached frames */
void flush_cached_frames(){
  this->pkt = NULL;
  this->decode_packet();
}

void freeStuff(){
  av_frame_free(&(this->pFrame));
  avcodec_free_context(&(this->pCodecCtx));
  avformat_close_input(&(this->pFormatCtx));
}

int get_current_frame_number(){
  return this->video_frame_count;
}

void increase_current_frame_number(){
  this->video_frame_count = this->video_frame_count%2500;
  this->video_frame_count++;
}

};

// Define C functions for the C++ class - as ctypes can only talk to C...


// extern "C"
// {
//     FMV* FMV_new(int argc, char **argv) {return new FMV(argc, argv[1]);}
//
//     const char* FMV_getSrcFilename(FMV* fmv) {return fmv->getSrcFilename();}
//
//     void FMV_setFrameFlag(FMV* fmv, bool flag){fmv->setFrameFlag(flag);}
//
//     bool FMV_getFrameFlag(FMV* fmv){return fmv->getFrameFlag();}
//
//     void FMV_connect(FMV* fmv, bool dump_av_format){fmv->connect(dump_av_format);}
//
//     int FMV_open_codec_context(FMV* fmv){return fmv->open_codec_context();}
//
//     int FMV_decode_packet(FMV* fmv){return fmv->decode_packet();}
//
//     int FMV_extract_motion_vectors(FMV* fmv){return fmv->extract_motion_vectors();}
//
//     /* flush cached frames */
//     void FMV_flush_cached_frames(FMV* fmv){fmv->flush_cached_frames();}
//
//     void FMV_freeStuff(FMV* fmv){fmv->freeStuff();}
//
//     int FMV_get_current_frame_number(FMV* fmv){return fmv->get_current_frame_number();}
//
//     void FMV_increase_current_frame_number(FMV* fmv){fmv->increase_current_frame_number();}
//
// }

FMV* createFMVobj(int argc, char *argv){
FMV* fmv = new FMV(argc, argv);
return fmv;

}

int main(int argc, char **argv){

  // FMV* fmv = createFMVobj(argc,argv[1]);
  FMV* fmv = new FMV(argc,argv[1]);
  fmv->connect(false);// Give true to dump_av_format
  int i = fmv->get_current_frame_number();
  int frame_num=0;
  cout<<"Total FPS:"<<fmv->getFPS()<<endl;
  while(frame_num==fmv->get_current_frame_number()){
    frame_num=fmv->extract_motion_vectors();
    fmv->getMAT();
    i=fmv->get_current_frame_number();

    /*Now printing the json object*/
    // printf ("%s\n",json_object_to_json_string(fmv->getFrameJson()));

    // cout<<"\t"<<fmv->getFPS()<<"|\t"<<fmv->getPseudoRealTimeStamp()<<endl;
    // cout<<"\t\t\t|\t"<<fmv->getPseudoRealTimeStamp();
  }
  // fmv->flush_cached_frames();
  fmv->freeStuff();
  return 0;
}


int process(int argc, char **argv){
// cout << "line 21: main() called\n";


// FMV* fmv = createFMVobj(argc,argv[1]);
FMV* fmv = new FMV(argc,argv[1]);
fmv->connect(false);// Give true to dump_av_format
int i = fmv->get_current_frame_number();
int frame_num=0;
cout<<"Total FPS:"<<fmv->getFPS()<<endl;
while(frame_num==fmv->get_current_frame_number()){
  frame_num=fmv->extract_motion_vectors();
  fmv->getMAT();
  i=fmv->get_current_frame_number();

  /*Now printing the json object*/
  // printf ("%s\n",json_object_to_json_string(fmv->getFrameJson()));

  // cout<<"\t"<<fmv->getFPS()<<"|\t"<<fmv->getPseudoRealTimeStamp()<<endl;
  // cout<<"\t\t\t|\t"<<fmv->getPseudoRealTimeStamp();
}
// fmv->flush_cached_frames();
fmv->freeStuff();
return 0;
}

//
BOOST_PYTHON_MODULE(efmvs)
{using namespace boost::python;

    // def("process", process);
    class_<FMV,FMV*>("FMV",init<int,char*>())
      .def("getSrcFilename",&FMV::getSrcFilename)
      .def("setFrameFlag",&FMV::setFrameFlag)
      .def("getFrameFlag",&FMV::getFrameFlag)
      .def("connect",&FMV::connect)
      .def("open_codec_context",&FMV::open_codec_context)
      .def("decode_packet",&FMV::decode_packet)
      .def("extract_motion_vectors",&FMV::extract_motion_vectors)
      .def("flush_cached_frames",&FMV::flush_cached_frames)
      .def("freeStuff",&FMV::freeStuff)
      .def("get_current_frame_number",&FMV::get_current_frame_number)
      .def("increase_current_frame_number",&FMV::increase_current_frame_number)
      .def("getMAT",&FMV::getMAT)
      // .def("getArray",&FMV::getArray)
      .def("getline",&FMV::getline)
      .def("getheight",&FMV::getheight)
      .def("getwidth",&FMV::getwidth)
      // .def("getFrameJson", &FMV::getFrameJson)
      .def("getFPS", &FMV::getFPS)
      .def("getPseudoTimeStamp", &FMV::getPseudoTimeStamp)
      .def("getPseudoRealTimeStamp", &FMV::getPseudoRealTimeStamp);

      //using namespace XM;

      //expose module-level functions


}
