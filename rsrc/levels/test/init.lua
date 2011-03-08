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
obj.images[0] = {
    filename = "ground.png";
    posX = -3;
    posY = -0.25;
    imgW = 2.371604938271605;
    imgH = 0.5;
};
obj.images[1] = {
    filename = "ground.png";
    posX = -0.628395061728395;
    posY = -0.25;
    imgW = 2.371604938271605;
    imgH = 0.5;
};
obj.images[2] = {
    filename = "ground.png";
    posX = 1.74320987654321;
    posY = -0.25;
    imgW = 2.371604938271605;
    imgH = 0.5;
};
addObject(obj);

obj = {};
obj.class = "PHLPlayer";
obj.posX = 1.5;
obj.posY = 0.75;
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
obj.class = "PHLCamera";
obj.posX = 3;
obj.posY = 1.75;
obj.camH = 3.5;
addObject(obj);

