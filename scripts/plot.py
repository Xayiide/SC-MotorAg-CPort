import sys

import matplotlib.pyplot as plt

def main():
    if len(sys.argv) != 3:
        print(f"Uso: {sys.argv[0]} <filename> <output_png>")
        return

    fn = sys.argv[1]
    out = sys.argv[2]

    with open(fn, "r") as f:
        lines = f.readlines()
        valores = [float(v.strip("\n").strip(" ")) for v in lines]

        x = range(1, 2001)

        plt.plot(x, valores)

        plt.axvline(x=140, color ='b')
        plt.axhline(y=85.6, color='r')
        plt.title("Simulación")

        plt.savefig(out)

if __name__ == '__main__':
    main()