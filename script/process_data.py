import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.optimize import minimize
from scipy.special import expit
from scipy.interpolate import interp1d

# sort the data of the scene 'sceneName'
# return a list of list. Each list contains all the patches in this image section (see utility.XYtoID() for order)
# generate the dataset by using process_log() before hand
def sort_data(resultFilePath):
    path="data"
    thresholds=[[] for i in range(16)]
    with open(resultFilePath,"r") as F:
        data = F.readlines()
        for l in data:
            l=l.replace("\n","")
            lSplit=l.split(";")
            
            ID=int(float(lSplit[0]))
            spp=int(float(lSplit[1]))
            detected=int(float(lSplit[2]))
            
            thresholds[ID].append([spp,detected])
    return thresholds

# format data into 2 arrays: dataX(SPP) and dataY(detectedLabel)
def sortDataToXY(resultFilePath):
    result=sort_data(resultFilePath)
    R=[]
    for r in range(16):
        dataX=[]
        dataY=[]
        for i in range(len(result[r])):
            dataX.append(result[r][i][0])
            dataY.append(result[r][i][1])
        R.append((dataX,dataY))
    return R

# sigmoid function
def sigmoid(x):
    #y = 1 / (1 + np.exp(-x))
    y=expit(x)
    return y

# logistic function
def logistic(x,k,x0):
    #y = 1 / (1 + np.exp(k*(x-x0)))
    y = expit((k*(x0-x)))
    return y

# likelihood function for fit_logisticFunction_MLE
# args are [k,x0]
def logistic_likelihood(x,*args):
    THETA = x
    X=np.asarray(args[0],dtype=np.float32)
    Y=np.asarray(args[1],dtype=np.float32)
    S=0
    epsilon = 1e-1
    for i in range(len(args[0])):
        Sig=sigmoid(THETA[0]*(X[i]-THETA[1]))
        logSig=np.log(Sig+epsilon)
        logOneMinusSig=np.log(1.0-Sig+epsilon)
        S=S+((Y[i]*logSig*1) + ((1-Y[i])*logOneMinusSig*1))
    return S

# fits a logistic function to dataX and dataY
# uses Maximood likelihood Estimation
# returns [k,x0]
def fit_logisticFunction_MLE(dataX,dataY):
    # initial guess
    x0 = [1,np.median(dataX)]
    # bounds for k and x0
    l_u_bounds = [(0,2),(1,500)]
    res = minimize(logistic_likelihood,x0,args=(dataX,dataY),bounds=l_u_bounds)
    return res.x

# minus heavyside, centered on x0
# normalized: Norm-2
def heavyside(x0):
    X=np.linspace(1,500,500)
    Y=[]
    for x in X:
        if(x <= x0):
            Y.append(1)
        else:
            Y.append(0)
    Y=np.asarray(Y)
    norm = np.sqrt(np.dot(Y,Y))
    if(norm != 0):
        Y = Y/norm
    return Y

# reconstruct the signal with one iteration of Matching Pursuit (function base/ heavyside)
def reconstruct_MP(dataX,dataY):
    maxID = 1
    maxDot = 0

    dataX=np.asarray(dataX)
    dataY=np.asarray(dataY)

    dataX=np.append(dataX,[1,500],axis=0)
    dataY=np.append(dataY,[1,0],axis=0)

    #sort data
    argSort = np.argsort(dataX)
    dataX=np.sort(dataX)
    dataY=np.take_along_axis(dataY,argSort,axis=0)
    
    for i in range(1,500):
        H=np.asarray(heavyside(i))


        # interpolate
        X=np.linspace(1,500,500)
        Y=interp1d(dataX,dataY,kind="next")(X)

        # normalize
        normY = np.sqrt(np.dot(Y,Y))
        if(normY != 0):
            Y = Y/normY

        # projection
        dot = np.dot(H,Y)
        
        if(dot>maxDot):
            maxDot=dot
            maxID=i
    return maxID

def reconstruct_MP_DEBUG(dataX,dataY):
    maxID = 1
    maxDot = 0

    dataX=np.asarray(dataX)
    dataY=np.asarray(dataY)

    dataX=np.append(dataX,[1,500],axis=0)
    dataY=np.append(dataY,[1,0],axis=0)

    #sort data
    argSort = np.argsort(dataX)
    dataX=np.sort(dataX)
    dataY=np.take_along_axis(dataY,argSort,axis=0)
    X=np.linspace(1,500,500)
    #Y=interpolate_next(X,dataX,dataY)
    # normalize
    #print(Y)
    for i in range(1,500):
        H=np.asarray(heavyside(i))

        Y=interp1d(dataX,dataY,kind="next")(X)
        #Y=interpolate_next(X,dataX,dataY)
        # normalize
        normY = np.sqrt(np.dot(Y,Y))
        if(normY != 0):
            Y = Y/normY

        # projection
        dot = np.dot(H,Y)
        
        if(dot>maxDot):
            maxDot=dot
            maxID=i
    #Y=np.interp(X,dataX,dataY)
    H=np.asarray(heavyside(maxID))
    plt.plot(dataX,dataY,"k.")
    plt.plot(X,Y/max(Y),"--b")
    H=np.asarray(heavyside(maxID))
    plt.plot(X,H,"--r")
    return maxID
