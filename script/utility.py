# -*- coding: utf-8 -*-
"""
Created on Thu Apr 15 11:12:09 2021

@author: Quentin
"""
import sys
import os
import numpy as np

# return a list of the name of all the files in path
# with all the strings in lookFor in their name
def listFiles(path,lookFor):
    fileList = os.listdir(path)
    matching=[]
    for f in fileList:
        condition=1
        for i in range(len(lookFor)):
            if lookFor[i] not in f:
                condition=condition*0
        if condition==1:
            matching.append(f)
    return matching
            

# convert (X,Y) position (0.0 to 1.0) into 0-15 id (represents a 4x4 grid)
#
#    0-------------------> [x=1]
#    | ------------------
#    | | 0 | 1 | 2 | 3  |
#    | ------------------
#    | | 4 | 5 | 6 | 7  |
#    | ------------------
#    | | 8 | 9 |10 |11  |
#    | ------------------
#    | |12 |13 |14 |15  |
#    | ------------------
#    V [y=1]
def XYtoID(position):
    x=position[0]
    y=position[1]
    
    x=np.ceil((x+0.05)*4 -1)
    y=np.ceil((y+0.05)*4 -1)
    
    
    return np.abs(x+(4*y))
            

# %%
