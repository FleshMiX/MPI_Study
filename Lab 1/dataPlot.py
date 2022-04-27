import numpy as np
import matplotlib.pyplot as plt

z = []
num = ''
dataFile = open('data.txt', 'r')
data = dataFile.read()

# Парсим
for char in data:
    if char != ' ':
        num += char
    else:
        z.append(float(num))
        num = ''

t = [[i for i in range(1,10001)] for i in range(10000)]
x = [[i]*10000 for i in range(1,10001)]
z = np.array(z)
z = z.reshape((10000,10000))

# Строим
fig = plt.figure(figsize=(7, 4))
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(x, t, z, cmap='plasma')
plt.show()
