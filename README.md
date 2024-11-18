**HOW TO RUN**


1. Run Makefile

```
  make
```

2a. Run Client:

```  
  ./sendFile FILENAME IPAddress:PortNum [BufferSize]
```

2b. At the same time, in a different terminal, run Server:

```
  ./server PortNum
```


3. To run Python Testing, when the server is running, run the following command
   
     a. For Local Test
     ```
       python3 test_runner.py Local
     ```
     b. For Server Test
     ```
        python3 test_runner.py Server
     ```
     
     
