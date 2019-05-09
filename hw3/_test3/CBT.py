import sys
import numpy as np
import cPickle as pk

#=================================================#
def readMatrix(fname, n, m):
    R = np.zeros((n, m))
    for i in range(n):
        for j in range(m):
            R[i][j]=np.nan
    W = np.zeros((n, m))
    with open(fname) as f:
        for line in f:
            [a, b, c] = line.strip().split()
            a = int(a) 
            b = int(b)
            R[a][b] = float(c)
            W[a][b] = 1
    return R, W
#=================================================#
def loadpk(fn):
    with open(fn,"rb") as fp:
        return pk.load(fp)
#=================================================#

N = 500
M = 1000
K = 200

#=================================================#

B = loadpk("codebook")

if(len(sys.argv)==2): #CV
    print("Cross Validation")
    cv=int(sys.argv[1])
    tgtR, W = readMatrix(str(cv)+".train", N, M)
else: #do test
    print("Generate Test Result")
    tgtR, W = readMatrix("train.txt", N, M)

tgtR_mean = np.nanmean(tgtR, axis=1)
tgtR_mean[np.isnan(tgtR_mean)] = 0
tgtR_std = np.nanstd(tgtR, axis=1)
tgtR_std[np.isnan(tgtR_std)] = 1
tgtR_std[tgtR_std==0] = 1
tgtR = (tgtR-tgtR_mean.reshape(-1,1)) / tgtR_std.reshape(-1,1)


tgtR[np.isnan(tgtR)] = 0

U = np.zeros((N, K))
V = np.zeros((M, K))
for i in range(M):
    j = np.random.choice(range(K))
    V[i][j] = 1

One = np.ones((N,M))
for t in range(5):
    print("iter = %d" % t)

    ############
    tmp = np.dot(B, V.T)
    for i in range(N):
        #j = np.argmin(np.sum(W[i] * ((tgtR[i] - tmp)**2), axis=1))
        j = np.argmin( np.dot( (tgtR[i]-tmp)**2, W[i] ) )
        U[i] = np.zeros(K)
        U[i][j] = 1
    print("step1 done.")
    ############

    ############
    tmp = np.dot(U, B)
    tmp = tmp.T
    for i in range(M):
        #j = np.argmin(np.sum(W.T[i] * ((tgtR.T[i] - tmp.T)**2), axis=1))
        j = np.argmin( np.dot( (tgtR[:,i]-tmp)**2, W[:,i] ) )
        V[i] = np.zeros(K)
        V[i][j] = 1
    print("step2 done.")
    ############

    if(len(sys.argv)==2): #CV
        Ans = W*tgtR + (One-W)*np.dot(np.dot(U,B),V.T)
        Ans = Ans*tgtR_std.reshape(-1,1) + tgtR_mean.reshape(-1,1)
        count=0.0
        total=0.0
        with open(str(cv)+".test") as fp:
            for line in fp:
                [a,b,c] = line.strip().split()
                a=int(a)
                b=int(b)
                c=float(c)
                count+=(c-Ans[a][b])**2
                total+=1
        print("CV:%d, Iter:%d, RMSE:%lf" % (cv,t,(count/total)**0.5))

if(len(sys.argv)==1): #do test
    Ans = W*tgtR + (One-W)*np.dot(np.dot(U,B),V.T)
    with open("test.txt") as fp:
        with open("result.txt",'w') as fw:
            for line in fp:
                [a,b,c] = line.strip().split()
                a=int(a)
                b=int(b)
                c=Ans[a][b]
                s=str(a)+' '+str(b)+' '+str(c)+'\n'
                fw.write(s)




#=================================================#
