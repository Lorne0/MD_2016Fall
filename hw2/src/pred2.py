import numpy as np
import random

user_own_cats = {}
item_cats = {}
user_like_cats = {}
item_links = {}

def update(d, key, vals):
   if key not in d:
      d[key] = set()
   d[key].update(vals)

with open("message.txt") as f:
   for line in f:
      [user, item, cat, link] = map(int, line.split())
      update(user_own_cats, user, [cat])
      update(item_cats, item, [cat])
      item_links[item] = link

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
      pred = len(own.intersection(que)) > 0

      print("%d %d %d" % (user, item, pred))

