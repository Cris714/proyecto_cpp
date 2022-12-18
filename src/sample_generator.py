import random
import sys

I = range(1, 101)
J = range(1, 21)
K = range(1, 41)

P = [1,2,3,4]
R = [5,6,7,8,9,10,13,14]
Z = [1,2,4,5,6,7,9]
S = [1,2,3,4]
M = [1,2,3,7,8,9,10,11,12]
C = [[],[1,11,12],[2,11,12],[3,11,12],[4,11,12],[5,11,12],
     [6,11,12],[7,11,12],[9,11,12],[9,11,12],[10,11,12],
     [1,2,3,4,5,6,7,9,11,12],[1,2,3,4,5,6,7,9,11,12]]

MIN = 1
MAX = 5000

with open("../data/supMaximaCambio.csv", "w+") as f:
    # f.write("i,j,k,z,r,cijkzr\n")
    # for r in R:
    #     for z in Z:
    #         for k in K:
    #             for j in J:
    #                 for i in I:
    #                     f.write(f"{i},{j},{k},{z},{r},{random.randint(MIN, MAX)}\n")
    # f.write("region,zona,manejo,superficie\n")
    # for r in R:
    #     for z in Z:
    #         for m in M:
    #             f.write(f"{r},{z},{m},0\n")
    f.write("propietario,region,zona,clase,manejo,cambio,superficie\n")
    for p in P:
        for r in R:
            for z in Z:
                for s in S:
                    for m in M:
                        for c in C[m]:
                            f.write(f"{p},{r},{z},{s},{m},{c},0\n")
    