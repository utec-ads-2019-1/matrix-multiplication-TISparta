import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures

threads = [1, 2, 4, 8, 16, 32]
x = []
y = []

for n_threads in threads:
    with open('./times-obtained/%d-threads.txt' %(n_threads)) as file:
        sum = 0
        cnt = 0
        for time in file:
            x.append(n_threads)
            y.append(1.0 / float(time))
            sum += float(time)
            cnt += 1
        print("Tiempo promedio cuando se uso %d hilos = %f" %(n_threads, sum / cnt))

X = np.array(x).reshape(-1, 1)
Y = np.array(y).reshape(-1, 1)
pr = PolynomialFeatures(degree = 8)
X_poly = pr.fit_transform(X)
pr.fit(X_poly, Y)
linear_regressor = LinearRegression()
linear_regressor.fit(X_poly, Y)
Y_pred = linear_regressor.predict(X_poly)

plt.scatter(X, Y)
plt.plot(X, Y_pred, color = 'red')
plt.xlabel('Threads')
plt.ylabel('Performance')
plt.xticks(threads)
plt.savefig('graphics.jpg')
