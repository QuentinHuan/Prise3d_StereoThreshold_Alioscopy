from time import sleep
from numpy.random import random
import process_data as pd
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import sys

# simulate a stimulus response
# at spp sample
# response follows a logistic probability distribution of parameters (k,x0)
# ajust p_error to add some random acquisition errors
def newObservation(spp,k,x0):
    p_error = 0.30 #bit switching error probability
    p=pd.sigmoid(-k*(spp-x0))

    obs=np.random.binomial(1, p, 1)[0]
	
    if obs==1:
        e=np.random.binomial(1, p_error, 1)[0] # error flag
        if(e==1):
            obs=0
    return obs

# simulate the whole MLE threshold seeking procedure
# x0_ref and k_ref the real values of the parameters to estimate
# N the amount of step to simulate
def MLE_simulation(x0_ref,k_ref,N):
    X=[250]
    Y=[]
    X0_estimated=0
    k_estimated=0
    dataX=[]
    dataY=[]
    for i in range(N):
        # simulate test result
        obs = newObservation(X[i],k_ref,x0_ref)
        Y.append(obs)

        # convert to float 32
        dataX=np.asarray(X,dtype=np.float32)
        dataY=np.asarray(Y,dtype=np.float32)

        # fit new logistic curve to data
        params = pd.fit_logisticFunction_MLE(dataX[0:i+1],dataY)
        X0_estimated=params[1]
        k_estimated=params[0]

        # next stimulus will be at estimated threshold
        X.append(int(X0_estimated))

    return [X0_estimated,k_estimated,dataX,dataY]


# launch a batch of MLE threshold seeking procedure to evaluate precision
# B is the amount of procedures to simulate
# N is the number of observation per procedure
def test_MLE_procedure(B,N,bShowPlots):

    dataX=[]
    dataY=[]
    ERROR_X0_MLE=[]
    ERROR_X0_Rec=[]
    ERROR_K_MLE=[]
    print("simulation running...")
    for i in range(B):
        SPP=np.random.randint(1,500,(N))
        k_ref=5*np.random.random()
        x0_ref=np.random.randint(3,500)

        x0_estimated, k_estimated,dataX ,dataY = MLE_simulation(x0_ref,k_ref,N)

        # Matching Pursuit reconstruction
        x0_estimated_MP = pd.reconstruct_MP(dataX,dataY)

        ERROR_X0_MLE.append((int(x0_estimated)-x0_ref))
        ERROR_X0_Rec.append((int(x0_estimated_MP)-x0_ref))
        ERROR_K_MLE.append((k_estimated-k_ref))
        
        sys.stdout.write((str((int((i/B)*100)))+ "%\r"))

    print("-----------------------------------")
    print("RESULTS : ")
    print("-----------------------------------")
    print("number of simulations = "+str(B))
    print("number of observations per simulations = "+str(N))
    print("")
    print("mean X0 signed error = "+str(np.mean(np.asarray(ERROR_X0_MLE))))
    print("mean X0 absolute error = "+str(np.mean(abs(np.asarray(ERROR_X0_MLE)))))
    #print("median X0 error = "+str(np.median(ERROR_X0_MLE)))
    print("")
    print("mean reconstructed X0 error = " + str(np.mean(np.asarray(ERROR_X0_Rec))))
    print("mean reconstructed X0 absolute error = " + str(np.mean(abs(np.asarray(ERROR_X0_Rec)))))
    print("-----------------------------------")
    
    if(bShowPlots==True):
        fig, axs = plt.subplots(2,1)
        fig.suptitle("estimation avec MLE (rouge), reconstruction avec MP (bleu)")
        # fit and data
        X=np.linspace(1,500,500)

        Y_fit=pd.sigmoid(-k_ref*(X-x0_ref))
        axs[0].plot(X,Y_fit,"k")

        for i in range(N):
            Y_fit=pd.sigmoid(-k_estimated*(X-int(x0_estimated)))
            axs[0].plot(X,Y_fit,"--",color=(1,0,0,0.1))
            axs[0].plot(dataX,dataY,"kx")
            #if(i==N-1):
                #axs[0].plot(X,Y_fit,"--",color=(1,0,0,1))
                
            H=pd.heavyside(x0_estimated_MP)
            axs[0].plot(X,H/np.max(H),"--",color=(0,0,1,0.1))
            #plt.pause(0.1) # uncomment for animation
        
        axs[1].plot(ERROR_X0_MLE,"k.")
        axs[1].plot(ERROR_X0_Rec,"bx")

        plt.show()
    return np.mean(ERROR_X0_MLE)

# generate next stimulus given dataX and dataY
def next_stimulus_MLE(dataX,dataY):
    # experiment beginning: first stimulus is in sampling space center
    if len(dataX)==0:
        return 250
    else:
        # make sure it's a float32 array
        dataX=np.asarray(dataX,dtype=np.float32)
        dataY=np.asarray(dataY,dtype=np.float32)
        # fit new logistic curve to data
        params = pd.fit_logisticFunction_MLE(dataX,dataY)
        X0_estimated=int(params[1])

        return X0_estimated




#test_MLE_procedure(1,15,True)

#B=200
#L=[10,20,30,40,50]
#result=[]
#for N in L:
#    result.append(test_MLE_procedure(B,N,False))
#
#fig, axs = plt.subplots(1,1)
#axs.plot(L,20*np.asarray(result),"k")
#axs.set_xlabel("number of observations")
#axs.set_ylabel("threshold estimation precision (in spp)")
#plt.show()
