import numpy as np
from scipy.optimize import least_squares
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def normalize_data(data):
    norms = np.linalg.norm(data, axis=1)
    return data / norms[:, np.newaxis]

def ellipsoid_residuals(params, data):
    a, b, c, d, e, f, g, h, i = params
    x, y, z = data.T
    return a*x**2 + b*y**2 + c*z**2 + 2*d*x*y + 2*e*x*z + 2*f*y*z + 2*g*x + 2*h*y + 2*i*z - 1

def calibrate_magnetometer(data):
    # Normalize data
    normalized_data = normalize_data(data)

    # Fit an ellipsoid
    initial_guess = np.ones(9)  # Initial guess for ellipsoid parameters
    params = least_squares(ellipsoid_residuals, initial_guess, args=(normalized_data,))
    ellipsoid_params = params.x

    # Extract calibration parameters
    offset = -ellipsoid_params[6:9] / ellipsoid_params[:3]
    scaling = np.sqrt(1 / ellipsoid_params[:3])

    return offset, scaling

# Load magnetometer data from file
data = np.loadtxt("data.txt", delimiter=",")

# Calibrate magnetometer
offset, scaling = calibrate_magnetometer(data)

# Apply calibration
calibrated_data = (data - offset) * scaling

# Plot raw and calibrated data
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Raw data
ax.scatter(data[:, 0], data[:, 1], data[:, 2], c='b', label='Raw Data')

# Calibrated data
ax.scatter(calibrated_data[:, 0], calibrated_data[:, 1], calibrated_data[:, 2], c='r', label='Calibrated Data')

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('Magnetometer Calibration')
ax.legend()

plt.show()

print("Offset:", offset)
print("Scaling:", scaling)
