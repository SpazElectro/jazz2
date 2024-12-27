import cv2
import numpy as np
import logging
import time
import multiprocessing
from multiprocessing import Manager, Lock, Pool
import struct  # To handle binary packing of width and height

# Setup logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger()

# Number of worker processes for processing frames in parallel
NUM_WORKERS = multiprocessing.cpu_count()

# Lock for thread-safe writing to output file
file_lock = Lock()

def encode_video_frames(video_path, output_path, max_frames=None):
    """
    Processes each frame of the video, encodes it as black-and-white pixels,
    and writes all frames into a single file. Uses multiprocessing for faster processing.
    
    :param video_path: The path to the input video.
    :param output_path: The path to the output file.
    :param max_frames: The maximum number of frames to process (None means no limit).
    """
    logger.info(f"Starting to process video: {video_path}")
    
    # Open the video file
    cap = cv2.VideoCapture(video_path)
    
    # Check if the video was opened successfully
    if not cap.isOpened():
        logger.error(f"Failed to open video: {video_path}")
        return

    # Get video properties
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    logger.info(f"Video opened successfully. Total frames: {total_frames}, FPS: {fps}, Frame size: {frame_width}x{frame_height}")

    # If max_frames is set, ensure it doesn't exceed the total number of frames
    if max_frames and max_frames < total_frames:
        logger.info(f"Limiting to {max_frames} frames.")
        total_frames = max_frames

    # Create a manager for shared data and lock
    manager = Manager()
    shared_file = manager.list()
    
    # Create a pool of worker processes
    with Pool(processes=NUM_WORKERS) as pool:
        frame_number = 0
        frames_processed = 0
        start_time = time.time()

        # Process the frames using the pool
        while True:
            ret, frame = cap.read()
            if not ret or (max_frames and frames_processed >= max_frames):
                break  # End of video or max frames reached
            
            frame_number += 1
            frames_processed += 1

            # Process the frame using the pool (synchronously now)
            pool.apply(process_frame, args=(frame, shared_file))

            # Log FPS every 100 frames
            if frames_processed % 100 == 0:
                elapsed_time = time.time() - start_time
                fps_processing = frames_processed / elapsed_time
                logger.info(f"Processing {frames_processed}/{total_frames} frames - FPS: {fps_processing:.2f}")

        # Wait for all tasks in the pool to complete
        pool.close()
        pool.join()

        # Write the encoded frames to the output file
        with open(output_path, 'wb') as out_file:
            logger.info(f"Output file {output_path} opened for writing.")
            
            # Store frame width and height at the start of the file
            out_file.write(struct.pack('II', frame_width, frame_height))
            
            for encoded_frame in shared_file:
                with file_lock:
                    out_file.write(encoded_frame)
    
    cap.release()
    logger.info(f"Video processing completed. Encoded data saved to {output_path}")

def process_frame(frame, shared_file):
    """
    Processes a single frame: converts it to black-and-white, encodes it, and adds to shared list.
    
    :param frame: The current video frame.
    :param shared_file: The shared list for storing encoded frames.
    """
    start_time = time.time()

    # Convert frame to grayscale (black and white)
    gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Convert the frame to binary (1 for white, 0 for black)
    bw_frame = (gray_frame > 128).astype(np.uint8)  # Threshold at 128
    
    # Encode the frame as binary data
    encoded_frame = encode_pil_image_from_array(bw_frame)
    
    # Append the encoded frame to the shared list
    shared_file.append(encoded_frame)

    elapsed_time = time.time() - start_time
    logger.debug(f"Frame processed in {elapsed_time:.4f} seconds")

def encode_pil_image_from_array(bw_array):
    """
    Encodes a black-and-white numpy array into a compact binary format.
    
    :param bw_array: The black-and-white numpy array representing an image.
    :return: A bytearray containing the encoded image.
    """
    encoded_data = bytearray()

    # Process the pixels row by row
    rows, cols = bw_array.shape
    for row in bw_array:
        # Ensure the row length is a multiple of 8
        padded_row = np.pad(row, (0, 8 - len(row) % 8) if len(row) % 8 != 0 else 0, mode='constant', constant_values=0)
        
        # Process each 8-pixel group
        for i in range(0, len(padded_row), 8):
            byte = 0
            for bit in padded_row[i:i+8]:
                byte = (byte << 1) | bit
            encoded_data.append(byte)
    
    return encoded_data

if __name__ == "__main__":
    video_path = 'input.mp4'
    output_path = 'output.asvidu'
    max_frames = None  # Set the maximum number of frames to process
    encode_video_frames(video_path, output_path, max_frames)
