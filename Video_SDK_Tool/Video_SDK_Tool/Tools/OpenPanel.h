//
//  OpenPanel.h
//  Video_SDK_Tool
//
//  Created by 阿凡提 on 2022/9/29.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OpenPanel : NSObject
+(void)openPanelWithTitleMessage:(NSString *)message andWithCanChoseDic:(BOOL)canChoseDic andWithSuccessBlock:(void(^)(NSString *address))choseFinshedBlock andWithWindow:(NSWindow *)window;
@end

NS_ASSUME_NONNULL_END
