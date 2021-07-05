# output the results of the analysis
# 
#
import process_data as pd
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.optimize import minimize

# return a list of the perceptive thresholds (refers to the image ID)
# threshold_spp = 20*threshold[ID]
def compute_thresholds(resultFilePath,finalEstimation=False):
    T=[]
    dataX=[]
    dataY=[]
    result=pd.sortDataToXY(resultFilePath)
    print("compute_thresholds for :" + resultFilePath)
    for i in range(16):
        dataX = np.asarray(result[i][0],dtype=np.float32)
        dataY = np.asarray(result[i][1],dtype=np.float32)
        #logistic curve fitting
        params = pd.fit_logisticFunction_MLE(dataX,dataY)
        
        if(finalEstimation==True):
            #MP reconstruction threshold finder
            params = [1,max(pd.reconstruct_MP(dataX,dataY),params[1])]


        #print("cell "+str(i)+":")
        #print("parameters = ")
        
        T.append((params[0],max([int(params[1]),1])))
    print(T)
    return T
    
# show the logistic plots and threshold values
def showResult(resultFilePath,finalEstimation=False):
    print("----------------------------")
    print("showResult :"+resultFilePath)
    result=pd.sortDataToXY(resultFilePath)
    T = compute_thresholds(resultFilePath,finalEstimation)
    print("number of observations per blocks :"+str(len(result[0][0])))
    fig, axes = plt.subplots(4,4, sharex=True, sharey=True)
    fig.suptitle(resultFilePath.replace("data/","").replace("_results.log","")+" --MP="+str(int(finalEstimation)), fontsize=16)
    for i in range(4):
        for j in range(4):
            dataX = np.asarray(result[4*i + j][0],dtype=np.float32)
            dataY = np.asarray(result[4*i + j][1],dtype=np.float32)
            params = T[4*i + j]

            #logistic curve plot
            X = np.linspace(1,501,501)
            Y = pd.logistic(X,params[0],params[1])
            axes[i,j].plot(X,Y,"k",linewidth=1)
            axes[i,j].plot(params[1],0.5,"rx")
            axes[i,j].axvline(params[1],ls="--",color="r",ymin=0,ymax=0.5,linewidth=0.5)
            axes[i,j].set_xlabel("seuil("+str(4*i+j)+")="+str(20*int(np.round(params[1])+1))) 

            #data points plot
            axes[i,j].set_ylim([0,1.05])
            axes[i,j].plot(dataX,dataY,"bx")

    plt.setp(axes[:, 0], ylabel='P_detection(spp)')


# generate an image made of all the threshold images
# bStereo = True to generate the stereo pair
def show_thresholdImage(resultFilePath,imgDataBasePath,bStereo,finalEstimation=False):
    T = compute_thresholds(resultFilePath,finalEstimation)
    suffix="MLE"
    if finalEstimation==True:
        suffix=suffix+"_MP"
    print("----------------------------")
    print("show_thresholdImage for "+resultFilePath)
    sceneName=resultFilePath.replace("data/p3d_","").replace("_results.log","")
    side = "right"
    if(bStereo==True):
        imgOut=Image.new('RGB', (800*2, 800))
    else:
        imgOut=Image.new('RGB', (800, 800))
        
    for i in range(4):
        for j in range(4):
            side="right"
            im_l = Image.open(imgDataBasePath+"/p3d_"+sceneName+"-"+side+"/p3d_"+sceneName+"-"+side+"_"+  str(T[i + 4*j][1]).zfill(5) +".png")
            region_l = im_l.crop((i*200, j*200, (i+1)*200, (j+1)*200))

            imgOut.paste(region_l,(i*200, j*200))
            
            if(bStereo==True):
                side="left"
                im_r = Image.open(imgDataBasePath+"/p3d_"+sceneName+"-"+side+"/p3d_"+sceneName+"-"+side+"_"+  str(T[i + 4*j][1]).zfill(5) +".png")
                region_r = im_r.crop((i*200, j*200, (i+1)*200, (j+1)*200))
    
                imgOut.paste(region_r,(i*200+800, j*200))
    imgOut.save("./img/Thresh_"+sceneName+suffix+".png")
    print("image saved to: "+"./img/Thresh_"+sceneName+suffix+".png")