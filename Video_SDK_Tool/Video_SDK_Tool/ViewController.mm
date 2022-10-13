//
//  ViewController.m
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/9/28.
//

#import "ViewController.h"
#include "get_video_infos.hpp"
#import "OpenPanel.h"
#import "CamraViewController.h"

@interface ViewController ()
@property(strong,nonatomic) CamraViewController *cameraController;
@end


@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    
}

- (IBAction)choseFileBtnClickForffpby:(NSButton *)sender {
    
    [OpenPanel openPanelWithTitleMessage:@"请选择一个视频文件进行分析" andWithCanChoseDic:NO andWithSuccessBlock:^(NSString * _Nonnull address) {
        NSLog(@"您选择的地址是:%@",address);
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            aft_video_infos::get_video_infos([address UTF8String]);
        });
        
    } andWithWindow:self.view.window];
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

}



/// 采集视频
- (IBAction)cameraSources:(NSButton *)sender {
    [self.cameraController.window makeKeyAndOrderFront:nil];
}

-(CamraViewController *)cameraController{
    if (!_cameraController) {
        _cameraController = [[CamraViewController alloc]initWithWindowNibName:@"CamraViewController"];
    }
    return _cameraController;
}



@end
