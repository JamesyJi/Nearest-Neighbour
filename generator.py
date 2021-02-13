'''Generates and outputs nodes to a file'''

import sys
import random

file = open("dataset.txt", "w")

dim = int(input("How many dimensions?"))
for i in range(int(input("How many nodes?"))):
    node = []
    for d in range(dim):
        node.append(random.uniform(0, 100))
    
    file.write(' '.join(map(str, node)) + '\n')

file.close()

testFile = open("testset.txt", "w")

for i in range(int(input("How many test nodes?"))):
    node = []
    for d in range(dim):
        node.append(random.uniform(0, 100))

    testFile.write(' '.join(map(str, node)) + '\n')

testFile.close()
