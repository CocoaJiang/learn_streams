//
//  CameraSource.m
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/10/10.
//

#import "CameraSource.h"
#import <AVFoundation/AVFoundation.h>

@implementation RecordingParameter
@end


@interface CameraSource ()<AVCaptureVideoDataOutputSampleBufferDelegate,AVCaptureAudioDataOutputSampleBufferDelegate>


/// 外部数据部分
@property(strong,nonatomic)RecordingParameter *recordingParameter;
@property(weak,nonatomic)id<CameraSourceDelegate> delegate;

/// 内部相关业务部分
@property(strong,nonatomic)AVCaptureSession *captureSession;
@property(strong,nonatomic)dispatch_queue_t sessionQueue;

/// video
@property(strong,nonatomic)AVCaptureDeviceInput* videoCaptureDevices;
@property(strong,nonatomic)AVCaptureVideoDataOutput *videoDataOutput;
@property(strong,nonatomic)AVCaptureConnection *videoCaptureConnection;

/// andio
@property(strong,nonatomic)AVCaptureDeviceInput *audioCaptureDevices;
@property(strong,nonatomic)AVCaptureAudioDataOutput *captureAudioDataOutput;
@property(strong,nonatomic)AVCaptureConnection *audioCaptureConnection;



@end


@implementation CameraSource

static inline void ERROR(NSString *error){
    NSLog(@"ERROR = %@",error);
}


-(instancetype)initWithRecordingParameter:(RecordingParameter *)recordingParameter andWithDelegate:(id<CameraSourceDelegate>)delegate{
    if (self = [super init]) {
        _delegate = delegate;
        _recordingParameter = recordingParameter;
        [self readyRecord];
    }
    return self;
}



/// 准备视频设备
-(void)readyForCamera{
    
    /// 第一步，获取设备
    AVCaptureDevice *device;
    if (@available(macOS 10.15, *)) {
         device = [AVCaptureDevice defaultDeviceWithDeviceType:AVCaptureDeviceTypeBuiltInWideAngleCamera mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionFront];
    } else {
        device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    }
    
    NSError *error = nil;
    AVCaptureDeviceInput *device_fornt = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
    _videoCaptureDevices = device_fornt;
    if (error) {
        ERROR(error.localizedRecoverySuggestion);
        return;
    }
    if (!device) {
        ERROR(@"AVCaptureDeviceInput_camera 获取失败");
        return;
    }
    
    
    /// 第二步，获取输出设备
     _videoDataOutput = [[AVCaptureVideoDataOutput alloc]init];
    [self.videoDataOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    [self.videoDataOutput setSampleBufferDelegate:self queue:self.sessionQueue];
    
    
    
    /// 第三步  添加输入设备和输出设备
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddInput:_videoCaptureDevices]) {
        [self.captureSession addInput:_videoCaptureDevices];
    }
    if ([self.captureSession canAddOutput:_videoDataOutput]) {
        [self.captureSession addOutput:_videoDataOutput];
    }
    [self.captureSession commitConfiguration];

    
    
    /// 第四步 连接输入设备和输出设备
    _videoCaptureConnection = [_videoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddConnection:_videoCaptureConnection]) {
        [self.captureSession addConnection:_videoCaptureConnection];
    }
    [self.captureSession commitConfiguration];
    
    
    /// 经过以上步骤就可以采集视频数据了，采集的结果以CMSapleBuffer 回调到此类。
    /// 如果您想对于采集的视频设置一些参数，比如说采集的分辨率，那么就要进行下面的设置 包括但不限于分辨率/帧率/视频格式/颜色空间等
    
    [self.captureSession beginConfiguration];
    if ([self.captureSession canSetSessionPreset:AVCaptureSessionPreset3840x2160]) {
        [self.captureSession setSessionPreset:AVCaptureSessionPreset3840x2160];
    }else if ([self.captureSession canSetSessionPreset:AVCaptureSessionPreset1920x1080]){
        [self.captureSession setSessionPreset:AVCaptureSessionPreset1920x1080];
    }else if ([self.captureSession canSetSessionPreset:AVCaptureSessionPresetHigh]){
        [self.captureSession setSessionPreset:AVCaptureSessionPresetHigh];
    }else{
        [self.captureSession setSessionPreset:AVCaptureSessionPreset1280x720];
    }
    [self.captureSession commitConfiguration];

}


/// 准备音频设备
-(void)readyForMicPhone{
    
    
    /// 第一步，获取音频设备
    NSError *error = nil;
    AVCaptureDevice *device_micphone = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    _audioCaptureDevices = [AVCaptureDeviceInput deviceInputWithDevice:device_micphone error:&error];
    if (error) {
        ERROR(error.localizedRecoverySuggestion);
        return;
    }
    if (!device_micphone) {
        ERROR(@"AVCaptureDeviceInput_micphone 获取失败");
        return;
    }
    
    /// 第二步，初始化音频输出data
    _captureAudioDataOutput  = [[AVCaptureAudioDataOutput alloc]init];
    [_captureAudioDataOutput setAudioSettings:nil];
    [_captureAudioDataOutput setSampleBufferDelegate:self queue:self.sessionQueue];
    
    
    /// 第三步，添加输入设备和添加输入出设备
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddInput:self.audioCaptureDevices]) {
        [self.captureSession addInput:self.audioCaptureDevices];
    }
    if ([self.captureSession canAddOutput:self.captureAudioDataOutput]) {
        [self.captureSession addOutput:self.captureAudioDataOutput];
    }
    [self.captureSession commitConfiguration];
    
    
    /// 第四步，添加连接器
    self.audioCaptureConnection = [self.videoDataOutput connectionWithMediaType:AVMediaTypeAudio];
    [self.captureSession beginConfiguration];
    if ([self.captureSession canAddConnection:self.audioCaptureConnection]) {
        [self.captureSession addConnection:self.audioCaptureConnection];
    }
    [self.captureSession commitConfiguration];
}


-(void)readyRecord{
    _captureSession = [[AVCaptureSession alloc]init];
    _sessionQueue = dispatch_queue_create("videoDataOutput_QUEUE", NULL);
    [self readyForCamera];
    [self readyForMicPhone];
}

#pragma mark - delegate_audio & video

-(void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    if (self.delegate && [self.delegate respondsToSelector:@selector(didReciveSampleBuffer:andWithType:)]) {
        [self.delegate didReciveSampleBuffer:sampleBuffer andWithType:connection == self.videoCaptureConnection ? 0 : 1];
    }
}


-(void)startRecord{
    if (!self.captureSession.isRunning) {
        [self.captureSession startRunning];
    }else{
        return;;
    }
}

-(void)stopRecord{
    if (self.captureSession.isRunning) {
        [self.captureSession stopRunning];
    }else{
        return;;
    }
}



@end
