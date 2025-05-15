import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn import preprocessing
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import accuracy_score, confusion_matrix, classification_report
import os
import sys
import pydicom as pyd
from joblib import dump, load



# Read the CSV file
colsd = [
        'Sequence_Letter', 'Sequence_Number','Echo_Time', 'Repetition_Time', 'Scanner_Manufacturer', 'Series_Description', 'Scanning_Sequence', 
        'Sequence_Name', 'Flip_Angle', 'MR_Acquisition_Type', 'Sequence_Variant', 'Scan_Options', 
        'Magnetic_Field_Strength','Slice_Thickness'
        ]   
        
#cols = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N']
data = pd.read_csv("trainingSET.csv", sep = ',', low_memory=False, names=colsd)
data = data.fillna(0)
data.to_csv('file_name.csv')
'''
extraParams = []
for paths in data['filepath']:
    extraParams.append(getInfo(paths))
extraInfo = pd.DataFrame(extraParams)
print(extraParams)

'''
'''
data['ScanningSequence'] = extraInfo[:0]
data['Sequencename'] = extraInfo[:1]
data['Flipangle'] = extraInfo[:2]
data['MRacquisitiontype'] = extraInfo[:3]
data['Sequencevariant'] = extraInfo[:4]

data['Contrast/BolusAgent'] = extraInfo[:5]
data['Inversiontime'] = extraInfo[:6]
data['Scanoptions'] = extraInfo[:7]
data['MagneticFieldStrength'] = extraInfo[:8]
data['AngioFlag'] = extraInfo[:9]
data['SliceThickness'] = extraInfo[:10]
'''
print(data)
X = data[['Echo_Time','Repetition_Time','Flip_Angle']].to_numpy()
X_series_desc = data[['Series_Description']].to_numpy().reshape(-1)
X_manufacturer = data[['Scanner_Manufacturer']].to_numpy().reshape(-1)
X_scanning_Sequence = data[['Scanning_Sequence']].to_numpy().reshape(-1)
X_MRI_aqquisition = data[['MR_Acquisition_Type']].to_numpy().reshape(-1)
X_series_variant = data[['Sequence_Variant']].to_numpy().reshape(-1)
#X_CB_agent = data[['Contrast/BolusAgent']].to_numpy().reshape(-1)
#X_inversion_time = data[['Inversiontime']].to_numpy().reshape(-1)
#X_scan_options = data[['Scan_Options']].to_numpy().reshape(-1)
X_MF_strength = data[['Magnetic_Field_Strength']].to_numpy().reshape(-1)
#X_AngioFlag = data[['AngioFlag']].to_numpy().reshape(-1)
X_sliceThicness = data[['Slice_Thickness']].to_numpy().reshape(-1)

y = data[['Sequence_Number']].to_numpy().reshape(-1)
n, d = X.shape

X = X.astype('float64')
y = y.astype('float64').reshape(-1)

#Create bag of chars representation
print('creating bag of chars...')
chars = list(map(chr, range(97, 123))) + ['0','1','2','3','4','5','6','7','8','9']
k_chars = len(chars)

# = np.zeros((n,d+k_chars)).astype('float64')
#X_bag_of_chars_CB = np.zeros((n,d+k_chars)).astype('float64')
#X_bag_of_chars_Inversion_time = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_manu = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_MFstrength = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_MRItype = np.zeros((n,d+k_chars)).astype('float64')
#X_bag_of_chars_scanoptions = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_ScanSeq = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_seriesDesc = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_sliceThic = np.zeros((n,d+k_chars)).astype('float64')
X_bag_of_chars_seriesVariant = np.zeros((n,d+k_chars)).astype('float64')

for i in range(n):
    for j in range(d):
        X_bag_of_chars_seriesDesc[i,j] = X[i,j]

    for j in range(k_chars):    
        # print(X_series_desc[i], chars[j], X_series_desc[i].lower().(chars[j]))
        try:
            #X_bag_of_chars_Angio[i,j+d] = X_AngioFlag[i].lower().count(chars[j])
            #X_bag_of_chars_CB[i, j+d] = X_CB_agent[i].lower().count(chars[j])
            #X_bag_of_chars_Inversion_time[i, j+d] = X_inversion_time[i].lower().count(chars[j])
            X_bag_of_chars_manu[i,j+d] += X_manufacturer[i].lower().count(chars[j])
            X_bag_of_chars_MFstrength[i,j+d] = X_MF_strength[i].lower().count(chars[j])
            X_bag_of_chars_MRItype[i,j+d]= X_MRI_aqquisition[i].lower().count(chars[j])
            #X_bag_of_chars_scanoptions[i,j+d] += X_scan_options[i].lower().count(chars[j])
            X_bag_of_chars_ScanSeq[i,j+d] = X_scanning_Sequence[i].lower().count(chars[j])
            X_bag_of_chars_seriesDesc[i,j+d] = X_series_desc[i].lower().count(chars[j])
            X_bag_of_chars_sliceThic[i,j+d] = X_sliceThicness[i].lower().count(chars[j])
            X_bag_of_chars_seriesVariant[i,j+d] += X_series_variant[i].lower().count(chars[j])
        except:
            continue
X_bag_of_chars = X_bag_of_chars_manu + X_bag_of_chars_MFstrength + X_bag_of_chars_MRItype
X_bag_of_chars += X_bag_of_chars_ScanSeq + X_bag_of_chars_seriesDesc + X_bag_of_chars_seriesVariant + X_bag_of_chars_sliceThic

# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
X_train, X_test, y_train, y_test = train_test_split(X_bag_of_chars, y, test_size=0.2, random_state=42)

print('training...')
clf=RandomForestClassifier(max_depth=None,random_state=0,n_estimators=100)
clf.fit(X_train,y_train)

#save model
dump(clf, 'RandomForestModel.joblib') 

print(clf.score(X_train,y_train)*100)
print(clf.score(X_test,y_test)*100)


forest = RandomForestClassifier()
forest.fit(X_train, y_train)
y_pred_test = forest.predict(X_test)

matrix = confusion_matrix(y_test, y_pred_test)
matrix = matrix.astype('float') / matrix.sum(axis=1)[:, np.newaxis]

# Build the plot
plt.figure(figsize=(16,7))
sns.set(font_scale=1.4)
sns.heatmap(matrix, annot=True, annot_kws={'size':10},
            cmap=plt.cm.Greens, linewidths=0.2)

# Add labels to the plot
'''
class_names = data['Sequence_Letter'].unique()
tick_marks = np.arange(len(class_names))
tick_marks2 = tick_marks + 0.5
plt.xticks(tick_marks, class_names, rotation=25)
plt.yticks(tick_marks2, class_names, rotation=0)
plt.xlabel('Predicted label')
plt.ylabel('True label')
plt.title('Confusion Matrix for Random Forest Model')
plt.show()
'''
