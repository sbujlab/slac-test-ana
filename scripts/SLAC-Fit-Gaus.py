import numpy as np
from scipy.optimize import curve_fit as fit
import matplotlib.pyplot as plt

def gauss(x, mu, sig, b, z):
    return b*(1/(sig*2*np.pi)) * np.exp(-.5*((x-mu)/sig)**2) + z

def x2(xdata, mu, b, z):
    return -b*(xdata-mu)**2 + z

#plotType = 'gauss'
plotType = 'x2'


xdata = np.array([-1.5, 0, 1.5,3.0])
ydata = np.array([1649/100,1683/100,1682/100,1653/100])


if plotType=='gauss':
    popt, pcov = fit(gauss, xdata, ydata, bounds=([1, 0, 0, 50], [3, 10, np.inf, 200]))
else:
    popt, pcov = fit(x2, xdata, ydata)

xspace = np.linspace(-3, 6, 1000)

fig, ax = plt.subplots(1,1)
if plotType=='gauss':
    ax.plot(xspace, gauss(xspace, popt[0], popt[1], popt[2], popt[3]), label='best fit gaussian')
else:
    ax.plot(xspace, x2(xspace, popt[0], popt[1], popt[2]), label='best fit parabola')
ax.plot(xdata, ydata, 'o', label='measured data')
ax.set_xlabel('theta (degrees)')
ax.set_ylabel('ADC Channel counts')
ax.set_title('ADC Channels as a function of angle')
ax.axvline(x=popt[0], label='gaussian fit center = {:.2f} degrees'.format(popt[0]), color='m')
ax.legend()
fig.show()

print(popt)

input('press enter to finish script')
