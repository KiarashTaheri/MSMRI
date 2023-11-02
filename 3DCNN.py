import nibabel as nib
import os
import numpy as np
import pickle
import keras
# os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"   #if like me you do not have a lot of memory in your GPU
# os.environ["CUDA_VISIBLE_DEVICES"] = "" #then these two lines force keras to use your CPU
from keras.models import Sequential
from keras.layers import Dense, Flatten, Conv3D, MaxPooling3D, Dropout, BatchNormalization
from keras.utils import to_categorical
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import h5py
from scipy.ndimage import zoom
from PIL import Image
import cv2
import seaborn as sns
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle

def save_image(path,image_array):
    im = Image.fromarray(image_array)
    im = im.convert('RGB')
    im.save(os.path.join('test-out',path))

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

def load_data(desired_shape=(60,60,20)):
    X = []
    y = []

    data_path = os.path.join(os.getcwd(), 'data')
    #Loop over all subject directories in data_path
    total_nii_files = len(os.listdir(data_path))
    count = 0
    for nii_file in os.listdir(data_path):
        site_id = nii_file.split('-')[1]
        # if (site_id != '03'): continue

        count += 1
        img_path = os.path.join(data_path, nii_file)

        img = nib.load(img_path)
        img_data = img.get_fdata()

        # save_image(nii_file+'.png',img_data[:,:,img_data.shape[2]//2]/10)

        #resize image to desired shape
        orig_shape = img_data.shape
        img_data = zoom(img_data, (desired_shape[0]/orig_shape[0],desired_shape[1]/orig_shape[1],desired_shape[2]/orig_shape[2]), order=1)

        #Append labels to y based on subject_dir
        if ('RRM' in img_path):
            y.append(0)
        elif ('PPM' in img_path):
            y.append(1)
        elif ('CON' in img_path):
            y.append(2)
        elif ('RIS' in img_path):
            y.append(3)
        else:
            continue

        X.append(img_data)
        print('('+str(count)+'/'+str(total_nii_files)+')',nii_file, orig_shape ,'->',img_data.shape)
        
        # save_image(nii_file+'_resized.png',img_data[:,:,img_data.shape[2]//2]/10)
        # quit()

    #Convert list to numpy array
    X = np.array(X)
    y = np.array(y)
    y = to_categorical(y)
    X = X.reshape(X.shape[0], X.shape[1], X.shape[2], X.shape[3], 1)

    return np.array(X), np.array(y)

# X, y = load_data(desired_shape=(60,60,20))

# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# pickle.dump(X_train, open('X_train.pkl', 'wb'))
# pickle.dump(X_test, open('X_test.pkl', 'wb'))
# pickle.dump(y_train, open('y_train.pkl', 'wb'))
# pickle.dump(y_test, open('y_test.pkl', 'wb'))
# quit()

X = pickle.load(open('X_MNI.pkl', 'rb'))
y = pickle.load(open('y_MNI.pkl', 'rb'))
y = to_categorical(y)
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# X_train = pickle.load(open('X_train.pkl', 'rb'))
# y_train = pickle.load(open('y_train.pkl', 'rb'))
# X_test = pickle.load(open('X_test.pkl', 'rb'))
# y_test = pickle.load(open('y_test.pkl', 'rb'))

X_train, y_train = shuffle(X_train, y_train, random_state=42)
X_test, y_test = shuffle(X_test, y_test, random_state=42)

print('X_test',X_test.shape)
print('y_test',y_test.shape)

print('X_train',X_train.shape)
print('y_train',y_train.shape)

n, x, y, z, c = X_train.shape
_, k = y_train.shape

sample_shape = (x,y,z,c)

#Create the model

model = Sequential()
model.add(Conv3D(32, kernel_size=(3, 3, 3), activation='relu', kernel_initializer='he_uniform', input_shape=sample_shape))
model.add(MaxPooling3D(pool_size=(2, 2, 2)))
# model.add(BatchNormalization(center=True, scale=True))
# model.add(Dropout(0.5))
model.add(Conv3D(64, kernel_size=(3, 3, 3), activation='relu', kernel_initializer='he_uniform'))
model.add(MaxPooling3D(pool_size=(2, 2, 2)))
# model.add(BatchNormalization(center=True, scale=True))
# model.add(Dropout(0.5))
model.add(Flatten())
model.add(Dense(256, activation='relu', kernel_initializer='he_uniform'))
model.add(Dense(256, activation='relu', kernel_initializer='he_uniform'))
model.add(Dense(k, activation='softmax'))

# Compile the model
model.compile(loss='categorical_crossentropy',
              optimizer=keras.optimizers.Adam(learning_rate=0.001),
              metrics=['accuracy'])
model.build(sample_shape)
model.summary()
# Fit data to model
history = model.fit(X_train, y_train,
            batch_size=128,
            epochs=100,
            verbose=1,
            validation_split=0.2,
            class_weight={0:1, 1:3.84, 2:4.63, 3:4.49},
            steps_per_epoch=None,
            shuffle=True)
            

#save trained model
model.save('model.h5')

# #load trained model
# model = keras.models.load_model('model.h5')

#get predicted labels
y_pred = model.predict(X_train)
y_pred = np.argmax(y_pred, axis=1)

y_test_pred = model.predict(X_test)
y_test_pred = np.argmax(y_test_pred, axis=1)

#get accuracies
train_accuracy = np.mean(y_pred == np.argmax(y_train, axis=1))
test_accuracy = np.mean(y_test_pred == np.argmax(y_test, axis=1))

model.evaluate(X_train, y_train, verbose=1)

#get list of per class accuracies
train_perClass_accuracy = []
test_perClass_accuracy = []
for i in range(4):
    train_perClass_accuracy.append(round(np.mean(y_pred[np.argmax(y_train, axis=1) == i] == i),3))
    test_perClass_accuracy.append(round(np.mean(y_test_pred[np.argmax(y_test, axis=1) == i] == i),3))

print('y_train per class', train_perClass_accuracy)
print('y_test per class', test_perClass_accuracy)

#get count of each class from y_train
unique, counts = np.unique(np.argmax(y_train, axis=1), return_counts=True)
print('y_train',dict(zip(unique, counts)))

#get count of each class from y_test
unique, counts = np.unique(np.argmax(y_test, axis=1), return_counts=True)
print('y_test',dict(zip(unique, counts)))

print('train_accuracy',train_accuracy)
print('test_accuracy',test_accuracy)

plot_confusion_matrix(np.argmax(y_train, axis=1), y_pred, labels=['RRM', 'PPM', 'CON', 'RIS'],dataset='training dataset')
plot_confusion_matrix(np.argmax(y_test, axis=1), y_test_pred, labels=['RRM', 'PPM', 'CON', 'RIS'],dataset='testing dataset')
