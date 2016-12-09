import random
with open('ENTRADA2','r') as source:
    first_line = source.readline()
    data = [ (random.random(), line) for line in source ]
data.sort()
with open('ShuffledInput','w') as target:
    target.write(first_line)
    for _, line in data:
        target.write( line )