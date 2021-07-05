# -*- coding: utf-8 -*-
"""
Created on Thu Apr 15 11:43:03 2021

@author: Quentin
"""
import process_log as pl
import utility as u
import os
import time
import shutil
import errno
import glob

import process_data as pd
import Output as out

# process all log files located in logPath
# save the dataset in ./data
def process_all_log(logPath):
    
    print("##########################")
    print("  begin data processing   ")
    print("##########################")
    path=logPath
    # list all .log files in PATH
    logs=u.listFiles(path,[".log"])
    print(str(len(logs))+" .log files detected in " + path)
    
    # remove old files
    files = glob.glob('./results/*')
    for f in files:
        os.remove(f)
       
    # ------------------------------------
    # clean all logs file (remove all the irrelevant UE4 system logs)
    # ------------------------------------
    for f in logs:   
        pl.prune_logFile(path,f)
    
    print("cleaning all files done")
    print("")
    path="data"
    
    # list all .log files in ./data
    logs=u.listFiles(path,["prune_"])
    print(str(len(logs))+" clean .log files detected in "+ path)
    
    # ------------------------------------
    # split log files by scene
    # ------------------------------------
    for f in logs: 
        pl.split_logFile(path,f)
    
    print("split all files by scene done")
    
    # ------------------------------------
    # analyze each files
    # ------------------------------------
    logs=u.listFiles(path,[".log","_2"])
    print(str(len(logs))+" files in "+ path)
    i=0
    for f in logs: 
        pl.analyze_logFile(path,f)
        i=i+1
        print("file annalysis : "+str(i)+"/"+str(len(logs)))
    
    print("-------------------------")
    print("file annalysis done   ")
    print("-------------------------")
    
    # ------------------------------------
    # get scenelist
    # ------------------------------------
    logs=u.listFiles(path,[".log"])
    sceneList=[]
    for f in logs:
        name = f.split("_20")[0] 
        if name not in sceneList:
            sceneList.append(name)
    
    # ------------------------------------
    # merge files by scene
    # ------------------------------------
    logs=u.listFiles(path,[".log","results"])
    pl.merge_logFile(path,sceneList,1)

    logs=u.listFiles(path,[".log"])
    for f in logs:
        print("generated: " + f)


# process one log files located in logPath, whose name is logName
# ex: E:/logFolder/mylog.log
# logPath = E:/logFolder
# logName = mylog.log
#
# add the results to the dataset in ./data
def process_one_log(logPath,logName):
    
    print("##########################")
    print("  begin data processing   ")
    print("##########################")
    path=logPath

    # ------------------------------------
    # clean all logs file (remove all the irrelevant UE4 system logs)
    # ------------------------------------
    pl.prune_logFile(path,logName)
    
    print("cleaning file done")
    print("")
    path="data"
    
    # list all .log files in ./logs
    logs=u.listFiles(path,["prune_"])
    print(str(len(logs))+" clean .log files detected in "+ path)
    
    # ------------------------------------
    # split log files by scene
    # ------------------------------------
    for f in logs: 
        pl.split_logFile(path,f)
    
    print("split all files by scene done")
    
    # ------------------------------------
    # analyze each files
    # ------------------------------------
    logs=u.listFiles(path,[".log"])
    print(str(len(logs))+" files in "+ path)
    i=0
    for f in logs: 
        if("results" in f):
            print(f+"is result file, skip...")
        else:
            pl.analyze_logFile(path,f)
            i=i+1
            print("file annalysis : "+str(i)+"/"+str(len(logs)))
    
    print("-------------------------")
    print("file annalysis done   ")
    print("-------------------------")
    
    # ------------------------------------
    # get scenelist
    # ------------------------------------
    logs=u.listFiles(path,[".log","_20"])
    sceneList=[]
    for f in logs:
        name = f.split("_20")[0] 
        if name not in sceneList:
            sceneList.append(name)
    
    logs=u.listFiles(path,[".log","results"])
    sceneList=[]
    for f in logs:
        name = f.split("_results")[0] 
        if name not in sceneList:
            sceneList.append(name)
            
    print("detected scenes:")
    print("")
    print(sceneList)
    print("")
    # ------------------------------------
    # merge files by scene
    # ------------------------------------
    logs=u.listFiles(path,[".log","results"])
    pl.merge_logFile(path,sceneList,0)

    logs=u.listFiles(path,[".log","results"])
    for f in logs:
        print("generated: " + f)

#process_one_log("E:\git\Prise3D_StereoThreshold\ExperimentData\Resultats 20052021\exclude","P3d_Expe1.log")
#process_all_log("E:\git\Prise3D_StereoThreshold\ExperimentData\Resultats 20052021")

#scenes=["arcsphere","contemporary-bathroom","caustic-view0","p3d_crown_results","p3d_indirect_results"]
#
#path="data/p3d_arcsphere_results.log"
#path="data/p3d_contemporary-bathroom_results.log"
##path="data/p3d_caustic-view0_results.log"
#path="data/p3d_crown_results.log"
#path="data/p3d_indirect_results.log"
#show_thresholdImage(path,"../../../image",True)

#out.showResult(path,False)
#out.showResult(path,True)

#out.show_thresholdImage(path,"../../../image",True,False)
#out.show_thresholdImage(path,"../../../image",True,True)
#out.plt.show()



