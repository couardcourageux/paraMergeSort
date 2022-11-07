import random
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-f', type=str, required=True)
parser.add_argument('-n', type=int, required=True)

args = parser.parse_args()
a, b, pathname = 100, args.n, args.f
n = random.randint(a, b)
with open(pathname, 'w') as f:
    f.write("{} ".format(n))
    for i in range(n):
        f.write(" {} ".format(random.randint(0, 300000)))