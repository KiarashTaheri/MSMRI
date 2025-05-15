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
    def __init__(self):
        self.moduleRF = load('RFModelNewParams.joblib')

    def myPredictK(self, echo_Time, repetition_Time, Manufacturer, series_description, bolus_agent):    
        if len(bolus_agent) == 0 or bolus_agent == "NO" or bolus_agent.isspace() :
            bolusValue = 0
        else:
            bolusValue = 1

        X = np.array([echo_Time, repetition_Time, bolusValue])
        n = X.shape
        X = X.astype('float64')
        print(X)
        print(n)
        n=3

        chars = list(map(chr, range(97, 123))) + ['0','1','2','3','4','5','6','7','8','9']
        print(chars)
        k_chars = len(chars)

        X_series_desc = np.array([series_description])
        X_manufacturer = np.array([Manufacturer])


        X_bag_of_chars_manu = np.zeros((1,3+k_chars)).astype('float64')
        X_bag_of_chars_seriesDesc = np.zeros((1,3+k_chars)).astype('float64')
        X_bag_of_chars_seriesDesc = np.zeros((1,3+k_chars)).astype('float64')
        X_bag_of_chars = np.zeros((1,3+k_chars)).astype('float64')
        for i in range(n):
            X_bag_of_chars[0][i] = X[i]
        for j in range(k_chars):    
            try:
                X_bag_of_chars_seriesDesc[0,j+3] = X_series_desc[0].lower().count(chars[j])
                X_bag_of_chars_manu[0,j+3] = X_manufacturer[0].lower().count(chars[j])
                pass
            except:
                continue
        X_bag_of_chars = X_bag_of_chars_manu + X_bag_of_chars_seriesDesc
        yPred = self.moduleRF.predict(X_bag_of_chars)
        
        convertCols = ['Sequence', 'letter']
        seqDataCovert = pd.read_csv('DB_sequenceTypes21.csv',names=convertCols)
        seqDict = dict(zip(seqDataCovert['letter'], seqDataCovert['Sequence']))
        yLetter = chr(int(yPred[0]) + 64)
        print(yLetter)
        yClass = seqDict.get(yLetter)

        dictTitle = ['Description', 'Number']
        letterToNumber = pd.read_csv('dictionarySeriesDescription-Number.csv', names=dictTitle)
        LettterDict = dict(zip(letterToNumber['Description'], letterToNumber['Number']))
        yNumber = LettterDict.get(yClass)
        print(yNumber)
        print(yPred[0])
        print(yClass)
        return yNumber

def main():
    echoTime = int(sys.argv[1])
    repetitionTime = int(sys.argv[2])
    Manufacturer = sys.argv[3]
    seriesdescription = sys.argv[4]
    bolusagent = sys.argv[5]


    predictor = MRI_Image_Predictor()
    result = predictor.myPredictK(echo_Time=echoTime, repetition_Time=repetitionTime, Manufacturer=Manufacturer, series_description=seriesdescription, bolus_agent=bolusagent )
    

main()