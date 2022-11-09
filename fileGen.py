import random
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-f', type=str, required=True)
parser.add_argument('-n', type=int, required=True)

args = parser.parse_args()
pathname =  args.f
n = args.n
with open(pathname, 'w') as f:
    f.write("{} ".format(n))
    for i in range(n):
        f.write(" {} ".format(random.randint(0, n)))
