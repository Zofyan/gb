file1 = open("/Users/sofyanaarrass/Downloads/Blargg9.txt", 'r')
file2 = open("temp_log.txt", 'r')
x = 0
while True:
    line1 = file1.readline()
    line2 = file2.readline()

    if line1 == line2:
        print(line1)
        x += 1
    else:
        print("DIFF")
        print(line1)
        print(line2)
        print(x)
        break