//
//  PLImageView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/10/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLImageView.h"
#import "PLImage.h"
#import "PLObjectView.h"
#include "PHImage.h"
#import "SubentityController.h"
#import "ObjectController.h"
#import "PLObject.h"
#import "PHFileManager.h"

PLImageView::PLImageView(PLImage * _model) : model(_model)
{
    [model retain];
    [model setActor:this];
    modelChanged();
}

PLImageView::~PLImageView()
{
    [model setActor:NULL];
    [model release];
}

void PLImageView::modelChanged()
{
    NSRect frame = [model frame];
    setFrame(PHRect(frame.origin.x,frame.origin.y,frame.size.width,frame.size.height));
    string path = string([[model fileName] UTF8String]);
    if (path[0]=='/')
        path = PHFileManager::resourcePath()+"/img/"+path;
    else
        path = string([[[[((ObjectController*)(((SubentityController*)(model.owner)).object.owner)) fileURL] URLByAppendingPathComponent:[model fileName]] path] UTF8String]);
    setImage(PHImage::imageFromPath(path));
    setRotation(-toRad(model.rotation));
    NSRect portion = model.portion;
    setTextureCoordinates(PHRect(portion.origin.x,portion.origin.y,portion.size.width,portion.size.height));
    setHorizontallyFlipped(model.horizontallyFlipped);
    setVerticallyFlipped(model.verticallyFlipped);
    PLColor c = model.tint;
    setTintColor(PHColor(c.r,c.g,c.b,c.a));
    setAlpha(model.alpha);
}

void PLImageView::draw()
{
    PHImageView::draw();
}