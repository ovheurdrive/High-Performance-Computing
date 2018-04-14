import sys
import random

size = int(sys.argv[1])
with open("metadata_t.txt", "w") as f0:
  f0.write(str(size) + ' ' + str(size))


with open("matrix_t.txt", "w") as f1:
  for i in range(size):
    for j in range(size):
      if i == j:
        f1.write(str(random.randint(100,150)) + ' ')
      elif abs(i-j) < 10:
        f1.write(str(random.randint(-2,2)) + ' ')
      else:
        f1.write('0' + ' ')
    f1.write('\n')

with open("vector_t.txt", "w") as f2:
  for i in range(size):
    f2.write(str(random.randint(0,6)))
    f2.write("\n")

