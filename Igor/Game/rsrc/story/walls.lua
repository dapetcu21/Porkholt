bg_particles = IGBackgroundParticles:new()
bg_particles:attachToWorld()

ripples = IGRipples:new()
ripples:attachToWorld()
ripples:addToDistortion(bg_particles)

local h = IGScripting:screenSize().y
upperWall = IGWallManager:new(h-0.6, h-0.2, 1, h)
lowerWall = IGWallManager:new(0.2, 0.6, 1, 0)
upperWall:attachToWorld()
lowerWall:attachToWorld()

