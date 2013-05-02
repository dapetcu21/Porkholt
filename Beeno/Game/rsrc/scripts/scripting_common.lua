require("init_common")

objects = nil
layers = nil
addLayer = nil
layerAddImage = nil
jointWithClass = nil
addJoint = nil
addObject = function (o) return PHWorld:insertObject(o) end

function PHOutput(s)
  print("Porkholt: "..s)
end

function PHLog(fmt, ...)
  local t = {...}
  for i,v in ipairs(t) do
    if (type(v) == "table") then
      t[i] = tostring(v)
    end
  end
  PHOutput(string.format(fmt, ...))
end

function PHPrint(t,level,prefix,pre)
  if (not t) then return end
  level = level or 1
  prefix = prefix or ""
  pre = pre or ""
  if (type(t)=="table" and level>0) then
    PHLog(prefix..pre.."%s:",t)
    for i,v in pairs(t) do 
      PHLog(prefix.."  %s: %s",i,tostring(v)); 
    end
    local super = getmetatable(t);
    if (type(super)=="table") then
      super = super.__index
    end
    if (type(super)=="table") then
      PHPrint(super,level-1,prefix.."  ","index")
    end
  else
    PHLog(prefix..pre.."%s",tostring(t))
  end
end

function PHCallbackHelper(cb)
  cb.callback(unpack(cb.args))
end

PHWorld = {}
function PHWorld:insertObject(o) --not a good idea to batch-create objects mid-level, especially if not inserted at the end
  local ud = self._insertObj
  if ud then
    ud = ud.ud
  end
  return self:_insertObject(o,self._insertPos,ud)
end
function PHWorld:insertAtTheEnd()
  self._insertPos = 0
  self._insertObj = nil
end
function PHWorld:insertAtTheBeggining()
  self._insertPos = 1
  self._insertObj = nil
end
function PHWorld:insertBefore(o)
  self._insertPos = 2
  self._insertObj = o
end
function PHWorld:insertAfter(o)
  self._insertPos = 3
  self._insertObj = o
end
PHWorld:insertAtTheEnd()
function PHWorld:fadeToColor(color,cb,...)
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_fadeToColor(color,call)
end
function PHWorld:dismissFading(cb,...)
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_dismissFading(call)
end
--function PHWorld:dismissFading(cb,...)
--function PHWorld:overlayText(text,duration)
function PHWorld:curtainText(text,cb,...)
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_curtainText(text,call)
end
--function PHWorld:resourcePath()
--function PHWorld:win()
--function PHWorld:die()
--function PHWorld:boom(location,magnitude,damage,radius)

PHLObject = {}
function PHLObject:new(o,ud, ...)
  o = o or {}
  o.ud = ud
  setmetatable(o,self)
  self.__index = self
  o = o:init(...)
  return o
end
function PHLObject:init()
  return self
end
-- function PHLObject:rotation()
-- function PHLObject:position()
-- function PHLObject:transform()
-- function PHLObject:setRotation(r)
-- function PHLObject:setPosition(p)
-- function PHLObject:setTransform(p,r)
-- function PHLObject:move(p)
-- function PHLObject:rotateAround(r,a)
-- function PHLObject:rotate(r)
-- function PHLObject:isDynamic()
-- function PHLObject:setDynamic(d)
-- function PHLObject:destroy()
-- function PHLObject:invalidateAllAnimations()
-- function PHLObject:skipAllAnimations()
-- function PHLObject:addAnimation(anim)
-- function PHLObject:applyImpulse(impulse,applicationPoint)
-- function PHLObject:velocity()
-- function PHLObject:scalarVelocity()
-- function PHLObject:setVelocity(vel) -- vector
-- function PHLObject:angularVelocity()
-- function PHLObject:setAngularVelocity(val) -- number
-- function PHLObject:applyAngularImpulse(val) -- number
-- function PHLObject:mass()
-- function PHLObject:centerOfMass() --in object coordinates
-- function PHLObject:childWithTag(tag)
-- function PHLObject:poof()
-- function PHLObject::isFlipped()
-- function PHLObject::setFlipped(f)
-- function PHLObject::flip()

PHObject = {}
function PHObject:new(o, ...)
  o = o or {}
  setmetatable(o,self)
  self.__index = self
  o = o:init(...)
  return o
end
function PHObject:init()
  return self
end

PHKeyframeAnimatorGroup = PHObject:new()
-- function PHKeyframeAnimatorGroup:pause()
-- function PHKeyframeAnimatorGroup:resume()
-- function PHKeyframeAnimatorGroup:setFilterTag(tag)
-- function PHKeyframeAnimatorGroup:clearFilterTag()
-- function PHKeyframeAnimatorGroup:playSection()
-- function PHKeyframeAnimatorGroup.groupNamed()

PHTimer = PHObject:new{time = 0, willrepeat = false}
function PHTimer:init(tm,rep,cb, ...)
  if (PHObject.init(self)) then
    self.time = tm or 0
    self.willrepeat = rep or false
    self.callback = cb or nil
    self.args = {...}
    return self
  end
  return nil
end
function PHTimer:setCallback(cb, ...)
  self.callback = cb or nil
  self.args = {...}
end
function PHTimer:timerFired()
  local cb = self.callback
  local args = self.args or {}
  if (cb) then
    cb(unpack(args))
  end
end
--function PHTimer:invalidate()
--function PHTimer:schedule(timer)

PHLAnimation = PHObject:new{
  LinearFunction = 0,
  BounceFunction = 1,
  FadeInFunction = 2,
  FadeOutFunction = 3,
  FadeInOutFunction = 4,
  ConstantFunction = 5, --use this for forces and velocities
  curveFunction = LinearFunction
}
--PHLAnimation.time --the duration of the animation
--PHLAnimation.movement
---
--PHLAnimation.rotation
--PHLAnimation.rotationCenter -- in world coordinates or object coordinates if objectCoordinates is true
---
--PHLAnimation.velocity --tries to maintain this vectorial velocity using a maximum force of correctorForce, can be applied infinitely
--PHLAnimation.brakeForce --the maximum force it tries to brake with
--PHLAnimation.correctorForce --defaults to infinity, don't leave it at infinity unless you want instant velocity correction
---
--PHLAnimation.force --can be applied infinitely
--PHLAnimation.impulse --same as force if LinearFunction, allows for better distribution control
--PHLAnimation.angularImpulse
--PHLAnimation.objectCoordinates --apply the force/impulse relative to the object coordinate system, does not affect the application point, defaults to false
--PHLAnimation.forceApplicationPoint --the force/impulse application point
---
--PHLAnimation.curveFunction --this can be one of the built-in functions defined as numbers above or an outright function
--PHLAnimation.nextAnimation --you can chain animations
--PHLAnimation.disableDynamics --disable physics for the object while animated... defaults to true
--function PHLAnimation:invalidate() --this leaves the animation as it is and cancells it (including its callback)
--function PHLAnimation:skip() --this applies all changes immediately, except for forces and velocities
--function PHLAnimation:invalidateChain()
--function PHLAnimation:skipChain()
---
--PHLAnimation.callbackOnInvalidate --normally, calling invalidate() on an animation cancels the callback, set this to true to override that behaviour
function PHLAnimation:setCallback(cb,...)
  self.cb = cb
  self.args = {...}
end
function PHLAnimation:animationFinished() --don't call this manually
  if self.cb then
    self.cb(unpack(self.args or {}))
  end
end

PHView = PHObject:new()
--function PHView:rotation()
--function PHView:setRotation(rot)
--function PHView:horizontallyFlipped()
--function PHView:verticallyFlipped()
--function PHView:setHorizontallyFlipped()
--function PHView:setVerticallyFlipped()
--function PHView:frame()
--function PHView:setFrame(frame)

PHImageView = PHView:new()
--function PHImageView:width()
--function PHImageView:height()
--function PHImageView:setImage(image) --if it contains "/" image is interpreted as a path, else it's interpreted as an image name (rsrc/img/*)
--function PHImageView:tint()
--function PHImageView:setTint(color)
--function PHImageView:textureCoordinates()
--function PHImageView:setTextureCoordinates(tc)
--function PHImageView:constrainCurveToFrame()
--function PHImageView:setConstrainCurveToFrame()
--function PHImageView:repeatX()
--function PHImageView:setRepeatX(r)
--function PHImageView:repeatY()
--function PHImageView:setRepeatY(r)

PHParticleAnimator = PHObject:new()
--function PHParticleAnimator:isPlaying()
--function PHParticleAnimator:setPlaying(p)
--function PHParticleAnimator:isGenerating()
--function PHParticleAnimator:setGenerating(g)
--function PHParticleAnimator:generatingFor()
--function PHParticleAnimator:setGeneratingFor(time)
--function PHParticleAnimator:particlesPerSecond()
--function PHParticleAnimator:setParticlesPerSecond(pps)
--function PHParticleAnimator:generationArea()
--function PHParticleAnimator:setGenerationArea(rect)
--function PHParticleAnimator:elipticalArea()
--function PHParticleAnimator:setElipticalArea(isElipse)
--function PHParticleAnimator:velocity()
--function PHParticleAnimator:setVelocity(vec)
--function PHParticleAnimator:velocityVariation()
--function PHParticleAnimator:setVelocityVariation(delta)
--function PHParticleAnimator:spreadAngle()
--function PHParticleAnimator:setSpreadAngle(alpha)
--function PHParticleAnimator:gravity()
--function PHParticleAnimator:setGravity(vec)
--function PHParticleAnimator:initialSize()
--function PHParticleAnimator:setInitSize(sz)
--function PHParticleAnimator:finalSize()
--function PHParticleAnimator:setFinalSize(sz)
--function PHParticleAnimator:particleLifetime()
--function PHParticleAnimator:setParticleLifetime(time)
--function PHParticleAnimator:lifetimeVariation()
--function PHParticleAnimator:setLifetimeVariation(delta)
--function PHParticleAnimator:initialColor()
--function PHParticleAnimator:setInitColor(color)
--function PHParticleAnimator:finalColor()
--function PHParticleAnimator:setFinalColor(color)
--function PHParticleAnimator:rotatesWithVelocity()
--function PHParticleAnimator:setRotatesWithVelocity(bool)
--function PHParticleAnimator:play()
--function PHParticleAnimator:pause()
--function PHParticleAnimator:clear()

PHLNPC = PHLObject:new()
--function PHLNPC:hasTrail()
--function PHLNPC:setTrail(f)
--function PHLNPC:usesTrail()
--function PHLNPC:setUsesTrail(f)
function PHLNPC:addDialog(text,cb,...)
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_addDialog(text,call)
end
function PHLNPC:setDialog(text,cb,...) --this immediately shows the dialog bypassing the stack system
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_setDialog(text,call)
end
--function PHLNPC:showsQuest()
--function PHLNPC:setShowsQuest(s)
--function PHLNPC:reallyShowsQuest()
function PHLNPC:questTapped(obj) end --override this to do something when the user taps the quest/info popup
function PHLNPC:walk(offset,speed,cb,...) --same as walkTo(position()+offset,speed), speed optional, defaults to 2
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_walk(offset,speed,call)
end
function PHLNPC:walkTo(destination,speed,cb,...) -- speed optional, defaults to 2
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_walkTo(destination,speed,call)
end
--function PHLNPC:braked()
--function PHLNPC:setBraked() --brakes the body
--function PHLNPC:healthPoints()
--function PHLNPC:setHP(hp)
--function PHLNPC:increaseHP(ammount)
--function PHLNPC:decreaseHP(ammount)
--function PHLNPC:maximumHP()
--function PHLNPC:setMaximumHP(maxHP)
--function PHLNPC:die()
--function PHLNPC:invulnerable()
--function PHLNPC:setInvulnerable(b)
--function PHLNPC:isInvulnerable() --this also accounts for hurt vulnerability

PHLPlayer = PHLNPC:new()
--function PHLPlayer:usesForce()
--function PHLPlayer:setUsesForce(b) --enables or disables the force
--function PHLPlayer:userInput()
--function PHLPlayer:setUserInput(b) --enables or disabled user input for the player. doesn't brake the body
--function PHLPlayer:forceGauge()
--function PHLPlayer:setForceGauge(f)
--function PHLPlayer:maximumForce()
--function PHLPlayer:setMaximumForce(f)
--function PHLPlayer:forceGrowth()
--function PHLPlayer:setForceGrowth(g)
--function PHLPlayer:isBarHidden()
--function PHLPlayer:setBarHidden(b)
function PHLPlayer:setFreezed(b)
  self:setBraked(b)
  self:setUserInput(not b)
end

PHLSensor = PHLObject:new()
function PHLSensor:objectEntered(obj) --override these. obj is either a table representing an object or a string with the object's class if the object isn't scriptable
end
function PHLSensor:objectExited(obj)
end

PHLMob = PHLNPC:new()

PHLBull = PHLMob:new()
PHLBomberBird = PHLMob:new()
PHLEggBomb = PHLSensor:new()

function PHLBomberBird.createAndLaunchBird(accuracy,dropVelocity)
  local bird = objectWithClass("PHLBomberBird")
  local b = camera:bounds()
  bird.startingPoint = point(
    -bird.bounds.width-bird.bounds.x,
    b.height-bird.bounds.height-bird.bounds.y)
  local p = player:position()
  bird.rotationAxis = point(p.x - b.x+(math.random()*2-1),bird.bounds.height+15+2.5*(math.random()*2-1))
  bird.accuracy = accuracy
  bird.dropVelocity = dropVelocity
  PHWorld:insertAtTheEnd()
  bird = PHWorld:insertObject(bird)
  bird:attack()
  return bird
end

PHLCamera = PHLObject:new()
--function PHLCamera:bounds()
--function PHLCamera:followsPlayer()
--function PHLCamera:setFollowsPlayer(b)

PHLSign = PHLNPC:new()
function PHLSign:display(cb,...)
  local call = nil
  if (cb) then
    call = { callback = cb, args = {...} }
  end
  self:_display(call)
end
function PHLSign:questTapped()
  self:display()
end

PHLPowerup = PHLSensor:new()
function PHLPowerup:collected()
end
function PHLPowerup:dismissed()
end
-- function PHLPowerup:dismiss()
-- function PHLPowerup:collect()

PHLLevelEnd = PHLSensor:new()

PHLShieldPowerup = PHLPowerup:new()
PHLHPPowerup = PHLPowerup:new()
PHLPowerPowerup = PHLPowerup:new()
