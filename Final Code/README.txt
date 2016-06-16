Hand Gesture Recognition Using Krawtchouk Moments

Steps to run the code:
1. Extract Final Code
2. Open MATLAB(version R2015 or higher), migrate to the directory of Final Code
3. type the following commands
	learn = createFeatureVector();   %%Takes around 1 min to run

	label = [5 5 5 5 5 5 4 4 4 4 3 3 3 3 1 1 1 2 2 2 2 6 6 6 4 2 4 4 1 1 1 5 3 6 2 1]'			 	%% Set the labels for current data set

	test  =  test_image('<image-number>.jpg',learn, label, 1)
