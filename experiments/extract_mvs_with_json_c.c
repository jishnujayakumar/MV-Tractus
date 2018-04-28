/*
 * Copyright (c) 2012 Stefano Sabatini
 * Copyright (c) 2014 Clément Bœsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "include/libavutil/motion_vector.h"
#include "include/libavformat/avformat.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;
static AVStream *video_stream = NULL;
static const char *src_filename = NULL;

static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static int video_frame_count = 0;

static int print_motion_vectors_data(AVMotionVector *mv, int video_frame_count){
  printf("| #:%d | p/f:%2d | %2d x %2d | src:(%4d,%4d) | dst:(%4d,%4d) | dx:%4d | dy:%4d | motion_x:%4d | motion_y:%4d | motion_scale:%4d | 0x%"PRIx64" |\n",
      video_frame_count,
      mv->source,
      mv->w,
      mv->h,
      mv->src_x,
      mv->src_y,
      mv->dst_x,
      mv->dst_y,
      mv->dst_x - mv->src_x,
      mv->dst_y - mv->src_y,
      mv->motion_x,
      mv->motion_y,
      mv->motion_scale,
      mv->flags);
  printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
  return 0;
}

static int print_frame_data(AVFrame * frame){
  printf("%s\n", frame->data[0]);
  return 0;
}

static int dump_json_to_file(char *filepath, int video_frame_count, char* json_data){
   FILE *fp;
   fp = fopen(filepath, "w+");
   fprintf(fp, "This is testing for fprintf..\n");
   fputs(json_data, fp);
   fclose(fp);
}

static int decode_packet(const AVPacket *pkt)
{
    int ret = avcodec_send_packet(video_dec_ctx, pkt);
    /*Creating json objects*/
    json_object *macroblock_data = NULL;
    /*Creating json arrays*/
    json_object *frame_mv_data = NULL;
    json_object *frames_json_array = NULL;

    /*Creating json integers for storing mv data*/
    json_object *source;
    json_object *macroblock_width;
    json_object *macroblock_height;
    json_object *src_x;
    json_object *src_y;
    json_object *dst_x;
    json_object *dst_y;
    json_object *dx;
    json_object *dy;
    json_object *jstring3 = json_object_new_string("php");

    if (ret < 0) {
        fprintf(stderr, "Error while sending a packet to the decoder: %s\n", av_err2str(ret));
        return ret;
    }

    while (ret >= 0)  {
        ret = avcodec_receive_frame(video_dec_ctx, frame);
        frame_mv_data = json_object_new_array();
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            fprintf(stderr, "Error while receiving a frame from the decoder: %s\n", av_err2str(ret));
            return ret;
        }

        if (ret >= 0) {
            int i;
            AVFrameSideData *sd;
            printf("Hello%d\n",video_frame_count);
            video_frame_count++;
            sd = av_frame_get_side_data(frame, AV_FRAME_DATA_MOTION_VECTORS);
            if (sd) {
                const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
                for (i = 0; i < sd->size / sizeof(*mvs); i++) {
                    macroblock_data = json_object_new_object();
                    const AVMotionVector *mv = &mvs[i];
                    source = json_object_new_int(mv->source);
                    macroblock_width = json_object_new_int(mv->w);
                    macroblock_height = json_object_new_int(mv->h);
                    src_x = json_object_new_int(mv->src_x);
                    src_y = json_object_new_int(mv->src_y);
                    dst_x = json_object_new_int(mv->dst_x);
                    dst_y = json_object_new_int(mv->dst_y);
                    dx = json_object_new_int(mv->dst_x - mv->src_x);
                    dy = json_object_new_int(mv->dst_y - mv->src_y);
                    /*Form the json objects*/
                    json_object_object_add(macroblock_data,"source", source);
                    json_object_object_add(macroblock_data,"width", macroblock_width);
                    json_object_object_add(macroblock_data,"height", macroblock_height);
                    json_object_object_add(macroblock_data,"src_x", src_x);
                    json_object_object_add(macroblock_data,"src_y", src_y);
                    json_object_object_add(macroblock_data,"dst_x", dst_x);
                    json_object_object_add(macroblock_data,"dst_y", dst_y);
                    json_object_object_add(macroblock_data,"dx", dx);
                    json_object_object_add(macroblock_data,"dy", dy);
                    /*Adding the above created json object to the array*/
                    json_object_array_add(frame_mv_data,macroblock_data);
                    macroblock_data=NULL;
                    /*Now printing the json object*/
                    // printf ("The json object created: %s\n",json_object_to_json_string(macroblock_data));
                    // printf ("\nThe json object created: %s\n",json_object_to_json_string(frame_mv_data));
                    // print_motion_vectors_data(mv, video_frame_count);
                     int arraylen = json_object_array_length(frame_mv_data);
                     printf("%d - %d\n\n", video_frame_count, arraylen);
                }

                // printf("%s\n", strcat("./output/", strcat(video_frame_count, ".json")));

                // dump_json_to_file(strcat("./output/", strcat(video_frame_count, ".json")), video_frame_count, frame_mv_data);
            }
            frame_mv_data=NULL;

            /*Adding the above created json array to the output array*/
            // json_object_array_add(frames_json_array, frame_mv_data);

            //Print frame data
            // print_frame_data(frame);
            av_frame_unref(frame);
        }

        // printf ("The json object created: %s\n",json_object_to_json_string(frames_json_array));
    }

    return 0;
}

static int open_codec_context(AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, &dec, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    } else {
        int stream_idx = ret;
        st = fmt_ctx->streams[stream_idx];

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

        video_stream_idx = stream_idx;
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = dec_ctx;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    AVPacket pkt = { 0 };

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <video>\n", argv[0]);
        exit(1);
    }
    src_filename = argv[1];

    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        exit(1);
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    open_codec_context(fmt_ctx, AVMEDIA_TYPE_VIDEO);

    av_dump_format(fmt_ctx, 0, src_filename, 0);

    if (!video_stream) {
        fprintf(stderr, "Could not find video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    printf("framenum,source,blockw,blockh,srcx,srcy,dstx,dsty,motion_x,motion_y,motion_scale,flags\n");

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_idx)
            ret = decode_packet(&pkt);
        av_packet_unref(&pkt);
        if (ret < 0)
            break;
    }

    /* flush cached frames */
    decode_packet(NULL);

end:
    avcodec_free_context(&video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    return ret < 0;
}
