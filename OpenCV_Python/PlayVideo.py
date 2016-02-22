import numpy as np
import cv2

cap = cv2.VideoCapture('/Users/sartaj10/Downloads/Castle.2009.S08E11.HDTV.x264-LOL[ettv]/castle.2009.811.hdtv-lol[ettv].mp4')

while(cap.isOpened()):
    ret, frame = cap.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()