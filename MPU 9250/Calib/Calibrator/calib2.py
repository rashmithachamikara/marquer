import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def load_data(filename):
    """Load raw magnetic data from a file."""
    data = np.loadtxt(filename, delimiter=',')
    return data

def calibrate_magnetometer(data):
    """Calibrate the magnetometer using raw magnetic data."""
    # Extract raw magnetic data
    raw_x = data[:, 0]
    raw_y = data[:, 1]
    raw_z = data[:, 2]

    # Implement a more sophisticated calibration algorithm
    # For example, you can use the ellipsoid fitting method or the least squares method

    # Calculate calibration parameters
    bias_x = np.mean(raw_x)
    bias_y = np.mean(raw_y)
    bias_z = np.mean(raw_z)

    # You can add more advanced calibration here
    
    # Return the calibration parameters
    return bias_x, bias_y, bias_z

def visualize_calibration(data, bias_x, bias_y, bias_z):
    """Visualize the raw and calibrated magnetic data."""
    raw_x = data[:, 0]
    raw_y = data[:, 1]
    raw_z = data[:, 2]

    # Plot raw data
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(raw_x, raw_y, raw_z, color='b', label='Raw Data')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.legend()
    ax.set_title('Raw Magnetic Data')
    plt.show()

    # Plot calibrated data
    calibrated_x = raw_x - bias_x
    calibrated_y = raw_y - bias_y
    calibrated_z = raw_z - bias_z

    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(calibrated_x, calibrated_y, calibrated_z, color='r', label='Calibrated Data')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.legend()
    ax.set_title('Calibrated Magnetic Data')
    plt.show()

def main():
    filename = 'data.txt'  # Change this to the name of your data file
    data = load_data(filename)
    bias_x, bias_y, bias_z = calibrate_magnetometer(data)
    print("Calibration parameters:")
    print("Bias X:", bias_x)
    print("Bias Y:", bias_y)
    print("Bias Z:", bias_z)

    visualize_calibration(data, bias_x, bias_y, bias_z)

if __name__ == "__main__":
    main()
