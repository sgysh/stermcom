#stermcom
terminal emulator

by Yoshinori Sugino

---

##License
MIT

---

##Usage

    ./stermcom baud_rate device_node

Type Ctrl-x to exit this program

####Piping

    echo "command" | ./stermcom baud_rate device_node

####Redirection

    ./stermcom baud_rate device_node < commands.txt

