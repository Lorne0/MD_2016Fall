import random

def n2c(n):
  if n == 0:
    return ' '
  else:
    return chr(n+96)

def c2n(c):
  if c == ' ':
    return 0
  else:
    return ord(c)-96

def prob_encode():
    nums = list(range(27))
    random.shuffle(nums)
    cc = random.randint(3,5)
    dense = [ random.random() for _ in range(cc) ]
    s = sum(dense)
    dense = map(lambda p: p / s, dense)
    return list(zip(nums, dense))

def sample(table):
    r = random.random()
    s = 0
    for k, p in table:
        s += p
        if r < s:
            return k


table = [[]]

table[0].append(prob_encode())

for i in range(1, 8):
    table.append([])
    table[i].append([])
    for j in range(1, 27):
        table[i].append(prob_encode())

with open("encode.table", "w") as f:
    for i in range(8):
        for j, t in enumerate(table[i]):
            for k, _ in t:
                f.write("%d %d %d\n" % (i, j, k))

with open("test.txt", "w") as f:
    inp = open("ans.txt")
    s = inp.readline().strip()
    inp.close()
    k = 1
    for c in s:
        n = c2n(c)
        if n == 0:
            k = 0
        e = sample(table[k][n])
        f.write(n2c(e))
        k += 1
        if k > 7:
            k = 1

