import numpy as np
import pickle
import tensorflow as tf
#os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"   #if like me you do not have a lot of memory in your GPU
# os.environ["CUDA_VISIBLE_DEVICES"] = "" #then these two lines force keras to use your CPU
from keras.layers import Dense, Flatten, Conv3D, MaxPooling3D, Dropout, BatchNormalization
import pandas as pd
#import matplotlib.pyplot as plt
import h5py
from scipy.ndimage import zoom
#from PIL import Image
#import seaborn as sns
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle
import nibabel as nib
import os


'''
def save_image(path,image_array):
    image = plt.imsave(path, image_array)
    '''
'''
def plot_confusion_matrix(actual, predicted,labels,dataset):
  cm = tf.math.confusion_matrix(predicted, actual)
  ax = sns.heatmap(cm, annot=True, fmt='g')
  sns.set(rc={'figure.figsize':(12, 12)})
  sns.set(font_scale=1.4)
  ax.set_title('Confusion phenotype classification for ' + dataset)
  ax.set_xlabel('Actual Class')
  ax.set_ylabel('Predicted Class')
  plt.xticks(rotation=90)
  plt.yticks(rotation=0)
  ax.xaxis.set_ticklabels(labels)
  ax.yaxis.set_ticklabels(labels)
  plt.show()

def split_list(lst, chunk_size):
    return [lst[i:i + chunk_size] for i in range(0, len(lst), chunk_size)]
'''
##Function used to load data into correct dimensions

def load_data(desired_shape=(60,60,20)):
    try:
        X = []
        y = []
        #print("hello")
        dataFilePath = '/ISILON/home/kierasht/DB_artifactsDump.csv'
        dataFile = pd.read_csv(dataFilePath)
        #print(dataFile[['filepath']])
        imagePathFiles = dataFile['filepath'].values.tolist()
        #print(imageFile)
        #print("YES")
        #print(imagePathFiles)
        QCStats = dataFile['QC_artifacts'].values.tolist()
        #print(QCStats)
        #print("NEXT")
        
        #print(imagePathFiles)
        artifactStatus = QCStats[:0]
        #Loop over all subject directories in data_path
        total_nii_files = len(imagePathFiles)
        count = 0

        for index, nii_file in enumerate(imagePathFiles):
            # if (site_id != '03'): continue
                
            count += 1
            imageStringPath = nii_file.partition('/nifti/')
            imageStringPath = imageStringPath[0] + imageStringPath[1] + imageStringPath[2].split('/')[0]

            img_path = os.path.join(nii_file.strip())
            nii_file = nii_file[:-3]
            img = nib.load(img_path)
            
            img_data = img.get_fdata()
            #save_image(nii_file,img_data[:,:,img_data.shape[2]//2]/10)

            #resize image to desired shape
            orig_shape = img_data.shape
            if len(orig_shape) == 3:
                #print(orig_shape)
                width_factor = desired_shape[0]/orig_shape[0]
                height_factor = desired_shape[1]/orig_shape[1]
                depth_factor = desired_shape[2]/orig_shape[2]

                img_data = zoom(img_data, (width_factor, height_factor, depth_factor), order=1) 
                
                X.append(img_data)
                y.append(QCStats[index])
                print("artifacts")
                print(y)
                #[X[i:i+100] for i in range(0, len(X), 100)]
                
                #y_chunk = split_list(y,100)

                #print(y_chunk)
            

                print('('+str(count)+'/'+str(total_nii_files)+')',nii_file, orig_shape ,'->',img_data.shape)
            
                #save_image(nii_file,img_data[:,:,img_data.shape[2]//2]/10)
            # quit()print('NII FILE')
                
        #Convert list to numpy array
        #X = np.array(X)
        #y = np.array(y)
    
        #print("X")
        #print(X.shape, X)
        #X = X.reshape(X.shape[0], X.shape[1], X.shape[2], X.shape[3], 1)
        
    except RuntimeError as e:
        print(e)
        pass
    return X, y

X, y = load_data(desired_shape=(60,60,20))
#print(y,y.shape)
print(X)
print(y)

X1 = X[:len(X)//2]
X2 = X[len(X)//2:]
y1 = y[:len(y)//2]
y2 = y[len(y)//2:]

pickle.dump(X1, open('2024X1preNumpy.pkl', "wb"))
pickle.dump(X2, open('2024X2preNumpy.pkl', "wb"))
pickle.dump(y1, open('2024y1preNumpy.pkl', "wb"))
pickle.dump(y2, open('2024y2preNumpy.pkl', "wb"))

#X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, train_size= 0.8, random_state=42)

#pickle.dump(X_train, open('X_train.pkl', 'wb'))
#pickle.dump(X_test, open('X_test.pkl', 'wb'))
#pickle.dump(y_train, open('y_train.pkl', 'wb'))
#pickle.dump(y_test, open('y_test.pkl', 'wb'))

##Joining Pickles, done on Sockeye due to memory concerns
##

def imageLoading():
    X1 = pickle.load(open('2024X1preNumpy.pkl', 'rb'))
    X2 = pickle.load(open('2024X2preNumpy.pkl', 'rb'))
    y1 = pickle.load(open('2024y1preNumpy.pkl', 'rb'))
    y2 = pickle.load(open('2024y2preNumpy.pkl', 'rb'))

    X1 = np.array(X1)
    X2 = np.array(X2)
    y1 = np.array(y1)
    y2 = np.array(y2)
    

    
    X = np.concatenate((X1,X2))
    y = np.concatenate((y1,y2))
    
    X = X.reshape(X.shape[0], X.shape[1], X.shape[2], X.shape[3],1)
    X = np.array(X) 
    y = np.array(y)
    print(X)
    print(y)

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, train_size= 0.8, random_state=42)
    pickle.dump(X_train, open('2024Xtrain.pkl', 'wb'))
    pickle.dump(X_test, open('2024Xtest.pkl', 'wb'))
    pickle.dump(y_train, open('2024ytrain.pkl', 'wb'))
    pickle.dump(y_test, open('2024ytest.pkl', 'wb'))


def PickleAnalysis():
    X_train = pickle.load(open('XTrain.pkl', 'rb'))
    y_train = pickle.load(open('yTrain.pkl', 'rb'))
    X_test = pickle.load(open('XTest.pkl', 'rb'))
    y_test = pickle.load(open('yTest.pkl', 'rb'))

    print(y_train)
    #print(X_train)

    X_train, y_train = shuffle(X_train, y_train, random_state=42)
    X_test, y_test = shuffle(X_test, y_test, random_state=42)

    print('X_test',X_test.shape)
    print('y_test',y_test.shape)

    print('X_train',X_train.shape)
    print('y_train',y_train.shape)

    n, x, y, z, c = X_train.shape
    k = y_train.shape

    sample_shape = (x,y,z,c)
    #Create the model
    model = tf.keras.models.Sequential()
    model.add(tf.keras.layers.Conv3D(32, kernel_size=(3, 3, 3), activation='relu', kernel_initializer='he_uniform', input_shape=sample_shape))
    model.add(tf.keras.layers.MaxPooling3D(pool_size=(2, 2, 2)))
    # model.add(BatchNormalization(center=True, scale=True))
    # model.add(Dropout(0.5))
    model.add(tf.keras.layers.Conv3D(64, kernel_size=(3, 3, 3), activation='relu', kernel_initializer='he_uniform'))
    model.add(tf.keras.layers.MaxPooling3D(pool_size=(2, 2, 2)))
    # model.add(BatchNormalization(center=True, scale=True))
    # model.add(Dropout(0.5))
    model.add(tf.keras.layers.Flatten())
    model.add(tf.keras.layers.Dense(256, activation='relu', kernel_initializer='he_uniform'))
    model.add(tf.keras.layers.Dense(256, activation='relu', kernel_initializer='he_uniform'))
    model.add(tf.keras.layers.Dense(1, activation='sigmoid'))

    # Compile the model
    model.compile(loss='binary_crossentropy',
                optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                metrics=['binary_accuracy'])
    
    history = model.fit(X_train, y_train,
            batch_size=128,
            epochs=23,
            verbose=1,
            validation_split=0.2,
            class_weight={0:1, 1:3.84, 2:4.63, 3:4.49},
            steps_per_epoch=None,
            shuffle=True)
    
    model.summary()
    # Fit data to model
    
    #save trained model
    model = model.save('OfficialCNN.h5')

    # #load trained model
    # model = keras.models.load_model('model.h5')   

    #get predicted labels
    y_pred = model.predict(X_train)
    y_pred = np.argmax(y_pred, axis=0)

    y_test_pred = model.predict(X_test)
    y_test_pred = np.argmax(y_test_pred, axis=0)

    #get accuracies
    train_accuracy = np.mean(y_pred == np.argmax(y_train, axis=0))
    test_accuracy = np.mean(y_test_pred == np.argmax(y_test, axis=0))

    model.evaluate(X_train, y_train, verbose=1)
    print(train_accuracy, test_accuracy)


#imageLoading()
PickleAnalysis()
