import matplotlib.pyplot as plt
import numpy as np

x, y, z, a, b, c, d = np.loadtxt('datos.txt', delimiter = ',', unpack = True)
plt.plot(x, y, label='Sin sincronismo')
plt.plot(x, z, label='Pthread locks')
plt.plot(x, a, label='TAS')
plt.plot(x, b, label='TASTAS')
plt.plot(x, c, label='TASTAS con backoff')
plt.plot(x, d, label='Ticket')

plt.xlabel('Número de threads')
plt.ylabel('Tiempo')
plt.ylim([0,0.06])
plt.title('Gráfico\n Tiempo vs Threads')
plt.legend()
plt.show()
