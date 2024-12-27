import cv2
import numpy as np
import random

def generate_random_color_video(output_path, duration=5, width=640, height=480, fps=30):
    """
    Generates a 5-second video where each frame has a random color.
    
    :param output_path: The path to save the output video.
    :param duration: Duration of the video in seconds.
    :param width: Width of the video frames.
    :param height: Height of the video frames.
    :param fps: Frames per second.
    """
    # Open a VideoWriter object to save the video
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))
    
    total_frames = duration * fps
    print(f"Generating {total_frames} frames for a {duration}-second video.")

    # Generate frames with random colors
    for i in range(total_frames):
        # Create a frame with a random color
        frame = np.zeros((height, width, 3), dtype=np.uint8)
        frame[:] = [random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)]
        
        # Write the frame to the video file
        out.write(frame)

    # Release the VideoWriter object
    out.release()
    print(f"Video generated and saved to {output_path}")

if __name__ == "__main__":
    output_path = "test.mp4"
    generate_random_color_video(output_path)
