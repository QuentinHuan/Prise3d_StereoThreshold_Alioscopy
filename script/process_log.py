import matplotlib.pyplot as plt
import numpy as np
import fileinput
import os
import utility as u
import sys

#distance function
def phi(blockPos, gazePos, size):
    out = []
    
    for i in range(size):
        v = blockPos[i,:] - gazePos[i,:]
        r = np.max(np.abs(v))
        out.append(r)

    return np.asarray(out).astype(np.float64)


# removes all the irrelevant text from the log file
# save the clean file in ./logs/prune_filename.log
# path is the path to the source log files
def prune_logFile(path,fileName):
    file = open(path+"/"+fileName, 'r')
    Lines = file.readlines()
    
    outFile = open("./data/prune_"+fileName,"w")
    
    for l in Lines:
        if( (l[0] != "#") and ("p3d:" in l) and ("p3d:FIN" not in l) ):
            outFile.write(l.split("p3d:")[1])
    outFile.close()
    file.close()


# should be executed after prune_logFile()
# split the file by scenes: prune_file.log ==> scene1_file.log, scene2_file.log ...
# each line is : timestamp;patchPos.X patchPos.Y;gazePos.X gazePos.Y;spp;detected
def split_logFile(path,fileName):
    #check correct file
    if("prune_" not in fileName):
        print("incorrect file: "+fileName)
        print("split_logFile() should be executed after prune_logFile()")
        
    else:
        file = open(path+"/"+fileName, 'r')
        Lines = file.readlines()
        fileName_striped = fileName.replace("prune_","") #remove prune_
        fileName_striped = fileName_striped.replace("P3d_Expe1","") #remove P3d_Expe1
        fileName_striped = fileName_striped.replace("-backup-","") #remove 
        scene = "NULL"
        for l in Lines:
            l_split = l.split(";")
            #detect scene change: create scene_filename.log
            if( l_split[0] != scene):
                if(scene !="NULL"):
                    outFile.close()
                scene = l_split[0]
                outFile = open(path+"/"+scene+"_"+fileName_striped,"w")
            #rewrite without the scene name at the begining of each line
            outFile.write(l.replace(l_split[0]+";",""))
            
        if len(Lines)>0:
            outFile.write("-1;X=-1 Y=-1;X=-1 Y=-1;-1;0")
            outFile.close()
        file.close()
        os.remove(path+"/"+fileName)

# should be executed after split_logFile()
# for each patch, write down in the file: cellID;spp;bDetected(1 or 0)
def analyze_logFile(path,fileName):
    file = open(path+"/"+fileName, 'r')
    Lines = file.readlines()
    
    # ARRAYS
    T = [] # timestamps
    blockPos = []
    gazePos = []
    spp = [] 
    detected = [] # vive controller trigger press (1 if pressed, 0 otherwise)
    
    # DATA EXTRACTION
    for l in Lines:
        #split on ";"
        split = l.split(";")
        t = split[0]
        
        patchPos =  split[1].split(" ")
        patchX = patchPos[0].split("=")[1]
        patchY = patchPos[1].split("=")[1]

        gaze = split[2].split(" ")
        gazeX = gaze[0].split("=")[1]
        gazeY = gaze[1].split("=")[1]

        detect = int(split[4])

        T.append(t)
        blockPos.append((patchX,patchY))
        gazePos.append((gazeX,gazeY))
        spp.append(split[3])
        detected.append(detect)
    # convert to array
    blockPos = np.asarray(blockPos).astype(np.float64)
    gazePos = np.asarray(gazePos).astype(np.float64)
    T = np.asarray(T).astype(np.float64)
    spp = np.asarray(spp).astype(np.float64)
    PHI = phi(blockPos,gazePos,blockPos.shape[0])
    detected = np.asarray(detected).astype(np.float64)
    
    # DATA PROCESSING
    R=[]
    previousSpp=0
    previousID=0
    bDetected=0
    score=0
    for i in range(T.size):
        #detect block change: save result
        if( u.XYtoID(blockPos[i]) != previousID or spp[i] != previousSpp ):
            if score > (0.5/0.01): #detected for more than 0.5 second
                bDetected=1
            if (15 >= previousID and previousID >= 0 and i>0):
                R.append(str(previousID)+";"+str(previousSpp)+";"+str(bDetected)+"\n")
            bDetected=0
            score=0
        else:
            if PHI[i]<=0.25 and detected[i]==1:
                score=score+1
                
        previousSpp=spp[i]
        previousID=u.XYtoID(blockPos[i])
        
    
    # save into file
    file.close()
    file = open(path+"/"+fileName, 'w')#erase file
    file.writelines(R)
    file.close()
    return 0

# should be executed after analyze_logFile()
# for each scene, merge all results in a single file named scene_results.log
# delete all intermediate files
def merge_logFile(path,sceneList,bOverride):
    mode={0:"a" , 1:"w"}
    for s in sceneList:
        filename =path+"/"+s+"_results.log"
        F = open(filename,mode[bOverride])
        files=u.listFiles(path,[s])
        for f in files:
            if("results" in f):
                print(f+"is result file, skip merge...")
            else:
                splitFile = open(path+"/"+f,"r")
                F.writelines(splitFile.readlines())
                splitFile.close()
                os.remove(path+"/"+f)
        F.close()
