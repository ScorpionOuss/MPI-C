import matplotlib.pyplot as plt

FILE = 'out.b'
FILE2 = 'out3.b'
LENGTH = 1000

def plot(file):
    """ Plots the solution curve from out.b """
    fp = open(file, 'r')
    x = fp .read()
    x = x.split(" ")
    x.pop()
    data = [float(i) for i in x]
    plt.plot([x/LENGTH for x in range(1000)], data)
    plt.xlabel("X-axis")
    plt.ylabel("Solution")
    plt.title("Solution of the differential equation on the interval [0, 1]")
    plt.savefig('plot.png')

# plot(FILE)
plot(FILE2)