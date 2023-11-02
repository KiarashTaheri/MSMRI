import sys
import os
import pydicom as pyd
from joblib import load
import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
#from sklearn.model_selection import train_test_split
#from newRF import randomForestNew
#from sklearn.metrics import accuracy_score, confusion_matrix, classification_report

class MRI_Image_Predictor:
    def __init__(self, DICOMpath):
        self.path = pyd.dcmread(os.path.join(DICOMpath))
        echo_Time, repetition_Time, Manufacturer, series_description, bolus_agent = self.path['EchoTime'].value, self.path['RepetitionTime'].value, self.path['Manufacturer'].value, self.path['SeriesDescription'].value, self.path['Contrast/BolusAgent'].value
        moduleRF = load('RFModelNewParams.joblib')

        if len(bolus_agent) == 0 or bolus_agent == "NO" or bolus_agent.isspace() :
            bolusValue = 0
        else:
            bolusValue = 1

        X = np.array[echo_Time, repetition_Time, bolusValue]
        n, d = X.shape()


        chars = list(map(chr, range(97, 123))) + ['0','1','2','3','4','5','6','7','8','9']
        k_chars = len(chars)

        X_series_desc = np.array([series_description])
        X_manufacturer = np.array([Manufacturer])


        X_bag_of_chars_manu = np.zeros((n,d+k_chars)).astype('float64')
        X_bag_of_chars_seriesDesc = np.zeros((n,d+k_chars)).astype('float64')

        for j in range(k_chars):    
            try:
                X_bag_of_chars_seriesDesc[0,j+d] = X_series_desc[0].lower().count(chars[j])
                X_bag_of_chars_manu[0,j+d] = X_manufacturer[0].lower().count(chars[j])
                pass
            except:
                continue
        X_bag_of_chars = X + X_bag_of_chars_manu + X_bag_of_chars_seriesDesc
        yPred = moduleRF.predict(X_bag_of_chars)
        
        convertCols = ['Sequence', 'letter']
        seqDataCovert = pd.read_csv('DB_sequenceTypes21.csv',names=convertCols)
        seqDict = dict(zip(seqDataCovert['letter'], seqDataCovert['Sequence']))
        yLetter = chr(int(yPred[0]) + 64)
        yClass = seqDict.get(yLetter)
        print(yClass)
        return yClass
     