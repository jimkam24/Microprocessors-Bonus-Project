import serial
import tensorflow as tf
import numpy as np
import time
import struct  # For packing float values

debugging = 1

def load_mnist():
    (train_images, train_labels), (test_images, test_labels) = tf.keras.datasets.mnist.load_data()
    return test_images, test_labels

def send_image_int(serial_port, image_data):
    image_data = image_data.astype(np.uint8)  # Convert to uint8
    packed_data = struct.pack('<' + 'B' * 784, *image_data.flatten())  # Send as bytes
    serial_port.write(packed_data)

def send_image_float(serial_port, image_data):
    image_data = image_data.astype(np.float32)
    packed_data = struct.pack('<' + 'f' * 784, *image_data.flatten())  # little-endian !!!
    serial_port.write(packed_data)
    
def init_uart(port, baudrate=115200):
    try:
        ser = serial.Serial(port, baudrate, timeout=5)  
        time.sleep(2)
        print(f"Connected to {port} at {baudrate} baud.")
        return ser
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return None

def receive_prediction(serial_port):
    response = serial_port.readline().decode().strip()  # Read from UART
    return response

def normalize_image(image_data):
    image_data = image_data.astype(np.float32) / 255.0 # normalize to [0, 1]
    return image_data


# =========== Main =========== #

test_images, test_labels = load_mnist()

serial_port = init_uart("COM57")

counter = 0
pred_timer = 0
recv_timer = 0
send_timer = 0
if serial_port:
    print("Waiting for STM32 to be ready...")

    limit = 100
    for i in range(1000, 1000+limit):

        while True:
            
            ready_msg = serial_port.readline().decode().strip()
            
            start_send_timer = time.time()
            
            if ready_msg == "Ready for input":
                if debugging:
                    print("STM32 is ready!")
                break
                
        image = test_images[i]
        label = test_labels[i]

        # use for non quantized
        # normalized_image = normalize_image(image)
                
        # send_image_float(serial_port, normalized_image)
        
        # use for quantized
        send_image_int(serial_port, image)
        
        if debugging:
            print(f"Sent image with label: {label}")

        while True:
            ready_msg = serial_port.readline().decode().strip()
            if ready_msg == "Image received":
                if debugging:
                    print(ready_msg)
                break
        
        send_timer += time.time() - start_send_timer

        start_pred_timer = time.time()
        prediction = receive_prediction(serial_port)
        
        if debugging:
            print(f"STM32 Predicted: {prediction}")
        if (int(prediction) == label):
            counter = counter + 1
        pred_timer += time.time() - start_pred_timer
        

print(f"Correct predictions: {counter} / {limit}")
print(f"Accuracy = {100*(counter/limit)} %")
print(f"Mean Time for a prediction: {pred_timer/limit}")
print(f"Mean Time to send image {send_timer/limit}")

serial_port.close()