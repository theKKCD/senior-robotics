import cv2
import numpy as np
import os
import time

from imutils.video import WebcamVideoStream
import imutils

goalCalibration = False

mins = [0,0,0]
maxs = [0,0,0]

cv2.namedWindow('image', cv2.WINDOW_NORMAL)

def trackbarCallback(value):
	pass

cv2.createTrackbar('Hmin', 'image', 0, 255, trackbarCallback)
cv2.createTrackbar('Smin', 'image', 0, 255, trackbarCallback)
cv2.createTrackbar('Vmin', 'image', 0, 255, trackbarCallback)

cv2.createTrackbar('Hmax', 'image', 0, 255, trackbarCallback)
cv2.createTrackbar('Smax', 'image', 0, 255, trackbarCallback)
cv2.createTrackbar('Vmax', 'image', 0, 255, trackbarCallback)

cv2.createTrackbar('gamma', 'image', 0, 350, trackbarCallback)

cv2.setTrackbarPos('Hmin', 'image', 0)
cv2.setTrackbarPos('Hmax', 'image', 255)
cv2.setTrackbarPos('Smin', 'image', 0)
cv2.setTrackbarPos('Smax', 'image', 255)
cv2.setTrackbarPos('Vmin', 'image', 0)
cv2.setTrackbarPos('Vmax', 'image', 255)

vs = WebcamVideoStream(src=1).start()

while True:

	def processFrame(frame):
		frame = imutils.resize(frame, 320)
		frame = cv2.GaussianBlur(frame,(9,9),0)
		frame = cv2.medianBlur(frame,9)
		hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
		return frame, hsv

	frame, hsv = processFrame(vs.read())

	cv2.imshow("hsv", hsv)
	cv2.imshow("rgb", frame)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

	hsv = cv2.resize(frame, (320, 240))
	hsv = cv2.cvtColor(hsv, cv2.COLOR_BGR2HSV)
	hsv = cv2.GaussianBlur(hsv, (5, 5), 5)

	hmin = cv2.getTrackbarPos('Hmin', 'image')
	smin = cv2.getTrackbarPos('Smin', 'image')
	vmin = cv2.getTrackbarPos('Vmin', 'image')

	hmax = cv2.getTrackbarPos('Hmax', 'image')
	smax = cv2.getTrackbarPos('Smax', 'image')
	vmax = cv2.getTrackbarPos('Vmax', 'image')

	lower = (hmin,smin,vmin)
	upper = (hmax,smax,vmax)
	
	mask = cv2.inRange(hsv, lower, upper)
	cv2.imshow('image', mask)

	key = cv2.waitKey(1)
	if key == 27:
		break
	elif key == 115:
		hmin = cv2.getTrackbarPos('Hmin', 'image')
		smin = cv2.getTrackbarPos('Smin', 'image')
		vmin = cv2.getTrackbarPos('Vmin', 'image')

		hmax = cv2.getTrackbarPos('Hmax', 'image')
		smax = cv2.getTrackbarPos('Smax', 'image')
		vmax = cv2.getTrackbarPos('Vmax', 'image')

		settingsFileRead = open(os.path.dirname(os.path.abspath(__file__)) + "/../calibrationSettings.py")
		lines = []
	  
		for line in settingsFileRead:
			key = line.split(" = ")[0]
			
			if goalCalibration:
				if key == "goalLower":
					line = key + " = (" + str(hmin) + ", " + str(smin) + ", " + str(vmin) + ")\n" 
				elif key == "goalUpper":
					line = key + " = (" + str(hmax) + ", " + str(smax) + ", " + str(vmax) + ")\n"
				elif key == "goalWidthHeightRatio":
					mask = cv2.inRange(hsv, (hmin, smin, vmin), (hmax, smax, vmax))
					image, contours, hierarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
					c = max(contours, key=cv2.contourArea)
					perimeter = cv2.arcLength(c, True)
					approx = cv2.approxPolyDP(c, 0.04 * perimeter, True)
					(x, y, w, h) = cv2.boundingRect(approx)
					line = key + " = " + str(round(w / h, 2)) + "\n"
			else:
				if key == "ballLower":
					line = key + " = (" + str(hmin) + ", " + str(smin) + ", " + str(vmin) + ")\n" 
				elif key == "ballUpper":
					line = key + " = (" + str(hmax) + ", " + str(smax) + ", " + str(vmax) + ")\n" 
			
			lines.append(line)

		settingsFileRead.close()

		settingsFileWrite = open(os.path.dirname(os.path.abspath(__file__)) + "/../calibrationSettings.py", "w")
		settingsFileWrite.writelines(lines)
		settingsFileWrite.close()

cv2.destroyAllWindows()