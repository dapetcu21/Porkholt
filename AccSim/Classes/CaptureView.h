//
//  CaptureView.h
//  AccSim
//
//  Created by Marius Petcu on 3/23/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TouchController;
@interface CaptureView : UIView {
	TouchController * parent;
}

@property(nonatomic,assign) IBOutlet TouchController * parent;

@end
