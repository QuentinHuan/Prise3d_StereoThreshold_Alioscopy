# -*- coding: utf-8 -*-
"""
Created on Wed Jun  9 17:29:51 2021

@author: Quentin

# arguments:
# in terminal: "python3 ComputeNewStimulusSet.py sceneName"

# description
# print next stimuli values (in [1,500]^16) for scene sceneName
# values are coma separated
# use the results files in ./data to compute the new value 
"""

import MLE_Stimulus as MLE
import process_data as pd
import sys
import numpy as np
import os

def get_script_path():
    return os.path.dirname(os.path.realpath(sys.argv[0]))
wdir=get_script_path()

sceneName = sys.argv[1]
# results contains dataX and dataY for each block
results = pd.sortDataToXY(wdir+"/data/"+sceneName+"_results.log") # ==> [(dataX_0,dataY_0);...;(dataX_15,dataY_15)]
stimulus=""
# for each image block, compute a new stimulus value
for i in range(16):
    dataX = np.asarray(results[i][0],np.float32)
    dataY = np.asarray(results[i][1],np.float32)
    stimulus=stimulus+str(MLE.next_stimulus_MLE(dataX,dataY))
    if i!=15: stimulus=stimulus+"," # add coma, exept for last one
print(stimulus)

