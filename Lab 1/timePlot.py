import numpy as np
import matplotlib.pyplot as plt

time = []
num = ''

timeFile = open('time.txt', 'r')
data = timeFile.read()

# Парсим
for char in data:
    if char != ' ':
        num += char
    else:
        time.append(float(num))
        num = ''

num_proc = [i for i in range(1,len(time)+1)]

# Строим
plt.plot(num_proc, time)
plt.show()
