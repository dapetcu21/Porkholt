bullets = IGBulletManager:new()
bullets:attachToWorld()

bounds = IGScreenBounds:new()
bounds:attachToWorld()

function bullets:onImpact(object, bullet)
    if (bullet.owner == 1) then
        if (object) then
            print("mob hit")
        end
    elseif (bullet.owner == 2) then
        if (object == player) then
            print("player hit")
        end
    end
    bullets:removeBullet(bullet.handle)
end

