import random
import sys

I = range(1, 101)
J = range(1, 21)
K = range(1, 41)
Z = range(1, 11)
R = range(1, 21)

MIN = 1
MAX = 5000

with open("./sample.txt", "r+") as f:
    f.write("i,j,k,z,r,cijkzr\n")
    for r in R:
        for z in Z:
            for k in K:
                for j in J:
                    for i in I:
                        f.write(f"{i},{j},{k},{z},{r},{random.randint(MIN, MAX)}\n")
    