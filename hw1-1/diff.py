with open('pred.txt') as fp:
    for f in fp:
        pred = f

with open('pred_c.txt') as fp:
    for f in fp:
        ans = f

print len(pred)
print len(ans)

cnt=0
for i in range(len(pred)):
    if pred[i]!=ans[i]:
        print i
        cnt+=1

print cnt
