
import numpy as np
import matplotlib.pyplot as plt

nis_radar_values = open("/home/a3/nis_radar.txt","r")
nis_radar=[]
for val in nis_radar_values:
    #read line-->ignore '\n' character--> convert to float type
    nis_radar.append(np.float(val[:-1])) 

nis_lidar_values = open("/home/a3/nis_lid.txt","r")
nis_lidar=[]
for val in nis_lidar_values:
    nis_lidar.append(np.float(val[:-1]))

print('# of radar measurements: ',len(nis_radar))
print('# of lidar measurements: ',len(nis_lidar))

x_lidar = [i for i in range(len(nis_lidar))]
x_radar = [i for i in range(len(nis_radar))]

nis_95_percent = [7.815 for i in range(len(nis_lidar))]
nis_05_percent = [0.352 for i in range(len(nis_lidar))]
## plot the NIS values for lidar and radar
plt.figure()
plt.title('NIS lidar')
plt.xlabel('k')
plt.ylabel(r'$\epsilon$')
plt.plot(x_lidar, nis_05_percent,'r--',x_lidar, nis_95_percent,'C1--', x_lidar,nis_lidar,'C2')
plt.legend([r'5% line (0.352)',r'95% line (7.815)',r'NIS_lidar'])

plt.figure()
plt.title('NIS radar')
plt.xlabel('k')
plt.ylabel(r'$\epsilon$')
plt.plot(x_lidar, nis_05_percent,'r--',x_lidar, nis_95_percent,'C1--',x_radar, nis_radar,'C2')
plt.legend([r'5% line(0.352)',r'95% line (7.815)',r'NIS_radar'])

plt.show()

