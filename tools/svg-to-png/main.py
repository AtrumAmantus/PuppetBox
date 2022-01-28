import glob, os, sys
from svglib.svglib import svg2rlg
from reportlab.graphics import renderPM


def main():
    if len(sys.argv) > 1:
        _, directory = sys.argv
    else:
        directory = '.'

    os.chdir(directory)
    for file in glob.glob('*.svg'):
        convert(file)


def convert(file):
    svg_code = svg2rlg(file)
    file_name = os.path.splitext(file)[0]
    renderPM.drawToFile(svg_code, f'{file_name}.png', fmt='PNG')


if __name__ == '__main__':
    main()
