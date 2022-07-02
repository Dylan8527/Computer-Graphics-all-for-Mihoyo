import json

def swapAxis(matrix: list, axis1: int, axis2: int):
    for i in range(3):
        matrix[i][axis1], matrix[i][axis2] = matrix[i][axis2], matrix[i][axis1]

    return matrix

with open("transform.txt") as f:
    data = f.readlines()
width = 2.0 / list(map(float, data[2].replace("[","").replace("]","").replace(",","").split()))[0]
height = 2.0 / list(map(float, data[3].replace("[","").replace("]","").replace(",","").split()))[1]


dictionary = {
    "fl_x": width,
    "fl_y": height,
    "cx": 960.0,
    "cy": 540.0,
    "w": 1920.0,
    "h": 1080.0,
    "aabb_scale": 2,
    "frames": []
}

start = 8
step = 8
for i in range(start, len(data), step):
    index = int(data[i].replace(":", ""))
    matrix = []
    for j in range(i+2,i+6):
        row = map(float, data[j].replace("[","").replace("]","").replace(",","").split())
        matrix.append(list(row))
    # matrix = swapAxis(matrix, 1, 2)
    dictionary["frames"].append({
        "file_path": "images/{:04d}.png".format(index),
        # "sharpness": 31.752987436300323,
        "transform_matrix": matrix
    })

result = json.dumps(dictionary, indent=2)
with open("../data/nerf/earth/transform.json", 'w') as f:
    f.write(result)

