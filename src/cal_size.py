import sys
# f = open(argv[2],"r")
s = 0
p = 0
# line = f.readlines
with open(sys.argv[1],"r") as fp:
    for line in fp:
        # print(line)
        line_sp = line.split()
        if( len(line_sp) > 0):
            if(line_sp[0] == "assign"):
                if(len(line_sp) == 6):
                    s += 1
                elif(len(line_sp) == 4):
                    p += 1
print("circuit size is ",s,"\nbuffer and not gate size is",p)
            