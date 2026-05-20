import numpy as np
import matplotlib.pyplot as plt

# 1. Sector Map (Heatmap)
def plot_sector_map():
    theta = np.linspace(0, 2*np.pi, 100)
    # Define throttle variation: Lower in heading (0-180), Higher in opposite (180-360)
    # Using a sine wave to represent smooth cyclic transition
    throttle_variation = 50 - 20 * np.sin(theta) 

    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    bars = ax.fill(theta, np.ones_like(theta), color='none')
    
    # Create a "sector" look using a scatter with color mapping
    sc = ax.scatter(theta, np.ones_like(theta), c=throttle_variation, cmap='coolwarm', s=2000, marker='s')
    
    ax.set_yticklabels([])
    ax.set_title("Sector Map: Throttle Intensity (Red=High, Blue=Low)", va='bottom')
    plt.colorbar(sc, label='Throttle %')
    plt.show()

# 2. Throttle vs. Angular Position (Waveform)
def plot_waveform():
    angles = np.deg2rad(np.arange(0, 361, 1))
    avg_throttle = 50
    # Modulation: subtract speed toward heading, add speed away
    cyclic_mod = 15 * np.sin(angles) 
    total_throttle = avg_throttle + cyclic_mod

    plt.figure(figsize=(10, 4))
    plt.plot(np.rad2deg(angles), total_throttle, label='Dynamic Throttle', color='red', linewidth=2)
    plt.axhline(y=avg_throttle, color='black', linestyle='--', label='Average RPM (Vertical Control)')
    
    plt.title("Throttle Modulation vs. Encoder Angle")
    plt.xlabel("Blade Angle (Degrees)")
    plt.ylabel("Throttle / PWM Command")
    plt.xticks([0, 90, 180, 270, 360])
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.show()

# 3. Polar Velocity Plot (Lift Imbalance Visualization)
def plot_velocity_profile():
    theta = np.linspace(0, 2*np.pi, 500)
    base_velocity = 100
    # The 'oval' shape showing the velocity differential
    velocity = base_velocity + 30 * np.sin(theta)

    plt.figure(figsize=(6, 6))
    ax = plt.subplot(111, projection='polar')
    ax.plot(theta, velocity, color='blue', linewidth=3, label='Resultant Velocity Profile')
    ax.fill(theta, velocity, color='blue', alpha=0.1)
    
    # Indicate Heading direction
    ax.annotate('Heading', xy=(np.pi/2, 140), xytext=(np.pi/2, 160),
                arrowprops=dict(facecolor='black', shrink=0.05))
    
    ax.set_title("Polar Velocity Profile: Shifted Lift Vector", va='bottom')
    plt.show()

# Run the plotters
plot_sector_map()
plot_waveform()
plot_velocity_profile()