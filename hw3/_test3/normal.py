import numpy as np
import cPickle as pk

def readMatrix(fname, n, m):
    R = np.zeros((n, m))
    for i in range(n):
        for j in range(m):
            R[i][j] = np.nan
    with open(fname) as f:
        for line in f:
            [a, b, c] = line.strip().split()
            a = int(a) 
            b = int(b)
            R[a][b] = float(c)
    return R

N = 500
M = 500
K = 8

A = readMatrix("source.txt", N, M)

b = np.nanmean(A, axis=1)
b[np.isnan(b)] = 0.0
c = np.nanstd(A, axis=1)
c[np.isnan(c)] = 1.0
c[c==0.0] = 1.0
nA = (A-b.reshape(-1,1))/(c.reshape(-1,1))

with open("source_n.pk","wb") as fp:
    pk.dump(b, fp, protocol=pk.HIGHEST_PROTOCOL)
    pk.dump(c, fp, protocol=pk.HIGHEST_PROTOCOL)

with open("source_n.txt","wb") as fp:
    for i in range(N):
        for j in range(M):
            if np.isnan(nA[i][j]) ==  False:
                s = str(i)+' '+str(j)+' '+str(nA[i][j])+'\n'
                fp.write(s)

