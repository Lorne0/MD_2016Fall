import numpy as np
with open('../valid/test.num') as fp:
    for f in fp:
        a = f.split(' ')
        print len(a)
print a[-10:] 
a = a[:10000]

with open('test.num','w+') as fp:
    fp.write(' '.join(a))

with open('../valid/ans.num') as fp:
    for f in fp:
        a = f.split(' ')
        print len(a)
    
a = a[:10000]

with open('ans.num','w+') as fp:
    fp.write(' '.join(a))
