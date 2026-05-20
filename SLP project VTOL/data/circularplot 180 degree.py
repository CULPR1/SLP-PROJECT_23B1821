import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Load your data from image_ce21e9.jpg context
df = pd.read_csv('180_degree_50_%.csv')

# Convert degrees to radians for the plot
theta = np.deg2rad(df['Encoder'])
r = df['Actual_output']

fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
ax.plot(theta, r, label='Motor Output Speed')
ax.set_theta_zero_location("N") # Sets 0 degrees to the top
ax.set_theta_direction(-1)      # Matches clockwise rotation if needed
plt.legend()
plt.show()