import numpy as np
import random

user_own_cats = {}
item_cats = {}
user_like_cats = {}
item_links = {}
cat_items = {}
cat_probs = {}

def sample(prob):
    return sum(np.random.binomial(1, prob, 514))

def update(d, key, vals):
   if key not in d:
      d[key] = set()
   d[key].update(vals)

with open("message.txt") as f:
   for line in f:
      [user, item, cat, link] = map(int, line.split())
      update(user_own_cats, user, [cat])
      update(item_cats, item, [cat])
      update(cat_items, cat, [item])
      item_links[item] = link

cat_likes = {}
for cat in cat_items:
    cat_likes[cat] = sum(map(lambda i: item_links[i], cat_items[cat]))

cat_lens = list(map(len, cat_items.values()))
a = min(cat_lens)
b = max(cat_lens)

for cat in cat_items:
    cat_probs[cat] = 0.9 - 0.8 * (len(cat_items[cat]) - a) / (b - a)

with open("relation.txt") as f:
   for line in f:
      [u1, u2] = map(int, line.split())
      update(user_like_cats, u1, user_own_cats.get(u2, set()))
      update(user_like_cats, u2, user_own_cats.get(u1, set()))

with open("pred.id") as f:
   for line in f:
      l = line.split()
      user = int(l[0])
      item = int(l[1])
      own = user_own_cats.get(user, set()).union(user_like_cats.get(user, set()))
      que = item_cats.get(item, set())
      cocat = own.intersection(que)
      vote = sum(map(lambda cat: sample(cat_probs[cat]), cocat))
      pred = vote > (257 * len(cocat))

      print("%d %d %d" % (user, item, pred))

