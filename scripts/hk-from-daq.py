#!/usr/bin/python3

def main():
    filename = input("Enter .dat file name: ")

    if (filename[-4:] != ".dat"):
        filename += ".dat"

    print("Reading from data file \"" + filename + "\"...")
    print()

    with open(filename, "rb") as f:
        if (bytes.decode(f.read(10), 'utf-8') != "Unix time:"):
            print("ERR: No \"Unix time\" in data file. Corrupt?")
            return

if __name__ == "__main__":
    main()
    print()
