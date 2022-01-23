import os
import sys
import zipfile

args = sys.argv


def zip_dir(path, ziph):
    for root, dirs, files in os.walk(path):
        for file in files:
            if file != os.path.basename(__file__) and file != 'Assets1.zip':
                filename = os.path.join(root, file)
                ziph.write(
                    filename,
                    os.path.relpath(filename, os.path.join(path, '.'))
                )


sourceDir = '.'


if len(args) > 1:
    sourceDir = args[1]


with zipfile.ZipFile('Assets1.zip', 'w', zipfile.ZIP_DEFLATED) as zipf:
    zip_dir(sourceDir, zipf)