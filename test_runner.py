import subprocess
import sys
import os
import time

def runCommand(command):
    command = subprocess.run(command, shell=True, capture_output=True, text=True)
    return command.stdout.strip()

def main():
    if len(sys.argv)<2:
        print("TOO SMALL")
        sys.exit(1)

    os.system("./sendFile")
    LocalArgs = "wonderland.txt 127.0.0.1:50570"
    serverArgs = "wonderland.txt 131.104.48.82:50570"
    print("Running The Makefile")
    runCommand("make")
    for i in range (20):
        print(f"Running iteration {i+1}/20")
        if (sys.argv[1]=="Local"):
            localCommand = "./sendFile " + LocalArgs
            startTime = time.time()
            runCommand(localCommand)
            endTime = time.time()
            print(endTime - startTime)
        elif (sys.argv[1]=="Server"):
            localCommand = "./sendFile " + serverArgs
            startTime = time.time()
            runCommand(localCommand)
            endTime = time.time()
            print(endTime - startTime)

if __name__ == "__main__":
    main()
