objects = { n = 0 }

function addObject(o)
  objects[objects.n] = o
  o.index = objects.n
  objects.n = objects.n + 1
  return o
end

layers = { n = 0 }

function addLayer(l)
  layers[layers.n] = l
  layers.n = layers.n + 1
end

function layerAddImage(l,fn, x, y, w, h,opt)
  l.n = l.n or 0
  local img = opt or {}
  img.filename = fn
  img.pos = rect(x,y,w,h)
  l[l.n] = img
  l.n = l.n + 1
end


function objectAddImage(obj,fn, x, y, w, h,opt)
  obj.images = obj.images or {}
  obj.images.n = obj.images.n or 0
  opt = opt or {}
  opt.filename = fn
  opt.pos = rect(x,y,w,h)
  obj.images[obj.images.n] = opt
  obj.images.n =   obj.images.n + 1
end

function objectAddBox(obj,x, y, w, h,proto)
  local fix = obj.physics.fixtures
  local tmp = proto or {}
  fix[fix.n] = tmp
  tmp.shape = "box"
  tmp.box = rect(x,y,w,h)
  setmetatable(tmp,obj._fixturemeta)
  fix.n = fix.n + 1  
end

function objectAddCircle(obj,r,proto)
  local fix = obj.physics.fixtures
  local tmp = proto or {}
  fix[fix.n] = tmp
  tmp.shape = "circle"
  tmp.circleR = r
  setmetatable(tmp,obj._fixturemeta)
  fix.n = fix.n + 1  
end

function objectAddFreeform(obj,curve,proto)
  local fix = obj.physics.fixtures
  local tmp = proto or {}
  fix[fix.n] = tmp
  tmp.shape = "freeform"
  tmp.curve = curve
  setmetatable(tmp,obj._fixturemeta)
  fix.n = fix.n + 1
end

cWORLD = 1
cPLAYER = 2
cNPC = 4
cMOB = 8

Initializers = {}
function Initializers.PHLObject(obj)
  obj.images = { n = 0 }
  obj.physics = { fixtures = {n = 0} }
  obj._fixtureindex = {}
  obj._fixtureindex.categoryBits = cWORLD
  obj._fixtureindex.maskBits = cWORLD + cPLAYER + cNPC + cMOB
  obj._fixturemeta = { __index = obj._fixtureindex }
end
function Initializers.PHLCamera(obj)
  Initializers.PHLObject(obj)
end
function Initializers.PHLNPC(obj)
  Initializers.PHLObject(obj)
  obj._fixtureindex.categoryBits = cNPC
  obj._fixtureindex.maskBits = cWORLD
  obj.physics.dynamic = true
  obj.overHead = point(0,0.5)
  obj.questHeight = 0.5
  obj.overHead = point(0.15,0.3)
  obj.questPoint = point(0.15,0.3)
  obj.canBlink = false
end
function Initializers.PHLPlayer(obj)
  Initializers.PHLNPC(obj)
  obj._fixtureindex.categoryBits = cPLAYER
  obj._fixtureindex.maskBits = cWORLD + cPLAYER + cMOB
  obj.maxVelocityX = 3
  obj.trail = true
  obj.trailSize = 10
  obj.staticFace = true
  obj.faceFlipping = true
  obj.automaticFlipping = true
  obj.canBlink = true
  objectAddCircle(obj,0.25,{restitution = 0.5}) --0.5 restitution is the exact value for which the jumping system can't be exploited
  objectAddImage(obj,"/ball",-0.25,-0.25,0.5,0.5,{tag=20, class="PHTrailImageView"})
  objectAddImage(obj,"/face",-0.25,-0.25,0.5,0.5,{tag=21})
end
function Initializers.PHLSensor(obj)
  Initializers.PHLObject(obj)
  obj._fixtureindex.maskBits = cPLAYER
end
function Initializers.PHLMob(obj)
  Initializers.PHLNPC(obj)
  obj._fixtureindex.categoryBits = cMOB
  obj._fixtureindex.maskBits = cWORLD + cPLAYER
end
function Initializers.PHLBull(obj,scale)
  Initializers.PHLMob(obj)
  scale = scale or 1
  obj.bodyFlipping = true
  obj.attackRange = 3.5
  obj.attackVelocity = 5
  obj.attackDuration = 0.5
  obj.cooldownDuration = 2
  obj.relaxTime = 1
  local x = -1.008398
  local y = -0.316404
  local w = 1.8
  local h = 0.794531
  local bw = 1
  local bh = 0.636172
  local hbw = 0.1
  objectAddImage(obj,"/bull_body", x*scale, y*scale, w*scale, h*scale, {tag = 20 })
  objectAddImage(obj,"/bull_eye", x*scale, y*scale, w*scale, h*scale, {tag = 21 } )
  objectAddBox(obj,(bw/2-hbw)*scale,-bh/2*scale,hbw*scale,bh*scale)
  objectAddBox(obj,-bw/2*scale,-bh/2*scale,(bw-hbw)*scale,bh*scale)
end
function Initializers.PHLGround(obj)
  Initializers.PHLObject(obj)
end
function Initializers.PHLSign(obj)
  Initializers.PHLNPC(obj)
  obj.overHead = point(0.25,0.25)
  obj.questPoint = point(0.25,0.25)
  obj.physics.dynamic = false
  objectAddImage(obj,"/sign",-0.35,-0.35,0.7,0.7)
end
function Initializers.PHLPowerup(obj)
  Initializers.PHLSensor(obj)
end
function Initializers.PHLLevelEnd(obj)
  Initializers.PHLSensor(obj)
  local h = 4.0
  local w = h*0.75
  objectAddImage(obj,"/sunshaft", -w/2,-0.5, w, h)
  objectAddBox(obj,-0.25,-0.25,0.5,0.5)
end
function Initializers.PHLShieldPowerup(obj)
  Initializers.PHLPowerup(obj)
  objectAddImage(obj,"/blue_bottle", -0.15,-0.3,0.3,0.6, { tag = 40 })
  objectAddBox(obj, -0.15,-0.3,0.3,0.6)
end
function Initializers.PHLHPPowerup(obj)
  Initializers.PHLPowerup(obj)
  objectAddImage(obj,"/red_bottle", -0.15,-0.3,0.3,0.6, { tag = 40 })
  objectAddBox(obj, -0.15,-0.3,0.3,0.6)
end
function Initializers.PHLPowerPowerup(obj)
  Initializers.PHLPowerup(obj)
  objectAddImage(obj,"/green_bottle", -0.15,-0.3,0.3,0.6, { tag = 40 })
  objectAddBox(obj, -0.15,-0.3,0.3,0.6)
end
function Initializers.PHLBigBull(obj)
  Initializers.PHLBull(obj,1.5)
  obj.class = "PHLBull"
  print("big boss bull loaded")
end
function Initializers.PHLBomberBird(obj)
  Initializers.PHLMob(obj)
  local b = rect(-0.3,-0.3,0.6,0.6)
  local bds = vector(376,327)
  local bws = rect(20,0,186,193)
  local scale = 1/bds.x
  obj.bounds = rect(-bds.x/2*scale,-bds.y/2*scale,bds.x*scale,bds.y*scale)
  objectAddImage(obj,"/bird_body", -bds.x/2*scale,-bds.y/2*scale,bds.x*scale,bds.y*scale)
  objectAddImage(obj,"/bird_wing", (bws.x-bds.x/2)*scale,(bws.y-bds.y)/2*scale,bws.width*scale,bws.height*scale)
end
function Initializers.PHLEggBomb(obj)
  Initializers.PHLSensor(obj)
  obj.markAsSensor = false
  obj.physics.dynamic = true
  obj._fixtureindex.categoryBits = cMOB
  obj._fixtureindex.maskBits = cWORLD + cPLAYER + cNPC + cMOB
  local ar = 167/211
  local h = 0.6
  local box = rect(-h*ar/2,-0.4,h*ar,h)
  objectAddImage(obj,"/egg_bomb", box.x,box.y,box.width,box.height)
  objectAddBox(obj, box.x,box.y,box.width,box.height)
end

function objectWithClass(class,...)
  obj = {}
  if (not obj.class) then
    obj.class = class
  end
  obj.realClass = class
  if (Initializers[class]) then
    Initializers[class](obj,...)
  else
    Initializers.PHLObject(obj,...)
  end
  return obj
end

JointInitializers = {}

function JointInitializers.PHJoint(joint)
end

function jointWithClass(class)
  joint = {}
  joint.class = class
  if (JointInitializers[class]) then
    JointInitializers[class](joint)
  else
    JointInitializers.PHJoint(joint)
  end
  return joint
end

joints = { n = 0 }

function addJoint(o)
  joints[joints.n] = o
  o.index = joints.n
  joints.n = joints.n + 1
  return o
end

function colorWithRGBA(r,g,b,a)
  return {
    r = r or 1.0,
    g = g or 1.0,
    b = b or 1.0,
    a = a or 1.0
  }
end

rgba = colorWithRGBA
color = rgba

function point(x,y)
  return {
    x = x or 0,
    y = y or 0
  }
end

function range(start,length)
  return {
    start = start or 0,
    length = length or 0
  }
end

vector = point

function rotatedPoint(p,angle)
  angle = -angle/180 * math.pi
  return {
    x = math.cos(angle)*p.x-math.sin(angle)*p.y,
    y = math.sin(angle)*p.x+math.cos(angle)*p.y
  }
end

function translatedPoint(p,m)
  return {
    x = p.x+m.x,
    y = p.y+m.y
  }
end

function invertedPoint(p)
  return {
    x = -p.x,
    y = -p.y
  }
end

function rect(x,y,w,h)
  return {
    x = x or 0,
    y = y or 0,
    width = w or 0,
    height = h or 0
  }
end

-- log bridges

function angleToPoint(r,c,a)
  local pnt = {}
  pnt.x = c.x + r*math.cos(a)
  pnt.y = c.y + r*math.sin(a)
  return pnt
end

function pointToAngle(r,c,p)
  local ang
  local cos = p.x-c.x
   local sin = p.y-c.y
  local d = math.sqrt(cos^2+sin^2)
  cos = cos/d
  sin = sin/d
  if (math.abs(sin)<0.5) then
    ang = math.asin(sin)
    if (cos<0) then
      ang = math.pi-ang
    end
  else
    ang = math.acos(cos)
    if (sin<0) then
      ang = 2*math.pi-ang
    end
  end
  if (ang<0) then
    ang = ang + 2*math.pi
  end
  return ang
end

function createBridge(bridgeRadius, p1,p2 ,numLogs, jointPos, creator)
  local q = math.sqrt((p1.x-p2.x)^2+(p1.y-p2.y)^2)
  local c1 = {}
  local c2 = {}
  local m = {}
  m.x = (p1.x+p2.x)/2
  m.y = (p1.y+p2.y)/2
  c1.x = m.x + math.sqrt(bridgeRadius^2-(q/2)^2)*(p1.y-p2.y)/q
  c1.y = m.y + math.sqrt(bridgeRadius^2-(q/2)^2)*(p2.x-p1.x)/q
  c2.x = m.x - math.sqrt(bridgeRadius^2-(q/2)^2)*(p1.y-p2.y)/q
  c2.y = m.y - math.sqrt(bridgeRadius^2-(q/2)^2)*(p2.x-p1.x)/q
  local c = c1
  if (c1.y>c2.y) then
    c = c1
  else
    c = c2
  end
  
  local a1 = pointToAngle(bridgeRadius,c,p1)
  local a2 = pointToAngle(bridgeRadius,c,p2)
  while (a2<=a1) do
    a2=a2+math.pi*2
  end
  
  local add = (a2-a1)/(numLogs-1)
  
  local first = nil
  local last = nil
  
  while (numLogs ~= 0) do
    local pnt = angleToPoint(bridgeRadius,c,a1)
    local obj = creator()
    obj.pos = pnt
    obj.rotation = 280-(a1/math.pi*180)
    a1=a1+add
    numLogs=numLogs-1
    if (last) then
      local joint = jointWithClass("PHDistanceJoint")
      joint.body1 = last
      joint.body2 = obj
      joint.anchor1 = {x=jointPos, y=0}
      joint.anchor2 = {x=-jointPos, y=0}
      joint.collideConnected = false
      addJoint(joint)
    else
      first = obj
    end
    last = obj
  end
  return first,last
end

