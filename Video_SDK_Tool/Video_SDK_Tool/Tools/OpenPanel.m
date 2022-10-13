//
//  OpenPanel.m
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/9/29.
//

#import "OpenPanel.h"

@implementation OpenPanel
+(void)openPanelWithTitleMessage:(NSString *)message andWithCanChoseDic:(BOOL)canChoseDic andWithSuccessBlock:(void (^)(NSString * _Nonnull))choseFinshedBlock andWithWindow:(NSWindow *)window{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setPrompt:@"选择"];     // 设置默认选中按钮的显示（OK 、打开，Open ...）
    [panel setCanChooseDirectories:canChoseDic];
    [panel setAllowedFileTypes : @[@"mp4",@"mov"]];
    [panel beginSheetModalForWindow:window completionHandler:^(NSModalResponse result) {
        if (result == NSModalResponseOK && choseFinshedBlock) {
            choseFinshedBlock(panel.URL.path);
        }
    }];
    
}
@end
