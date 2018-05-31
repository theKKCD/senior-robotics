import cv2
import vision

video = cv2.VideoCapture(1)
cv2.namedWindow('image', cv2.WINDOW_NORMAL)

while(True):
    frame = vision.getFrame(video)
    if frame is not None:
        overlayedFrame = frame.copy()
        hsv = vision.getHSVFrame(frame)

        # Identify the ball
        ballCenter, ballRadius = vision.findBall(hsv)
        if ballCenter:
            cv2.circle(overlayedFrame, ballCenter, ballRadius, (0, 255, 0), 2)

        cv2.imshow('image', overlayedFrame)

    # Stop program on ESC
    key = cv2.waitKey(10)
    if key == 27:
        break


cv2.destroyAllWindows()