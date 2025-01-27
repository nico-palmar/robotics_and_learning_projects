import cv2


def get_vid_time(f_name, n_rev, gear_ratio=35, base_path="/home/nico/Downloads/"):
    # read video from file
    cap = cv2.VideoCapture(base_path + f_name)
    video_fps = cap.get(cv2.CAP_PROP_FPS)
    total_frames = cap.get(cv2.CAP_PROP_FRAME_COUNT)
    video_sec = total_frames / video_fps
    return (gear_ratio * n_rev) / video_sec

print(get_vid_time("IMG_0476.MOV", 7))
print(get_vid_time("IMG_0479.MOV", 4))
print(get_vid_time("IMG_0482.MOV", 4))
print(get_vid_time("IMG_0486.MOV", 9))

