#python script to display folding @ home stats
import sys, os, requests, time;
#time.sleep(10);
from PyQt5 import QtWidgets, QtCore, QtGui;
import setproctitle;
setproctitle.setproctitle("FAHStats_py");
f = open("/home/yobleck/fah/stats_log.txt","a"); f.write("\n"+str(time.ctime()) + " python modules loaded"); f.close();
#TODO: add change over time?
def get_stats():
    try:
        source = requests.get("https://api.foldingathome.org/user/yobleck"); #https://api.foldingathome.org/
        html = "";
        if(source.status_code == requests.codes.ok):
            html = source.json();
            name = html["name"];
            score = html["score"];
            wus = html["wus"];
            rank = html["rank"];
            
            user_count = requests.get("https://api.foldingathome.org/user-count").text;
        
            #timer code         + str(time.time())[0:11] + "\n" +\
            return  name + "\n" \
                    "Score: " + f"{score:,}" + "\n" + \
                    "WUs: " + f"{wus:,}" + "\n" + \
                    "Rank: " + f"{rank:,}" + " / " + f"{int(user_count):,}" + "\n" + \
                    "Top: " + str((rank/int(user_count))*100)[0:4] + "%"
        else:
            f = open("/home/yobleck/fah/stats_log.txt","a"); f.write("\n"+str(time.ctime()) + " error code: "+str(source.status_code));f.close();
            return "Error code: " + str(source.status_code);
    except:# Exception as e:
        f = open("/home/yobleck/fah/stats_log.txt","a"); f.write("\n"+str(time.ctime()) + " something went wrong"); f.close();
        return "something went wrong"; #\nError: " + str(source.status_code) + "\n" + str(e);
            

def do_thing():
    app = QtWidgets.QApplication(["FAHStats"]);
    win = QtWidgets.QMainWindow();
    
    win.setAttribute(QtCore.Qt.WA_ShowWithoutActivating); #stops new window from stealing focus
    win.setFocusPolicy(QtCore.Qt.NoFocus); #does this do anything?
    win.setWindowFlags(QtCore.Qt.WindowStaysOnBottomHint | QtCore.Qt.FramelessWindowHint  | QtCore.Qt.Tool); #window always behind,no title bar/taskbar icon
    win.resize(155, 100);
    win.move(0,1316);

    label = QtWidgets.QLabel(win); #label in window
    label.resize(155,100);
    label.setAlignment(QtCore.Qt.AlignTop | QtCore.Qt.AlignLeft);
    #label.setStyleSheet("background-color:black;"); #doesn't work with qtile
    
    label.setText(get_stats()); #initially get stats to be displayed
    def refresh(): #function to do above repeatedly
        label.setText(get_stats());
    
    timer = QtCore.QTimer(); #timer that refreshes label
    timer.timeout.connect(refresh);
    timer.start(600000); #milliseconds
    
    #actually show everything on screen
    win.show();
    label.show();
    sys.exit(app.exec_());


do_thing();
