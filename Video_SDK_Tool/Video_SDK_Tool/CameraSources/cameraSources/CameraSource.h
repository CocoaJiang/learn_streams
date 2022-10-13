//
//  CameraSource.h
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/10/10.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger,CameraSourceBufferType){
    CameraSourceBufferType_video = 0,
    CameraSourceBufferType_audio = 1
};


@protocol CameraSourceDelegate <NSObject>


///  采集完成的数据的回调
/// @param sampleBufferRef 数据的样本->buffer
/// @param type 音频数据还是视频数据。
-(void)didReciveSampleBuffer:(CMSampleBufferRef)sampleBufferRef andWithType:(CameraSourceBufferType)type;


@end


@interface RecordingParameter:NSObject




@end



/// 这个类管理录制。录制的结果以代理的方式回调到控制器
@interface CameraSource : NSObject


/// 初始化参数
/// @param recordingParameter 录制的参数
/// @param delegate 录制的代理，主要用来回调录制完成的数据
-(instancetype)initWithRecordingParameter:(RecordingParameter *)recordingParameter andWithDelegate:(id<CameraSourceDelegate>)delegate;


/// 开始录制
-(void)startRecord;



/// 结束录制
-(void)stopRecord;



@end

NS_ASSUME_NONNULL_END
