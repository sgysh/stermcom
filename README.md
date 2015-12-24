#stermcom
terminal emulator

by Yoshinori Sugino

---

##License
MIT

---

##How to install

    make
    sudo make install
    
If you do not have permissions to access the device node, you might want to add yourself to the dialout group.

    sudo adduser $USER dialout

##How to uninstall

    sudo make uninstall

---

##Usage

    stermcom baud_rate device_node

Type Ctrl-x to exit this program

####Piping

    echo "command" | stermcom baud_rate device_node

####Redirection

    stermcom baud_rate device_node < commands.txt

