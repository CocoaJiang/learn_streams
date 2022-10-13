//
//  get_video_infos.cpp
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/9/28.
//

#include "get_video_infos.hpp"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/bprint.h"
#include "libavutil/display.h"
#include "libavutil/hash.h"
#include "libavutil/mastering_display_metadata.h"
#include "libavutil/dovi_meta.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/spherical.h"
#include "libavutil/stereo3d.h"
#include "libavutil/dict.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/timecode.h"
#include "libavutil/timestamp.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libpostproc/postprocess.h"
}

void ERROR_STR(string error){
    cout << "发生错误_错误信息是:" << error << endl;
}



static inline void ERROR(string error){
    cout << "ERROR is :" << error << endl;
}



static void show_stream(AVFormatContext *format){
 
    
    
    
}



/// 获取一个视频的相关信息_流信息和_foramat信息
/// @param file_path 所存的文件的地址
void aft_video_infos::get_video_infos(const char *file_path){
    
    int ret = 0;
    av_register_all();
    
    AVFormatContext *format_ctx = avformat_alloc_context();
    if (!format_ctx) {
        ERROR("format_ctx _init_failed");
        return;
    }
    
    ret  = avformat_open_input(&format_ctx, file_path, NULL, NULL);
    if (ret != 0) {
        ERROR("文件访问失败，请检查具体路径");
        return;
    }
    
    AVInputFormat *input_fmt = format_ctx->iformat;
    if (!input_fmt) {
        ERROR("input_fmt _get_failed");
        return;
    }
    
    
    /// format
    cout << "filename :" << format_ctx->url << endl;
    cout << "nb_streams :" << format_ctx ->nb_streams << endl;
    cout << "nb_programs :" << format_ctx ->nb_programs << endl;
    cout << "start_time :" << to_string(format_ctx -> start_time > 1 ? : 0) << endl;
    cout << "duration :" << format_ctx -> duration << endl;
    cout << "probe_score :" << format_ctx ->probe_score<< endl;
    int64_t size = format_ctx->pb ? avio_size(format_ctx->pb) : -1;
    if (size >= 0){
        cout << "size :" << size << endl;
    }else{
        cout << "size :" << "N/V" << endl;
    }
    cout << "bit_rate :" << format_ctx -> bit_rate << endl;
    
    if (!format_ctx->metadata) {
        ERROR("metadata info not to found");
    }
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(format_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        if (tag->key) {
            cout << tag -> key  << " :"  << tag -> value << endl;
        }else{
            break;
        }
    }
    
    /// streeams
    for (int i = 0; i< format_ctx->nb_streams; i++) {
        AVStream *stream = format_ctx->streams[i];
        if (!stream) {
            break;
        }
        
        AVCodec *codec;
        AVCodecContext *dec_ctx;
        AVCodecParameters *par;
        AVRational sar, dar;
        AVBPrint bprint;
        const AVCodecDescriptor *cd;
        char val_str[128];
        const char *s;
        const char *profile = NULL;
        av_bprint_init(&bprint, 1, AV_BPRINT_SIZE_UNLIMITED);
        cout << "index : " << stream->index << endl;
        par = stream->codecpar;
        codec = avcodec_find_decoder(par->codec_id);
        if (!codec) {
            ERROR("codec _init_error");
            continue;
        }
        dec_ctx =  avcodec_alloc_context3(codec);
        if (!dec_ctx) {
            ERROR("dec_ctx _init_error");
            continue;;
        }
        cd = avcodec_descriptor_get(par->codec_id);
        if (cd) {
            cout << "codec_name :" << cd->name << endl;
            cout << "codec_long_name :" << cd->long_name << endl;
        }
        s = av_get_media_type_string(par->codec_type);
        if (s) {
            cout << "codec_type :" << s << endl;
        }
        AVBPrint buf;
        av_bprint_init(&buf, 0, AV_BPRINT_SIZE_AUTOMATIC);
        av_bprintf(&buf, "%d%s%d", dec_ctx->time_base.num, "/", dec_ctx->time_base.den);
        cout << "codec_time_base :" << buf.str << endl;
        if (dec_ctx) {
            cout << "coded_width : " << dec_ctx->coded_width << endl;
            cout << "coded_height : " << dec_ctx->coded_height << endl;
            cout << "closed_captions : " << !!(dec_ctx->properties & FF_CODEC_PROPERTY_CLOSED_CAPTIONS) << endl;
        }
       // s = av_get_pix_fmt_name(par->format);
        
        
        
    }
    

    /*
             s = av_get_pix_fmt_name(par->format);
             if (s) print_str    ("pix_fmt", s);
             else   print_str_opt("pix_fmt", "unknown");
             print_int("level",   par->level);

             print_color_range(w, par->color_range);
             print_color_space(w, par->color_space);
             print_color_trc(w, par->color_trc);
             print_primaries(w, par->color_primaries);
             print_chroma_location(w, par->chroma_location);

             if (par->field_order == AV_FIELD_PROGRESSIVE)
                 print_str("field_order", "progressive");
             else if (par->field_order == AV_FIELD_TT)
                 print_str("field_order", "tt");
             else if (par->field_order == AV_FIELD_BB)
                 print_str("field_order", "bb");
             else if (par->field_order == AV_FIELD_TB)
                 print_str("field_order", "tb");
             else if (par->field_order == AV_FIELD_BT)
                 print_str("field_order", "bt");
             else
                 print_str_opt("field_order", "unknown");

     #if FF_API_PRIVATE_OPT
             if (dec_ctx && dec_ctx->timecode_frame_start >= 0) {
                 char tcbuf[AV_TIMECODE_STR_SIZE];
                 av_timecode_make_mpeg_tc_string(tcbuf, dec_ctx->timecode_frame_start);
                 print_str("timecode", tcbuf);
             } else {
                 print_str_opt("timecode", "N/A");
             }
     #endif
             if (dec_ctx)
                 print_int("refs", dec_ctx->refs);
             break;

         case AVMEDIA_TYPE_AUDIO:
             s = av_get_sample_fmt_name(par->format);
             if (s) print_str    ("sample_fmt", s);
             else   print_str_opt("sample_fmt", "unknown");
             print_val("sample_rate",     par->sample_rate, unit_hertz_str);
             print_int("channels",        par->channels);

             if (par->channel_layout) {
                 av_bprint_clear(&pbuf);
                 av_bprint_channel_layout(&pbuf, par->channels, par->channel_layout);
                 print_str    ("channel_layout", pbuf.str);
             } else {
                 print_str_opt("channel_layout", "unknown");
             }

             print_int("bits_per_sample", av_get_bits_per_sample(par->codec_id));
             break;

         case AVMEDIA_TYPE_SUBTITLE:
             if (par->width)
                 print_int("width",       par->width);
             else
                 print_str_opt("width",   "N/A");
             if (par->height)
                 print_int("height",      par->height);
             else
                 print_str_opt("height",  "N/A");
             break;
         }

         if (dec_ctx && dec_ctx->codec && dec_ctx->codec->priv_class && show_private_data) {
             const AVOption *opt = NULL;
             while (opt = av_opt_next(dec_ctx->priv_data,opt)) {
                 uint8_t *str;
                 if (opt->flags) continue;
                 if (av_opt_get(dec_ctx->priv_data, opt->name, 0, &str) >= 0) {
                     print_str(opt->name, str);
                     av_free(str);
                 }
             }
         }

         if (fmt_ctx->iformat->flags & AVFMT_SHOW_IDS) print_fmt    ("id", "0x%x", stream->id);
         else                                          print_str_opt("id", "N/A");
         print_q("r_frame_rate",   stream->r_frame_rate,   '/');
         print_q("avg_frame_rate", stream->avg_frame_rate, '/');
         print_q("time_base",      stream->time_base,      '/');
         print_ts  ("start_pts",   stream->start_time);
         print_time("start_time",  stream->start_time, &stream->time_base);
         print_ts  ("duration_ts", stream->duration);
         print_time("duration",    stream->duration, &stream->time_base);
         if (par->bit_rate > 0)     print_val    ("bit_rate", par->bit_rate, unit_bit_per_second_str);
         else                       print_str_opt("bit_rate", "N/A");
     #if FF_API_LAVF_AVCTX
         if (stream->codec->rc_max_rate > 0) print_val ("max_bit_rate", stream->codec->rc_max_rate, unit_bit_per_second_str);
         else                                print_str_opt("max_bit_rate", "N/A");
     #endif
         if (dec_ctx && dec_ctx->bits_per_raw_sample > 0) print_fmt("bits_per_raw_sample", "%d", dec_ctx->bits_per_raw_sample);
         else                                             print_str_opt("bits_per_raw_sample", "N/A");
         if (stream->nb_frames) print_fmt    ("nb_frames", "%"PRId64, stream->nb_frames);
         else                   print_str_opt("nb_frames", "N/A");
         if (nb_streams_frames[stream_idx])  print_fmt    ("nb_read_frames", "%"PRIu64, nb_streams_frames[stream_idx]);
         else                                print_str_opt("nb_read_frames", "N/A");
         if (nb_streams_packets[stream_idx]) print_fmt    ("nb_read_packets", "%"PRIu64, nb_streams_packets[stream_idx]);
         else                                print_str_opt("nb_read_packets", "N/A");
         if (do_show_data)
             writer_print_data(w, "extradata", par->extradata,
                                               par->extradata_size);
         writer_print_data_hash(w, "extradata_hash", par->extradata,
                                                     par->extradata_size);

     #define PRINT_DISPOSITION(flagname, name) do {                                \
             print_int(name, !!(stream->disposition & AV_DISPOSITION_##flagname)); \
         } while (0)

         if (do_show_stream_disposition) {
             writer_print_section_header(w, in_program ? SECTION_ID_PROGRAM_STREAM_DISPOSITION : SECTION_ID_STREAM_DISPOSITION);
             PRINT_DISPOSITION(DEFAULT,          "default");
             PRINT_DISPOSITION(DUB,              "dub");
             PRINT_DISPOSITION(ORIGINAL,         "original");
             PRINT_DISPOSITION(COMMENT,          "comment");
             PRINT_DISPOSITION(LYRICS,           "lyrics");
             PRINT_DISPOSITION(KARAOKE,          "karaoke");
             PRINT_DISPOSITION(FORCED,           "forced");
             PRINT_DISPOSITION(HEARING_IMPAIRED, "hearing_impaired");
             PRINT_DISPOSITION(VISUAL_IMPAIRED,  "visual_impaired");
             PRINT_DISPOSITION(CLEAN_EFFECTS,    "clean_effects");
             PRINT_DISPOSITION(ATTACHED_PIC,     "attached_pic");
             PRINT_DISPOSITION(TIMED_THUMBNAILS, "timed_thumbnails");
             writer_print_section_footer(w);
         }

         if (do_show_stream_tags)
             ret = show_tags(w, stream->metadata, in_program ? SECTION_ID_PROGRAM_STREAM_TAGS : SECTION_ID_STREAM_TAGS);

         if (stream->nb_side_data) {
             print_pkt_side_data(w, stream->codecpar, stream->side_data, stream->nb_side_data,
                                 SECTION_ID_STREAM_SIDE_DATA_LIST,
                                 SECTION_ID_STREAM_SIDE_DATA);
         }

         writer_print_section_footer(w);
         av_bprint_finalize(&pbuf, NULL);
         fflush(stdout);

         return ret;
     }

     
     */
    
    
    
    
    
    
    avformat_close_input(&format_ctx);
    
    
}







