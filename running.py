import telnetlib, json, sys;

verbose = False;
if(str(sys.argv[-1]) in ["v", "-v", "-verbose", "--verbose"]):  #verbosity must be last argument
    verbose = True;
    sys.argv.pop(); #remove verbosity argument to simplify nested if statements below

if(len(sys.argv) > 1):
    if(len(sys.argv) > 2):
        tn = telnetlib.Telnet(str(sys.argv[1]), sys.argv[2]); #user inputs ip and port
    else:
        tn = telnetlib.Telnet(str(sys.argv[1]), 36330); #user inputs ip only
else:
    tn = telnetlib.Telnet("127.0.0.1", 36330); #user inputs nothing

tn.write(b"queue-info\n"); #run FAHClient command to get info

i = 0;
while(True): #reading line by line bypasses all the fluff at the beginning and end of the buffer
    line = tn.read_until(b"\n");
    #print("line:", i, line);
    
    if(b"id" in line): #TODO: case where there are multiple work units
        out = json.loads( line.strip(b"\n") ); #read info from telnet and convert to dict
    if(b"basecredit" in line): #exit if 1 work unit running TODO: does this work with >1 WU
        break;
    if(b"---" in line): #exit if no WUs running
        break;
    i+=1;

tn.close();

if("out" in locals()):
    if(verbose):
        print(out);
    else:
        print("id:", out["id"], "\npercentdone:", out["percentdone"], "\neta:", out["eta"], "\ncreditestimate:", out["creditestimate"]);
else:
    print("no work units found");
