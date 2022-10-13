//
//  CamraViewController.m
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/10/10.
//

#import "CamraViewController.h"
#import "OpenPanel.h"
#import "cameraSources/CameraSource.h"

@interface CamraViewController ()<CameraSourceDelegate>
{
    FILE *_saveFile;
}
@property(copy,nonatomic)NSString *save_path;
@property(strong,nonatomic)CameraSource *cameraSource;
@end

@implementation CamraViewController

- (void)windowDidLoad {
    [super windowDidLoad];
    
}

-(void)readyWriteFileWithAddress:(NSString *)address{
    _saveFile = fopen(address.UTF8String, "wb+");
}

- (IBAction)buttonClick:(NSButton *)sender {
    if ([sender.title isEqualToString:@"选择保存地址"]) {
        __weak typeof(self)weakSelf  = self;
        [OpenPanel openPanelWithTitleMessage:@"选择录制后的视频的保存路径" andWithCanChoseDic:YES andWithSuccessBlock:^(NSString * _Nonnull address) {
            weakSelf.save_path = address;
            [self readyWriteFileWithAddress:[address stringByAppendingPathComponent:@"/test.data"]];
        } andWithWindow:self.window];
        
    }else if ([sender.title isEqualToString:@"开始录制"]){
        if (!self.save_path || self.save_path.length<0) {
            NSAlert *alert = [[NSAlert alloc]init];
            alert.messageText = @"请选择一个文件来存放录制完成的视频";
            [alert addButtonWithTitle:@"确定"];
            [alert beginSheetModalForWindow:self.window completionHandler:NULL];
            return;
        }
    
        [self startRecord];

        
        
    }else{
     
        [_cameraSource stopRecord];
        
        fclose(_saveFile);
        
    }
    

}



-(void)startRecord{
    CameraSource *cameraSource = [[CameraSource alloc]initWithRecordingParameter:[RecordingParameter new] andWithDelegate:self];
    _cameraSource = cameraSource;
    [cameraSource startRecord];
}


-(void)didReciveSampleBuffer:(CMSampleBufferRef)sampleBufferRef andWithType:(CameraSourceBufferType)type{
    if (type == CameraSourceBufferType_video) {
        NSLog(@"视频");
        CVPixelBufferRef ref = CMSampleBufferGetImageBuffer(sampleBufferRef);
        NSData *data = [self dataFrompixelBuffer:ref];
        NSLog(@"%@",data);
        size_t len =   fwrite(data.bytes, 1, sizeof(data), _saveFile);
        NSImage *image = [[NSImage alloc]initWithData:data];
        NSLog(@"%@",image);
        if (len > 0) {
            
            NSLog(@"写入成功");
            
            return;
            
        }
        
    }else{
        NSLog(@"音频");
    }
    
}


-(NSData *)dataFrompixelBuffer:(CVPixelBufferRef)pixelBuffer {
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    size_t w = CVPixelBufferGetWidth(pixelBuffer);
    size_t h = CVPixelBufferGetHeight(pixelBuffer);
    
    size_t y_size = w * h;
    size_t uv_size = y_size/2;
    
    uint8_t *yuv_frame = (uint8_t *)malloc(y_size + uv_size);
    uint8_t *y_frame = (uint8_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    memcpy(yuv_frame, y_frame, y_size);
    uint8_t *uv_frame = (uint8_t *)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
    memcpy(yuv_frame + y_size, uv_frame, uv_size);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    NSData *data = [NSData dataWithBytesNoCopy:yuv_frame length:y_size + uv_size];
    return data;
}


@end
