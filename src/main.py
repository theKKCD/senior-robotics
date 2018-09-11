import vision
import motors

try:
	#import compass
	import pi
except ImportError:
	print("Disabling Pi specific functions")
	from stubs import *

moveBot = True

try:
    
    print("Started MAIN SCRIPT")

    while True:
            stop = vision.loop()
            if stop:
                    break

            if moveBot: # motors and shit
                    centrePadding = 25
                    print(vision.getBallCenter())

                    if vision.getBallCenter() is not None:
                            ballXPos = vision.getBallCenter()[0]

                            if abs(ballXPos) <= centrePadding:
                                    #do nothing
                                    pass
                            elif ballXPos > 0:
                                    motors.rotateCenter(direction = 1, power = 50)
                            elif ballXPos < 0:
                                    motors.rotateCenter(direction = -1, power = 50)

                    else:
                            pass#motors.rotateCenter(direction = 1, power = 50)
                            
    # do a bit of cleanup
    vision.cleanup()
    motors.cleanup()
    print("Script Ended Cleanly")
    
except KeyboardInterrupt:
    vision.cleanup()
    motors.cleanup()
    print("Keyboard Interrupt/nCleaned")
