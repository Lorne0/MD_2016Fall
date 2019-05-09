import sys
import numpy as np
from numpy import dot
import cPickle as pk
#############
def readMatrix(fname, n, m):
    R = np.zeros((n, m))
    W = np.zeros((n, m))
    with open(fname) as f:
        for line in f:
            [a, b, c] = line.strip().split()
            a = int(a) 
            b = int(b)
            R[a][b] = float(c)
            W[a][b] = 1
    return R, W
##############

N = 50000
M = 5000
K = 250

#=======================================================================#
if sys.argv[1]=='saveX':
    model_name = "model"
    with open(model_name) as f:
        P = []
        Q = []
        for (i, line) in enumerate(f):
            if i==4:
                mean = float(line.strip().split()[1])
            elif i >= N + 5:
                data = line.strip().split()
                if data[1] == 'T':
                    Q.append(list(map(float, data[2:])))
                else:
                    Q.append([ np.nan ] * K)
            elif i >= 5:
                data = line.strip().split()
                if data[1] == 'T':
                    P.append(list(map(float, data[2:])))
                else:
                    P.append([ np.nan ] * K)
    P = np.array(P)
    Q = np.array(Q)
    X = dot(P, Q.T)
    nan_idxs = np.isnan(X)
    X[nan_idxs] = mean
    with open('srcR','wb') as fp:
        pk.dump(X,fp, protocol=pk.HIGHEST_PROTOCOL)
#=======================================================================#

#=======================================================================#
elif sys.argv[1]=='init':
    with open('srcR', 'rb') as fp:
        X = pk.load(fp)
    #F = np.random.rand(N,K)+0.2
    #G = np.random.rand(M,K)+0.2
    #S = np.random.rand(K,K)+0.2
    
    a = np.random.randn(N,K)
    F,_ = np.linalg.qr(a)
    a = np.random.randn(M,K)
    G,_ = np.linalg.qr(a)

    S = dot(dot(F.T, X),G)

    with open('F', 'wb') as fp:
        pk.dump(F,fp, protocol=pk.HIGHEST_PROTOCOL)
    with open('S', 'wb') as fp:
        pk.dump(S,fp, protocol=pk.HIGHEST_PROTOCOL)
    with open('G', 'wb') as fp:
        pk.dump(G,fp, protocol=pk.HIGHEST_PROTOCOL)
#=======================================================================#

#=======================================================================#
elif sys.argv[1]=='AL':
    with open('srcR', 'rb') as fp:
        X = pk.load(fp)
    with open('G', 'rb') as fp:
        G = pk.load(fp)
    with open('F', 'rb') as fp:
        F = pk.load(fp)
    with open('S', 'rb') as fp:
        S = pk.load(fp)

    for t in range(2):
        tmp1 = dot(dot(X.T, F), S)
        tmp2 = dot(dot(dot(dot(G, G.T), X.T), F), S)
        G = G*tmp1/tmp2
        tmp1 = dot(dot(X, G), S.T)
        tmp2 = dot(dot(dot(dot(F, F.T), X), G), S.T)
        F = F*tmp1/tmp2
        tmp1 = dot(dot(F.T, X), G)
        tmp2 = dot(dot(dot(dot(F.T, F), S), G.T), G)
        S = S*tmp1/tmp2
        #RMSE
        E = (X-dot(dot(F, S), G.T))**2
        e = np.sqrt(np.mean(E))
        print("RMSE: %f" % e)

    #save G, F, S
    with open('G', 'wb') as fp:
        pk.dump(G,fp, protocol=pk.HIGHEST_PROTOCOL)
    with open('F', 'wb') as fp:
        pk.dump(F,fp, protocol=pk.HIGHEST_PROTOCOL)
    with open('S', 'wb') as fp:
        pk.dump(S,fp, protocol=pk.HIGHEST_PROTOCOL)
    #codebook
    U = np.zeros((N,K))
    V = np.zeros((M,K))
    f = np.argmax(F,axis=1)
    for i in range(N):
        U[i][f[i]] = 1
    f = np.argmax(G,axis=1)
    for i in range(M):
        V[i][f[i]] = 1
    B = (dot(dot(U.T, X), V)+1) / (dot(dot(U.T, np.ones((N,M))), V)+1)
    with open('codebook', 'wb') as fp:
        pk.dump(B,fp, protocol=pk.HIGHEST_PROTOCOL)

#=======================================================================#

else:
    with open('srcR', 'rb') as fp:
        X = pk.load(fp)
    with open('G', 'rb') as fp:
        G = pk.load(fp)
    with open('F', 'rb') as fp:
        F = pk.load(fp)
    with open('S', 'rb') as fp:
        S = pk.load(fp)

    if(sys.argv[1]=='G'):
        #update G
        tmp1 = dot(dot(X.T, F), S)
        print("1")
        tmp2 = dot(G, G.T)
        print("2")
        tmp2 = dot(tmp2, X.T)
        print("3")
        tmp2 = dot(tmp2, F)
        print("4")
        tmp2 = dot(tmp2, S)
        print("5")
        #tmp2 = dot(dot(dot(dot(G, G.T), X.T), F), S)
        print("G dot done.")
        _G = G*tmp1/tmp2
        with open('G', 'wb') as fp:
            pk.dump(_G,fp, protocol=pk.HIGHEST_PROTOCOL)
        print("update G done.")
    elif(sys.argv[1]=='F'):
        #update F
        tmp1 = dot(dot(X, G), S.T)
        print("1")
        tmp2 = dot(F, F.T)
        print("2")
        tmp2 = dot(tmp2, X)
        print("3")
        tmp2 = dot(tmp2, G)
        print("4")
        tmp2 = dot(tmp2, S.T)
        print("5")
        #tmp2 = dot(dot(dot(dot(F, F.T), X), G), S.T)
        print("F dot done.")
        _F = F*tmp1/tmp2
        with open('F', 'wb') as fp:
            pk.dump(_F,fp, protocol=pk.HIGHEST_PROTOCOL)
        print("update F done.")
    elif(sys.argv[1]=='S'):
        #update S
        tmp1 = dot(dot(F.T, X), G)
        print("1")
        tmp2 = dot(F.T, F)
        print("2")
        tmp2 = dot(tmp2, S)
        print("3")
        tmp2 = dot(tmp2, G.T)
        print("4")
        tmp2 = dot(tmp2, G)
        print("5")
        #tmp2 = dot(dot(dot(dot(F.T, F), S), G.T), G)
        print("S dot done.")
        _S = S*tmp1/tmp2
        with open('S', 'wb') as fp:
            pk.dump(_S,fp, protocol=pk.HIGHEST_PROTOCOL)
        print("update S done.")
#=======================================================================#

'''
E = (X-dot(dot(F, S), G.T))**2
e = np.sqrt(sum(sum(E))/(N*M))
print("RMSE: %f" % e)

U = np.zeros((N,K))
V = np.zeros((M,K))

f = np.argmax(F,axis=1)
for i in range(N):
    U[i][f[i]] = 1

f = np.argmax(G,axis=1)
for i in range(M):
    V[i][f[i]] = 1

B = dot(dot(U.T, X), V) / dot(dot(U.T, np.ones((N,M))), V)

np.save("codebook", B)

'''
