# Hand Gesture Recognition using Moments

Hand gesture recognition system can be used for interfacing between computer and human using hand gesture. This work presents a technique for a human computer interface through hand gesture recognition that is able to recognize static gestures from the standard numerical gestures. The objective of this thesis is to develop an algorithm for efficient segmentation and recognition of hand gestures with reasonable accuracy.

## Segmentation

The segmentation of hands is performed using the Codebook method for background subtraction. In the method, the hand is removed at first and the program is trained for the give background. After sufficient number of frames are recorded, a Codebook model is formed against which all the subsequent frames are compared. If any difference, the resultant is shown as a white hole on black background. This may contain a few holes which are removed using masking the segments.

## Feature Extraction

For the feature extraction, Krawtchouk moments are used to create a feature vector. A matrix is formed containing the features of all the images from the training data set. The extracted features are applied to a knn classifier. The K-nearest neighbours are then found for the given input image and the hand gesture is predicted accordingly.

## Results

We were able to bring down the runtime from approximately 4 hours to 62 seconds by using a recursive method for the hypergeometric function.
Along with that, the accuracy in detecting the gestures was 92% without transformation and 88.57% with transformation.

## Steps of Hand Gesture Recognition

![Alt text](https://github.com/sartaj10/Final-Year-Project/blob/master/Images/steps.png "Steps")

## Segmentation Results

![Alt text](https://github.com/sartaj10/Final-Year-Project/blob/master/Images/segmentation_results.png "Segmentation Results")

## Classified Results

![Alt text](https://github.com/sartaj10/Final-Year-Project/blob/master/Images/results_classified.png "Classified Results")

## Tablulated Results

![Alt text](https://github.com/sartaj10/Final-Year-Project/blob/master/Images/Tabulated-1.png "Tabulated Results")

![Alt text](https://github.com/sartaj10/Final-Year-Project/blob/master/Images/Tabulated-2.png "Tabulated Results")
