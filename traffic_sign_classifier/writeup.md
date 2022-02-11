# **Traffic Sign Recognition** 

## Writeup

---

**Build a Traffic Sign Recognition Project**

The goals / steps of this project are the following:
* Load the data set (see below for links to the project data set)
* Explore, summarize and visualize the data set
* Design, train and test a model architecture
* Use the model to make predictions on new images
* Analyze the softmax probabilities of the new images
* Summarize the results with a written report

[image1]: ./examples/training_images.png "Visualization"
[image2]: ./examples/validation_images.png "Grayscaling"
[image3]: ./examples/color_img.png "Color images"
[image4]: ./examples/gray_images.png "Gray images"
[image5]: ./examples/normalized_img.png "Normalized images"
[image6]: ./examples/new_imgs.png "New test images"
[image7]: ./examples/new_grayscale_img.png "Grayscal test image"

[image6]: ./examples/gray_images1.png "Random Noise"
[image7]: ./examples/gray_images2.png "Traffic Sign 1"
[image8]: ./examples/normalized_image1.png "Traffic Sign 2"
[image9]: ./examples/normalized_image2.png "Traffic Sign 3"
[image10]: ./examples/placeholder.png "Traffic Sign 4"
[image11]: ./examples/placeholder.png "Traffic Sign 5"


## Rubric Points
### Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/481/view) individually and describe how I addressed each point in my implementation.  

---
### Writeup / README

#### 1. Provide a Writeup / README that includes all the rubric points and how you addressed each one. You can submit your writeup as markdown or pdf. You can use this template as a guide for writing the report. The submission includes the project code.

You're reading it! and here is a link to my [project code](https://github.com/tesfamic/traffic_sign_classifier/blob/master/Traffic_Sign_Classifier.ipynb)

### Data Set Summary & Exploration

#### 1. Provide a basic summary of the data set. In the code, the analysis should be done using python, numpy and/or pandas methods rather than hardcoding results manually.

I used the python and numpy libraries to calculate summary statistics of the traffic
signs data set:

* The size of training set is 34799
* The size of the validation set is 4410
* The size of test set is 12630
* The shape of a traffic sign image is 32x32x3
* The number of unique classes/labels in the data set is 43

#### 2. Include an exploratory visualization of the dataset.

Here is an exploratory visualization of the data set. It is a simple plot of the images selected randomly from the training and validation datasets. The caption at the top of the image shows the corresponding 'label' of the image.

Trainings images 
![alt text][image1]

Validation images
![alt text][image2]

### Design and Test a Model Architecture

#### 1. Describe how you preprocessed the image data. What techniques were chosen and why did you choose these techniques? Consider including images showing the output of each preprocessing technique. Pre-processing refers to techniques such as converting to grayscale, normalization, etc. (OPTIONAL: As described in the "Stand Out Suggestions" part of the rubric, if you generated additional data for training, describe why you decided to generate additional data, how you generated the data, and provide example images of the additional data. Then describe the characteristics of the augmented training set like number of images in the set, number of images for each class, etc.)
> Three image preprocessing operations have been applied.
  * Conversion to grayscale image
  * Histogram equalization
  * Normalization 
  
> As a first step, I decided to convert the images to grayscale for two reasons mentioned below. 
   1. To Reduce costs associated to computation. Grayscale image has a single channel where as color image has 3 channels. Therefore processing grayscale images reduces the computation by about a 1/3 (Less memory, less training time means lower overall cost).
  2. For this particula application, it seems that grayscale format provided better classification accuracy as demonstrated by [Sermanet and Yan](http://ieeexplore.ieee.org/document/6033589/). I believe that shape feature is more usefull than color feature for traffic signs.

> As a second step, histogram equalization is applied to enhance the images in the dataset. This operation specially improves the contrast level of some of the pictures which are very dark or very bright.

> The last (mandatory operation) preprocessing is normalization. The images are normalized to get a pixel value in the range [-1, 1]. The formula recommended to use for normalization is (pixel - 128)/128. Normalization is very important for the CNN algorithm to converge quickly and to get better accuracy. This is because when the weights are re-evaluated for optimization, the amount of correction factor to be applied would be uniform. Without normalization, the algorithm has to apply different correction factors to minimize errors because of various values per pixel. Therefore, normalization helps the algorithm to converge quickly.

Here are examples of traffic sign images before and after preprocessing (grayscaling followed by histogram equalization and normalization)

Color images taken randomly from the dataset
![alt text][image3]

Grayscale and histogram equalized images 
![alt text][image4]

Normalized images
![alt text][image5]

#### 2. Describe what your final model architecture looks like including model type, layers, layer sizes, connectivity, etc.) Consider including a diagram and/or table describing the final model.

The model used for this assignment is LeNet CovNet as described in the course material. To summarize,

| Layer         		|     Description	        					| 
|:---------------------:|:---------------------------------------------:| 
| Input         		| 32x32x1 Grayscale image   							| 
| Convolution 5x5     	| 1x1 stride, VALID padding, outputs 28x28x6 	|
| RELU					|		Activation function							|
| Max pooling	      	| 2x2 stride (ksize of 2), VALID padding,  outputs 14x14x6 				|
| Convolution 5x5	    | 1x1 stride, VALID padding, outputs 10x10x16   				|
| RELU					|		Activation function							|
| Max pooling	      	| 2x2 stride (ksize of 2), VALID padding,  outputs 5x5x16 				|
| Fully connected		| Input 400, Output 120        									|
| RELU				|  Activation function      									|
| Fully connected		| Input 120, Output 84        									|
| RELU				|  Activation function      									|
| Fully connected		| Input 84, Output 43        									|



#### 3. Describe how you trained your model. The discussion can include the type of optimizer, the batch size, number of epochs and any hyperparameters such as learning rate.

> The following are the hyperparameters used:
   - Batch size: 64
   - Epoch : 30
   - Learning rate: 0.001
   - Standard deviation (sigma): 0.1
   - Mean: 0.0 
   
 > The training procedure is a kind of standard. First, the dataset is divided into batches, then the batches are fed to the algorithm that tries to reduce the mean entropy and optimize it using the Adam optimizer. In the training process, various batch sizes, learning rate and epochs are tested. I got better performance when batch size is lower and epoch is a bit longer.

#### 4. Describe the approach taken for finding a solution and getting the validation set accuracy to be at least 0.93. Include in the discussion the results on the training, validation and test sets and where in the code these were calculated. Your approach may have been an iterative process, in which case, outline the steps you took to get to the final solution and why you chose those steps. Perhaps your solution involved an already well known implementation or architecture. In this case, discuss why you think the architecture is suitable for the current problem.

My final model results were:
* validation set accuracy of 93.1%
* test set accuracy of 91%

The maximum validation accuracy obtained is ~95% which I think is fair for this particular model. Overall, the validation accuracy can range from 91 - 95% using the techniques described above.

### Test a Model on New Images

#### 1. Choose five German traffic signs found on the web and provide them in the report. For each image, discuss what quality or qualities might be difficult to classify.

Here are ten German traffic signs that I found on the web:

![alt text][image6] 

Grayscale and histogram equalized 
![alt text][image7]

 Ten images are considered for testing. In these images, children crossing, right-of-way, pedestrian and road work are somehow similar appearance when they are preprocessed. Therefore, it is expected that the model might confuse one with the other during classification.

#### 2. Discuss the model's predictions on these new traffic signs and compare the results to predicting on the test set. At a minimum, discuss what the predictions were, the accuracy on these new predictions, and compare the accuracy to the accuracy on the test set (OPTIONAL: Discuss the results in more detail as described in the "Stand Out Suggestions" part of the rubric).

Here are the results of the prediction:

| Image			        |     Prediction	        					| 
|:---------------------:|:---------------------------------:| 
| Children crossing      		| Right-of-way at next intersection   	| 
| Road work    			| Road work 										|
| Keep right					| Keep right											|
| Pedestrians	      		| Pedestrians					 				|
| Priority Road			| Priority road      							|
| Right-of-way at next intersection | Right-of-way at next intersection |
| Turn right ahead  | Turn right ahead |
| Stop | Stop  |
| Speed limit (30km/h) | Speed limit (50km/h) |
| General caution  | General caution |

The model was able to correctly guess 8 of the 10 traffic signs, which gives an accuracy of 80%. Looking at the incorrectly classified images, it is fair to say that those images are close to to the actual images. So, with more tuning on the parameters or by adding more layers it could be easy to improve the accuracy.

#### 3. Describe how certain the model is when predicting on each of the five new images by looking at the softmax probabilities for each prediction. Provide the top 5 softmax probabilities for each image along with the sign type of each probability. (OPTIONAL: as described in the "Stand Out Suggestions" part of the rubric, visualizations can also be provided such as bar charts)

> Here is the result

 | True image         | Predictions   |  Comment  |
 |:------------------:|:---------------------:|:---------:|
 | Children crossing  | 100% - Right-of-way at ... <br/>  0% - Beware of ice/snow   <br/>	0% - Children crossing|  Incorrect |
 | Road work    			| 100% - Road work 								| Correct |
 | Keep right		 	| 100% - Keep right <br/>0% - Speed limit (30km/h <br/>	0% - Wild animals crossing		| Correct |
 | Pedestrians	      | 100% - Pedestrians	<br/> 0% - Right-of-way at .. <br/>	0% - General caution			 				| Correct |
 | Priority Road			|100% - Priority road     				| |
 | Right-of-way at next intersection |100% - Right-of-way at ... | Correct |
 | Turn right ahead  | 100% Turn right ahead | Correct |
 | Stop | 99.6% - Stop <br/>0.23% - Speed limit (50km/h) <br/>0.07% - Yield | Correct |
 | Speed limit (30km/h) | 62.9% Speed limit (50km/h) <br/> 36.4% Speed limit (30km/h) <br/> 0.61% Roundabout mandatory | Incorrect |
 | General caution  | 100% General caution | Correct |

> There are two incorrectly classified images out of ten test images. Most of the correctly classified images have got 100% certainty. The algorithm misclassified the 'children crossing' and 'speed limit of 30km/h' images with 'right-of-way ' and 'speed limit of 50km/h'. The misclassified images are somehow similar to the true images. The reason for misclassification is probably because of the preprocessing stage in the pipline have taken away some of the important features that distinguish the signs. This can be seen from the grayscale images of the test images considered.

### (Optional) Visualizing the Neural Network (See Step 4 of the Ipython notebook for more details)
#### 1. Discuss the visual output of your trained network's feature maps. What characteristics did the neural network use to make classifications?


