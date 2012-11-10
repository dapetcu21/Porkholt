local h = IGScripting:screenSize().y
upperWall = IGWallManager:new(h-0.5, h, 1)
lowerWall = IGWallManager:new(0, 0.5, 1)
upperWall:attachToWorld()
lowerWall:attachToWorld()

