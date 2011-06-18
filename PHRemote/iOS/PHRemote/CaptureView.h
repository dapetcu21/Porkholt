//
//  CaptureView.h
//  PHRemote
//
//  Created by Marius Petcu on 6/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MainViewController;
@interface CaptureView : UIView {
    MainViewController * delegate;
}

@property(nonatomic,assign) IBOutlet MainViewController * delegate;

@end
