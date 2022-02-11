from styx_msgs.msg import TrafficLight
import cv2
import numpy as np

class TLClassifier(object):
    def __init__(self):
        #TODO load classifier
        pass

    def get_classification(self, image):
        """Determines the color of the traffic light in the image
           Color segmentation is used to determine the color.
           In this implementation, only RED is the color of interest.
           For other colors (yellow and green), the car keeps driving.
        Args:
            image (cv::Mat): image containing the traffic light

        Returns:
            int: ID of traffic light color (specified in styx_msgs/TrafficLight)

        """

        #TODO implement light color prediction: Only RED color is considered.
        # import random
        # cv2.imwrite("/home/robond/Pictures/"+str(random.randint(0,10000))+".png",image)

        img_hsv = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
        min_thresh = np.array([0, 150, 150],np.uint8)
        max_thresh = np.array([50, 255, 255],np.uint8)
        
        img_thresholded = cv2.inRange(img_hsv, min_thresh, max_thresh)
        red_pixel_count = cv2.countNonZero(img_thresholded)

        #if the number of white pixels from the thresholded image is greater than a threshold
        # (determined by trial and error), it is assumed that the count is from red light/color
        if red_pixel_count>300:
            return TrafficLight.RED        

        return TrafficLight.UNKNOWN
