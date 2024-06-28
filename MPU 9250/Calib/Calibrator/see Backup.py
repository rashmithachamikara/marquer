import serial
import matplotlib.pyplot as plt

def read_serial(port='COM6', baudrate=115200, timeout=1):
    try:
        # Open the serial port
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            print(f"Monitoring serial input on {port} at {baudrate} baud rate.")
            
            # Initialize lists to store X, Y, Z values
            x_values, y_values, z_values = [], [], []
            
            plt.figure()
            plt.xlabel('X')
            plt.ylabel('Y')
            plt.title('Real-time Magnetic Field Visualization')
            
            plt.ion()  # Turn on interactive mode
            
            while True:
                if ser.in_waiting > 0:
                    # Read a line from the serial input
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        # Split the line into X, Y, Z values
                        values = line.split(',')
                        if len(values) == 3:
                            try:
                                x, y, z = map(float, values)
                            except ValueError:
                                print("Invalid input:", line)
                                continue
                            
                            # Append the values to the respective lists
                            x_values.append(x)
                            y_values.append(y)
                            z_values.append(z)
                            
                            # Clear the plot
                            plt.clf()
                            
                            # Plot X vs Y
                            plt.scatter(x_values, y_values, c='r', label='XY Plane')
                            
                            # Plot Y vs Z
                            plt.scatter(y_values, z_values, c='g', label='YZ Plane')
                            
                            # Plot Z vs X
                            plt.scatter(z_values, x_values, c='b', label='ZX Plane')
                            
                            plt.legend()
                            plt.draw()
                            plt.pause(0.01)  # Pause to allow the plot to update
                        else:
                            print("Invalid input:", line)
                            
    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("Monitoring stopped by user.")

if __name__ == "__main__":
    read_serial()
