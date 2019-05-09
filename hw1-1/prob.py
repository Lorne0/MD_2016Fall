import numpy as np
dic = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9',' ']

encode = {}
with open('encode.txt') as fp:
    for f in fp:
        f = f.strip('\n')
        s = f.split(' ')
        if float(s[-1])!=0 or float(s[-1])!=-0.0:
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

with open('encode_prob','w+') as fp:
    for i in range(37):
        for j in range(37):
            fp.write(str(encode[(dic[i],dic[j])])+'\n')

with open('bigram_prob','w+') as fp:
    for i in range(37):
        for j in range(37):
            fp.write(str(bigram[(dic[i],dic[j])])+'\n')
