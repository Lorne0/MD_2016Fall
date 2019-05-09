import random

with open("train.txt") as fp:
    lines = fp.readlines()
random.shuffle(lines)

K = 1212

for i in range(5):
    x = K*i
    y = K*(i+1)
    with open(str(i+1)+".train", "w") as fp:
        fp.write("".join(lines[0:x]))
        fp.write("".join(lines[y:]))

    with open(str(i+1)+".test", "w") as fp:
        fp.write("".join(lines[x:y]))
