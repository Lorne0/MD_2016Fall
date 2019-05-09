import numpy as np
import time

stime = time.time()

dic = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9',' ']

with open('test.txt') as fp:
    for f in fp:
        test = f
print len(test)
#test = test[:10000]

encode = {}
with open('encode.txt') as fp:
    for f in fp:
        f = f.strip('\n')
        s = f.split(' ')
        if float(s[-1])!=0:
            encode[(f[0],f[2])] = np.log(float(s[-1]))
        else:
            encode[(f[0],f[2])] = -1000000000

bigram = {}
with open('bigram.txt') as fp:
    for f in fp:
        f = f.strip('\n')
        s = f.split(' ')
        bigram[(f[0],f[2])] = np.log(float(s[-1]))

for i in dic:
    for j in dic:
        if (i,j) not in bigram:
            bigram[(i,j)] = -1000000000



dp = np.zeros((37,len(test)))
track = np.zeros((37,len(test)))
dp[:,0] = np.log(1/37.0)
cnt = 1
maxlist = np.zeros(37)
for w in test:
    if cnt%100000==0:
        print cnt
    if cnt==len(test):
        break
    for i in range(37):
        for j in range(37):
            maxlist[j] = dp[j][cnt-1] + bigram[(dic[j],dic[i])] + encode[(dic[j],w)]

        dp[i][cnt] = np.max(maxlist)
        track[i][cnt] = int(np.argmax(maxlist))
    cnt+=1

cnt = len(test)-1
k = np.argmax(dp[:,cnt])
pred = dic[k]
while cnt!=0:
    k = int(track[k][cnt])
    pred = dic[k] + pred
    cnt-=1


print len(pred)
with open('pred.txt','w+') as fp:
    fp.write(pred)

print 'Time: '+str(time.time()-stime)+'s'






