import numpy as np
with open("ans_test1_pred.num") as fp:
    for f in fp:
        a = f.split(" ")
        a=a[:-1]
print len(a)
a=np.array(a)
with open("test1/pred.num") as fp:
    for f in fp:
        b = f.split(" ")
        b=b[:-1]
print len(b)
b=np.array(b)
print sum(a==b)/float(len(a))
