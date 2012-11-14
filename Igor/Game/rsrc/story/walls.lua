local h = IGScripting:screenSize().y
upperWall = IGWallManager:new(h-0.6, h-0.2, 1, h)
lowerWall = IGWallManager:new(0.2, 0.6, 1, 0)
upperWall:attachToWorld()
lowerWall:attachToWorld()

