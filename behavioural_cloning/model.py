import csv
import cv2
import h5py
import sklearn
import numpy as np
import matplotlib.pyplot as plt
from sklearn.utils import shuffle
from sklearn.model_selection import train_test_split
from keras.models import Sequential
from keras.layers import Flatten, Dense, Lambda, Dropout
from keras.layers.convolutional import Convolution2D
from keras.optimizers import Adam
from keras.layers import Cropping2D
####################################################
#  Data preparation
def generator(samples, batch_size=32):
    num_samples = len(samples)
    while 1: 
        shuffle(samples)
        for offset in range(0, num_samples, batch_size):
            batch_samples = samples[offset:offset+batch_size]

            images = []
            angles = []
            main_path = 'C:/Users/Tesfamichael/Documents/udacity/CarND-Behavioral-Cloning-P3-master/data/'
            for batch_sample in batch_samples:
                center_name = batch_sample[0].split('/')[-1]
                left_name = batch_sample[1].split('/')[-1]
                right_name = batch_sample[2].split('/')[-1]

                center_image = cv2.imread(center_name)
                left_image = cv2.imread(left_name)
                right_image = cv2.imread(right_name)

                center_angle = float(batch_sample[3])
                left_angle = float(batch_sample[3]) + 0.2
                right_angle = float(batch_sample[3]) - 0.2

                images.append(np.asarray(center_image))
                images.append(np.asarray(left_image))
                images.append(np.asarray(right_image))

                angles.append(center_angle)
                angles.append(left_angle)
                angles.append(right_angle)

            x_train = np.array(images)
            y_train = np.array(angles)
            yield sklearn.utils.shuffle(x_train, y_train)

def prep_data(samples,src):
    images = []
    angles = []
    if src == 'local':
        main_path = ''
    else:
        main_path = 'C:/Users/Tesfamichael/Documents/udacity/CarND-Behavioral-Cloning-P3-master/data/'

    for sample in samples:
        center_name = sample[0].split('/')[-1]
        left_name = sample[1].split('/')[-1]
        right_name = sample[2].split('/')[-1]

        center_image = cv2.imread(main_path+center_name)
        left_image = cv2.imread(main_path+left_name)
        right_image = cv2.imread(main_path+right_name)

        center_angle = float(sample[3])
        left_angle = float(sample[3]) + 0.2
        right_angle = float(sample[3]) - 0.2

        images.append(center_image)
        images.append(left_image)
        images.append(right_image)

        angles.append(center_angle)
        angles.append(left_angle)
        angles.append(right_angle)
    return images, angles
# #################################################################
#  Read csv files where the images are stored
#    Two csv files are read 
#      --> driving_log_cw - driving data collected by driving clockwise
#      --> driving_log_ccw - driving data collected by driving counter clockwise
#   ** All the images are stored in a single folder called IMG

lines = []
with open('data/driving_log_cw.csv') as csvfile:
    reader = csv.reader(csvfile)
    for line in reader:
        lines.append(line)
with open('data/driving_log_ccw.csv') as csvfile:
    reader = csv.reader(csvfile)
    for line in reader:
        lines.append(line)

##################################################################
#            CALL THE FUNCTION 'prep_data'
#     The following lines including the print statements should be
#     commented if 'keras' generator is to be used.
#------------------------------------------------------------
x1,y1 = prep_data(lines, 'local')

# Store read image data in array form for model training and validation
X_train = np.array(x1)
Y_train = np.array(y1)

print('x_train length:',len(X_train), ' shape:',X_train.shape)
print('y_train length:',len(Y_train), ' shape:',Y_train.shape)
#---------------------------------------------------------------------
#####################################################
#  Model structure/architecture directly taken from the 
model = Sequential()

# Normalize the input image with zero mean and deviation of 0.5
model.add(Lambda(lambda x: x / 255.0 - 0.5, input_shape=(160, 320, 3)))

# Crop the image 70 pixel rows from top of image and 25 pixels from bottom of image
#   new image size is 65x320
model.add(Cropping2D(cropping=((70, 25), (0, 0))))

# Add three 5x5 convolution layers (output depth 24, 36, and 48), each with 2x2 stride
# and one dropOut layer before the last convolution layer
model.add(Convolution2D(24, 5, 5, subsample=(2, 2), activation='relu'))
model.add(Convolution2D(36, 5, 5, subsample=(2, 2), activation='relu'))
model.add(Dropout(0.50))
model.add(Convolution2D(48, 5, 5, subsample=(2, 2), activation='relu'))

# Add two 3x3 convolution layers each with output depth of 64 and a dropout layer
model.add(Convolution2D(64, 3, 3, activation='relu'))
model.add(Convolution2D(64, 3, 3, activation='relu'))
model.add(Dropout(0.50))

# Add flatten layer
model.add(Flatten())

# Add three fully connected layers (depth 100, 50, 10) and one dropout before 
# the last fully connected layer
model.add(Dense(100))
model.add(Dense(50))
model.add(Dropout(0.50))
model.add(Dense(10))

# Output steering angle
model.add(Dense(1))

# Compile and train the model,
model.compile(loss='mse', optimizer='adam')
history_object= model.fit(X_train, Y_train, validation_split=0.2, shuffle=True, nb_epoch=5)

#################################################################################
#  The following code should be uncommented if 'generator' is to be used
#--------------------------------------------------------------------------------
# train_samples, validation_samples = train_test_split(lines, test_size=0.2)
#
# train_generator = generator(train_samples, batch_size=64)
# validation_generator = generator(validation_samples, batch_size=64)
#
# history_object = model.fit_generator(   train_generator,
#                                         validation_data=validation_generator,
#                                         steps_per_epoch=len(train_samples),
#                                         validation_steps=len(validation_samples),
#                                         epochs=5, shuffle=True, verbose= 1)
#---------------------------------------------------------------------------------
# Save model data
model.save_weights('./model.h5')
model.save('model.h5')
###################################################################################
### print the keys contained in the history object
print(history_object.history.keys())

# plot the training and validation loss for each epoch
plt.plot(history_object.history['loss'])
plt.plot(history_object.history['val_loss'])
plt.title('model mean squared error loss')
plt.ylabel('mean squared error loss')
plt.xlabel('epoch')
plt.legend(['training set', 'validation set'], loc='upper right')
plt.show()
################################################################################