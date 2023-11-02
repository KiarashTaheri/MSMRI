import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
#from sklearn import preprocessing
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import accuracy_score, confusion_matrix, classification_report
import os
import sys
import pydicom as pyd
from joblib import dump, load

##Helper Function that extracts contrast bolus parameter and saves it into a list
def getBolus():
    bolusValues = []
    testFile = pd.read_csv("output4.tsv", index_col = False, sep= "\t", engine= "python")
    paths = testFile['exapleDICOM'].values
    for num, path in enumerate(paths):
        if num != 0:
            #print(path)
            dicom = os.path.join(path)
            if pyd.dcmread(dicom):
                ds = pyd.dcmread(dicom)
                if '0x00180010' in ds:
                    bolus = ds[0x00180010].value
                    print(bolus)
                else:
                    bolus = "No"
                bolusValues.append(bolus)
                print("No")
            else:
                print("Can't open DICOM")
    return bolusValues

##Uses Helper function to save bolus agent into dataset
def extractBolus():
    # Read the CSV file
    colsd = [
            'id', 'exapleDICOM','QC_status', 'repetition_time',	'echo_time',	'series_description',	'scanner_manufacturer',	'acquisition_type',	'body_part_examined',	'sequenceLabel'
            ] 
            
    data = pd.read_csv("output4.tsv", sep= "\t", engine="python")
    print(data)
    data['Contrast/BolusAgent'] = pd.Series(getBolus())
    data = data.dropna()
    data.to_csv('parsedInfoRF.csv')
    print(data)


##Creates random forest
def randomForestNew():
    #colums = [
            #'id', 'exapleDICOM','QC_status', 'repetition_time',	'echo_time',	'series_description',	'scanner_manufacturer',	'acquisition_type',	'body_part_examined',	'sequenceLabel', 'Contrast/BolusAgent' ] 
    RFdata = pd.read_csv("parsedInfoRF.csv", engine="python")
    

    #print(RFdata)
    RFdata = RFdata.replace(r'^\s*$', "No", regex=True)

    convertCols = ['Sequence', 'letter']
    seqDataCovert = pd.read_csv('DB_sequenceTypes21.csv',names=convertCols)

    #print(seqDataCovert)
    seqDict  = dict(zip(seqDataCovert['Sequence'], seqDataCovert['letter']))
    #print(seqDict)

    RFdata['Sequence_Number'] = RFdata['sequenceLabel'].map(seqDict)
    RFdata = RFdata.replace({'Sequence_Number': {chr(i + 64): i for i in range(1, 27)}})

    RFdata['Contrast/BolusAgent'] = RFdata['Contrast/BolusAgent'].fillna(0)
    RFdata['Contrast/BolusAgent'] = RFdata['Contrast/BolusAgent'].replace('No',0)
    RFdata['Contrast/BolusAgent'] = RFdata['Contrast/BolusAgent'].replace({'\D': 1}, regex=True)
    print(RFdata['Contrast/BolusAgent'])

 
#Reshapes parameters into desired form

    print(RFdata)
    X = RFdata[['echo_time','repetition_time', 'Contrast/BolusAgent']].to_numpy()
    X_series_desc = RFdata[['series_description']].to_numpy().reshape(-1)
    X_manufacturer = RFdata[['scanner_manufacturer']].to_numpy().reshape(-1)
    X_MRI_aqquisition = RFdata[['acquisition_type']].to_numpy().reshape(-1)
    X_bodyPart_Examined = RFdata[['body_part_examined']].to_numpy().reshape(-1)
    
    
    y = RFdata[['Sequence_Number']].to_numpy().reshape(-1) 
    n, d = X.shape

    X = X.astype('float64')
    y = y.astype('float64').reshape(-1)

    #Create bag of chars representation
    print('creating bag of chars...')
    chars = list(map(chr, range(97, 123))) + ['0','1','2','3','4','5','6','7','8','9']
    k_chars = len(chars)

    X = np.zeros((n,d+k_chars)).astype('float64')
    #X_bag_of_chars_CB = np.zeros((n,d+k_chars)).astype('float64')
    X_bag_of_chars_manu = np.zeros((n,d+k_chars)).astype('float64')
    X_bag_of_chars_seriesDesc = np.zeros((n,d+k_chars)).astype('float64')
    X_bag_of_chars_aquisition = np.zeros((n,d+k_chars)).astype('float64')
    X_bag_of_chars_bodyPart = np.zeros((n,d+k_chars)).astype('float64')

    for i in range(n):
        for j in range(k_chars):    
            try:
                #X_bag_of_chars_CB[i, j+d] = X_CB_agent[i].lower().count(chars[j])
                X_bag_of_chars_manu[i,j+d] = X_manufacturer[i].lower().count(chars[j])
                X_bag_of_chars_seriesDesc[i,j+d] = X_series_desc[i].lower().count(chars[j])
                X_bag_of_chars_aquisition[i,j+d] = X_MRI_aqquisition[i].lower().count(chars[j])
                #X_bag_of_chars_bodyPart[i,j+d] = X_bodyPart_Examined[i].lower().count(chars[j])
                pass
            except:
                continue
    X_bag_of_chars = X + X_bag_of_chars_manu + X_bag_of_chars_seriesDesc


#Splits data into sets for training and testing
    X_train, X_test, y_train, y_test = train_test_split(X_bag_of_chars, y, test_size=0.2, random_state=42)

    print('training...')
    clf=RandomForestClassifier(max_depth=None,random_state=0,n_estimators=100)
    clf.fit(X_train,y_train)
    y_predTest = clf.predict(X_test)
    print(y_predTest)
    letter = chr(int(y_predTest[0]) + 64)
    print(list(seqDict.keys())[list(seqDict.values()).index(letter)])


    #save model
    
    print("SAVING MODEL 2")
    dump(clf, 'RFModelNewParams.joblib') 
    print("RESULTS")
    print(clf.score(X_train,y_train)*100)
    print(clf.score(X_test,y_test)*100)
    #my_df = pd.DataFrame(X_bag_of_chars)
    #my_df.to_csv('XbagofChars_oldDatasetparams.csv', index=False, header=False)

    y_predTest = clf.predict(X_test)
    matrix = confusion_matrix(y_test, y_predTest)
    matrix = matrix.astype('float') / matrix.sum(axis=1)[:,np.newaxis]

    # Build the Confusion Matrix
    plt.figure(figsize=(16,7))
    sns.set(font_scale=1.4)
    sns.heatmap(matrix, annot=True, annot_kws={'size':10},
                cmap=plt.cm.Greens, linewidths=0.2)

    # Add labels to the plot
   
    class_names = RFdata['sequenceLabel'].unique()
    tick_marks = np.arange(len(class_names))
    tick_marks2 = tick_marks + 0.5
    plt.xticks(tick_marks, class_names, rotation=25)
    plt.yticks(tick_marks2, class_names, rotation=0)
    plt.xlabel('Predicted label')
    plt.ylabel('True label')
    plt.title('Confusion Matrix for Random Forest Model')
    #plt.show()
    plt.savefig("ConfusionMatrixNew.jpg")




randomForestNew()