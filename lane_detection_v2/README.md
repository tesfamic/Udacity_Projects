## Advanced Lane Finding Project


The goals / steps of this project are the following:

* Compute the camera calibration matrix and distortion coefficients given a set of chessboard images.
* Apply a distortion correction to raw images.
* Use color transforms, gradients, etc., to create a thresholded binary image.
* Apply a perspective transform to rectify binary image ("birds-eye view").
* Detect lane pixels and fit to find the lane boundary.
* Determine the curvature of the lane and vehicle position with respect to center.
* Warp the detected lane boundaries back onto the original image.
* Output visual display of the lane boundaries and numerical estimation of lane curvature and vehicle position.

[//]: # (Image References)

[image0]: ./output_images/distorted_calibration1.png "Distorted"
[image1]: ./output_images/corrected_calibration1.png "Undistorted"
[image2]: ./output_images/distorted_test6.png "Original"
[image3]: ./output_images/corrected_test6.png "Undistorted"
[image4]: ./output_images/sobel_threshold.png "Soble x-gardient thresholded"
[image5]: ./output_images/hls_threshold.png "HLS - s-channel thresholded"
[image6]: ./output_images/original_image.png "Unwarped image"
[image7]: ./output_images/warped_image.png "Warped image"
[image8]: ./output_images/curve_fitted_image.png "Curve fitted image"
[image9]: ./output_images/before_detection.png "Reference image"
[image10]: ./output_images/after_detection.png "After lane detection"
[image11]: ./output_images/error_image.png "Challenges of detection"

[video1]: ./output_video.avi "Video"


### Here I will consider the rubric points individually and describe how I addressed each point in my implementation.  

---

### STEPS

### Camera Calibration

#### 1. Briefly state how you computed the camera matrix and distortion coefficients. Provide an example of a distortion corrected calibration image.

> I followed the exact steps covered in the materials presented before this project section. I have also used the opencv documentation page [here](https://docs.opencv.org/3.1.0/dc/dbb/tutorial_py_calibration.html) as a reference. The code for this step is contained in the third code cell of the Jupyter notebook located in "lane_detector_v2.ipynb". 
I start by preparing "object points", which will be the (x, y, z) coordinates of the chessboard corners in the world. Here I am assuming the chessboard is fixed on the (x, y) plane at z=0, such that the object points are the same for each calibration image.  Thus, `objp` is just a replicated array of coordinates, and `objpoints` will be appended with a copy of it every time I successfully detect all chessboard corners in a test image.  `imgpoints` will be appended with the (x, y) pixel position of each of the corners in the image plane with each successful chessboard detection.  
I then used the output `objpoints` and `imgpoints` to compute the camera calibration and distortion coefficients using the built in opencv function `cv2.calibrateCamera()`.  I applied this distortion correction to the test image using the `cv2.undistort()` function and obtained this result: 

 ![alt text][image0] 
 ![alt text][image1] 

### Pipeline (single images)

#### 1. Provide an example of a distortion-corrected image.

>To demonstrate this step, I will describe how I apply the distortion correction to one of the test images shown below.
As can be seen in the images below, the effect of undistortion is visible on the nearest tree on the left side. It appears closer in the corrected/undistorted image than the original.

![alt text][image2]
![alt text][image3]

#### 2. Describe how (and identify where in your code) you used color transforms, gradients or other methods to create a thresholded binary image.  Provide an example of a binary image result.

> I used a combination of color and gradient thresholds to generate a binary image. For the gradient thresholding, Sobel operation in x-gradient is used and for the color thresholding the s-channel of HLS color space is implemented. This thresholding operations are implemented in code cell 2 (collection of functions) in jupyter notebook (lane_detector.ipynb). Here's an example of my output for this step using one of the test images provided.

![alt text][image4] 
![alt text][image5]

#### 3. Describe how (and identify where in your code) you performed a perspective transform and provide an example of a transformed image.

> Warping operation (transforming perspective image into Bird's eye view image) involves finding the tranformation matrix obtained by 'perspectiveTransform()' function in opencv. Therefore, first the coordinates used to perfrom transfromation operaton are first selected by trial and error, and then the transformation matrices are calculated using 'perspectiveTransform()' function. Finally, the warping operation is done. The functions involved in the process can be found in code cell 2 with the following names.
       * `get_transform_matrices()` - returns two matrices called M and M_inv for transforming image into bird's eye view and back to perspective view.
       * `warp_img(image,t_matrix)` - takes the image to be warped and the transformation matrix. This function is used to warp/unwarp depending on the t_matrix provided. 
The source and destination coordinates(points) are the following:

| Source        | Destination   | 
|:-------------:|:-------------:| 
| 590,450       | 300,0         | 
| 690,4500      | 980,0         |
| 1060,680      | 980,720       |
| 260,680       |300,720        |

I verified that my perspective transform was working as expected by drawing the `src` and `dst` points onto a test image and its warped counterpart to verify that the lines appear parallel in the warped image.

![alt text][image6] 
![alt text][image7]

#### 4. Describe how (and identify where in your code) you identified lane-line pixels and fit their positions with a polynomial?

>I used the code given in the discussion section of this chapter to extract lane lines with very minor modifications. I chose to use the sliding window technique as presented. Before the actuall extraction of lane lines begins, the pre-processing step is executed. The sequence of operations is described in the notebook code cell 2 function called `detect_lanes()` which takes an image, camera matrix and distortion matrix for distortion correction ,transformation matrices for warping and unwarping. The function returns three parameters: processed image in the perspective view, curvature of lane and offset of lane center (see image below).

>Steps followed to detect lane-line pixels in brief:
  * Start from the bottom half of image,get histogram of the image for each column
  * Divide histogram array into two halves (vertically) -> bottom left quarter image, bottom right quarter image
  * Calculate the column with maximum histogram value for each quarter image (left lane and right lane) and use this as starting pixel 
  * Move up the image using the starting pixel locations as a reference point and find the white pixels in a certain window with specified dimensions and calculate the average position of the white pixels in the window. Use that position as the next lane-line pixel.
  * Repeat the preceeding step until the top image border is reached for both left and right half of image.
  * Based on collected pixel positions, apply second order curve fitting algorithm called `polyfit()` offered by python numpy library to get the curve that represents the lane lines.

![alt text][image8] 

#### 5. Describe how (and identify where in your code) you calculated the radius of curvature of the lane and the position of the vehicle with respect to center.

> The radius of curvature of any polynomial at a specified point can be calculated as shown in [here](https://www.intmath.com/applications-differentiation/8-radius-curvature.php) or as described in the note [here](https://classroom.udacity.com/nanodegrees/nd013/parts/fbf77062-5703-404e-b60c-95b78b2f3f9e/modules/2b62a1c3-e151-4a0e-b6b6-e424fa46ceab/lessons/096009a1-3d76-4290-92f3-055961019d5e/concepts/2f928913-21f6-4611-9055-01744acc344f). The code corresponding to this operation can be found around the bottom quarter of the lane_detector.ipynb notebook.

#### 6. Provide an example image of your result plotted back down onto the road such that the lane area is identified clearly.

> Sample output images are shown below. The function `detect_lanes()` in the lane_detector.ipynb notebook takes care of unwarping the image after lane-lines are identified and curve fitted.

![alt text][image10]

---

### Pipeline (video)

#### 1. Provide a link to your final video output.  Your pipeline should perform reasonably well on the entire project video (wobbly lines are ok but no catastrophic failures that would cause the car to drive off the road!).

Here's a [link to my video result](./output_video.avi)

---

### Discussion

####  Briefly discuss any problems / issues you faced in your implementation of this project.  Where will your pipeline likely fail?  What could you do to make it more robust?

> In my opinion, the lane detection techniques used in this project are very efficient when the lane marks are not covered by shadow due to trees, bridges or buildings and when there is sufficient contrast between the paint and the road. Otherwise, the technique will not work as desired. For instance, consider the image below. The portion of the image covered by tree shadow caused a little problem in the detection process. The challenge videos given in this project are also good test examples of how inefficient color and simple edge detection algorithms are under those scenarios. 
> To make more robust pipeline, other additional advanced techniques like lane tracking with vehicle dynamics and more recent approach using deep learning could be used.

![alt text][image11]

