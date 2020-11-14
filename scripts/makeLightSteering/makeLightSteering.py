import math
import glob, os
#Define Positions

#LED Points MotherShip
#x=[32.66906094  , 7.69273256   , 37, 22 , 17.79837388, 1.91742634, -2.862134931, -13.48191069, -14.91782843, -36.79731013, -30.67439018, -5.788075206 ]
#y=[17.37044782  ,  36.19146123 ,  0,  0 , 12.93127555, 21.91628336, -14.72440775, -6.575567202,  -1.567926949,  -3.867553141,  -20.69013743 , -36.5444686]
#x=[32.66906094,-5.788075206]
#y=[17.37044782,-36.5444686]


#10 atm MotherShip
#x=[0,37,28.3436444,3.867553141,-12.6547453,-4.574057198,-0.3839529416,17.56998122,22,-15,-10.79009701,0.7850393436,-1.046347106,-7.69273256,-3.224762482,-29.15639788,-36.99436472]
#y=[0,0,23.78314156,36.79731013,34.76862697,21.51924722, 21.99664929,13.23993051,0,0,-10.41987556,-14.97944302,-14.96346075,-36.19146123,-36.85920383, -22.77947459, -0.6457390382]

x=[13,10,10,10,5,5,5,0,0,0,-5,-5,-5,-9,-9]
y=[0 ,0 ,-7, 7,0,-7,7,7,0,-7,-7,0,7,7,0]

#Define Event NUmbers
Events=1000
NumberPhotons=50000
IncrementofPhotons=0
#Define FilePaths
#FileName="/media/ilker/DATA/Project/sipm_wheel/config/IntesityBigDisk"
#FileName="/media/ilker/DATA/Project/sipm_wheel/config/IntesityBigDiskCf252"
FileName="/home/ilker/Projects/sipm_wheel/config/Nov_21_2019_LEDPeak"
FReadName="" #"/home/ilker/Desktop/Analysis/Second_Data/"

FileCount=0
count=0
Files=[]
File=[]
Title=[]


# Assuming picking up true files that has truePoints has not corrected
#CenterX=-3
#CenterY=8
CenterX=0
CenterY=0

def IncrementLoop():
    f.write("x y n\n")
    for i in range(0,len(x)):
        NumberPhotons=0
    for k in range(0,Events):
        NumberPhotons+=IncrementofPhotons;
        f.write(str(x[i]) + " " + str(y[i]) + " " + str(NumberPhotons) +"\n")

def RegularLoop():
    f.write("x y n\n")
    for i in range(0,len(x)):
        for k in range(0,Events):
            f.write(str(x[i]) + " " + str(y[i]) + " " + str(NumberPhotons) +"\n")


if(FReadName):
    os.chdir(FReadName)
    print ("Possible Files:")
    print ("Index   File Name")
    for file in glob.glob("*.txt"):
        print(str(FileCount) + "--> " + file)
        Files.append(file)
        FileCount += 1

    Index = int(input("Enter the Index: "))
    Name = Files[Index]
    Fread = open(Name, "r")
    x=[]
    y=[]
    # get tFhe true positions
    for line in Fread:
        ReadLine = line.strip("\n").split("\t")
        if (count == 0):
            Title.extend(ReadLine)
        else:
            if len(ReadLine) > 2:
                File.append(ReadLine[0])
                x.append(int(ReadLine[1]) - CenterX)
                y.append(int(ReadLine[2]) - CenterY)
            else:
                continue

        count += 1
    Fread.close()

f=open(FileName,"w+")
print ("Starting to create file Named " + FileName)

if (IncrementofPhotons==0):
    RegularLoop()
else:
    IncrementLoop()


print ("Done!")
f.close()