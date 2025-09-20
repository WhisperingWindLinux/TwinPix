import os
import sys
import cv2
import numpy as np


def parse_color(color_name):
    colors = {
        "black": (0, 0, 0),
        "red": (0, 0, 255),
        "green": (0, 255, 0),
        "blue": (255, 0, 0),
        "yellow": (0, 255, 255),
        "cyan": (255, 255, 0),
        "magenta": (255, 0, 255),
        "white": (255, 255, 255),
        "orange": (0, 165, 255),
        "purple": (128, 0, 128),
        "lime": (0, 255, 128),
    }
    return colors.get(color_name.lower(), (0, 0, 0))


def compare_images(img1, img2, color, opacity=0.6):
    # Ensure both images are of the same size
    if img1.shape != img2.shape:
        raise ValueError("Images must have the same dimensions")

    # Convert images to BGRA
    img1_bgra = cv2.cvtColor(img1, cv2.COLOR_BGR2BGRA)
    img2_bgra = cv2.cvtColor(img2, cv2.COLOR_BGR2BGRA)

    # Create a transparent copy of the first image
    result_img = img1_bgra.copy()
    result_img[:, :, 3] = (opacity * 255)

    # Find the differences between the two images
    diff_mask = np.any(img1 != img2, axis=-1)  # Mask of differing pixels

    # Apply the specified color to the differing pixels
    color_with_alpha = (*color, 255)
    result_img[diff_mask] = color_with_alpha

    return result_img


def main():
    if len(sys.argv) != 5:
        print("Usage: script.py <name1> <name2> <color> <show_image: Yes/No>", file=sys.stderr)
        sys.exit(1)

    name1 = sys.argv[1]
    name2 = sys.argv[2]
    color_name = sys.argv[3]
    show_image = sys.argv[4].lower()

    if show_image not in ["yes", "no"]:
        print("Invalid value for <show_image>. Use 'Yes' or 'No'.", file=sys.stderr)
        sys.exit(1)

    try:
        # Get the color to use for highlighting differences
        selected_color = parse_color(color_name)

        my_variable = os.getenv("Runner")
        if my_variable == "TwinPix":
            # Read the first image's size and data from stdin
            size1 = int.from_bytes(sys.stdin.buffer.read(4), byteorder='little')
            image1_data = sys.stdin.buffer.read(size1)
            # Read the second image's size and data from stdin
            size2 = int.from_bytes(sys.stdin.buffer.read(4), byteorder='little')
            image2_data = sys.stdin.buffer.read(size2)
            # Decode the images from the binary data
            image1 = cv2.imdecode(np.frombuffer(image1_data, np.uint8), cv2.IMREAD_COLOR)
            image2 = cv2.imdecode(np.frombuffer(image2_data, np.uint8), cv2.IMREAD_COLOR)
        else:
            image1 = cv2.imread(name1, cv2.IMREAD_COLOR)
            image2 = cv2.imread(name2, cv2.IMREAD_COLOR)

        if image1 is None or image2 is None:
            print("Failed to decode images", file=sys.stderr)
            sys.exit(1)

        if image1.shape != image2.shape:
            print("Images must have the same dimensions", file=sys.stderr)
            sys.exit(1)

        # Compare the images and get the result
        result_image = compare_images(image1, image2, selected_color)

        if show_image == "yes":
            # Encode the resulting image to PNG format
            _, encoded_image = cv2.imencode('.png', result_image)
            # Write the encoded image to stdout
            if my_variable == "TwinPix":
                sys.stdout.buffer.write(encoded_image.tobytes())
            else:
                cv2.imshow("Differences", result_image)
                cv2.waitKey(0)
                cv2.destroyAllWindows()
        else:  # Show with white background
            white_background = np.full_like(image1, fill_value=255)  # Create a white background
            diff_mask = np.any(image1 != image2, axis=-1)  # Mask of differing pixels
            for i in range(3):  # Apply selected color to differences on white background
                white_background[:, :, i] = np.where(diff_mask, selected_color[i], white_background[:, :, i])
            result_image = white_background

            # Encode the resulting image to PNG format
            _, encoded_image = cv2.imencode('.png', result_image)
            # Write the encoded image to stdout
            if my_variable == "TwinPix":
                sys.stdout.buffer.write(encoded_image.tobytes())
            else:
                cv2.imshow("Differences on White Background", result_image)
                cv2.waitKey(0)
                cv2.destroyAllWindows()
    except Exception as e:
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
