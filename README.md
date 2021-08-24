# Folding @ Home Stats
View folding @ home stats on desktop

edit source = requests.get("https ://api.foldingathome.org/user/your_username_here") to get your stats

edit win.move() to change screen coordinates

edit win.resize() and label.resize() to change size

ctrl+\ to exit

# running.py 
get info about current work unit(s) status via telnet
## usage:
$ python running.py ip port v

    -ip address (if not given then defaults to 127.0.0.1)
    -port must follow ip address (if not given defaults to 36330)
    -v is verbose output (-v, -verbose and --verbose also work) must be last argument

outputs id, percentage, eta and points by default. vebose dumps all info as json object

TODO: depends on telnetlib whic is neing deprecated. Replace with sockets. See FAHControl code for reference.
