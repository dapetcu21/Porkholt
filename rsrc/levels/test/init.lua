objects = {};
objects.n = 0;

function addObject(o)
    objects[objects.n] = o;
    objects.n = objects.n + 1;
end

obj = {};
obj.class = "PHLObject";
obj.posX = 3;
obj.posY = 0.25;
obj.images = {};
obj.images.n = 3;
dubious = 2.371604938271605;
obj.images[0] = {
    filename = "ground.png";
    posX = -3;
    posY = -0.25;
    imgW = dubious;
    imgH = 0.5;
};
obj.images[1] = {
    filename = "ground.png";
    posX = -3+dubious;
    posY = -0.25;
    imgW = dubious;
    imgH = 0.5;
};
obj.images[2] = {
    filename = "ground.png";
    posX = -3+2*dubious;
    posY = -0.25;
    imgW = dubious;
    imgH = 0.5;
};
obj.physics = {
    n = 1;
}
obj.physics[0] = {
    shape = "box";
    boxH = 0.5;
    boxW = 3*dubious;
    boxX = -1.5*dubious;
    boxY = -0.25;
}
addObject(obj);

obj = {};
obj.class = "PHLPlayer";
obj.physics = {
    dynamic = 1;
    n = 1;
}
obj.physics[0] = {
    shape = "circle";
    circleR = 0.25;
    restitution = 0.5;
}
obj.posX = 1.5;
obj.posY = 2.75;
obj.maxVelocityX = 3;
obj.images = {};
obj.images.n = 1;
obj.images[0] = {
    filename = "ball.png";
    posX = -0.25;
    posY = -0.25;
    imgW = 0.5;
    imgH = 0.5;
}
addObject(obj);

obj = {};
obj.class = "PHLObject";
obj.posX = 4;
obj.posY = 2;
obj.rotation = 30;
obj.physics = {
    dynamic = 1;
    n = 1;
}
obj.physics[0] = {
    shape = "box";
    friction = 0.3;
    boxW = 1;
    boxH = 1;
    boxX = -0.5;
    boxY = -0.5;
    density = 0.1;
}
obj.images = {
    n = 1;
}
obj.images[0] = {
    filename = "box.png";
    posX = -0.5;
    posY = -0.5;
    imgW = 1;
    imgH = 1;
}
addObject(obj);


obj = {};
obj.class = "PHLCamera";
obj.posX = 3;
obj.posY = 1.75;
obj.camH = 3.5;
addObject(obj);

obj = {};
obj.class = "PHLObject";
obj.posX = 0;
obj.posY = 0;
obj.physics = {
    n = 1;
}
obj.physics[0] = {
    shape = "box";
    boxH = 10;
    boxW = 0.1;
    boxX = 0;
    boxY = 0;
}
addObject(obj);
obj = {};
obj.class = "PHLObject";
obj.posX = 3*dubious;
obj.posY = 0;
obj.physics = {
    n = 1;
}
obj.physics[0] = {
    shape = "box";
    boxH = 10;
    boxW = 0.1;
    boxX = 0;
    boxY = 0;
}
addObject(obj);
