import time
import rticonnextdds_connector as rti
from time import sleep

print("Driver input is up")
with rti.open_connector(url="./CPS.xml", config_name="ACCParticipantLibrary::Driver") as connector:
    output = connector.get_output("Publisher::DriverCommandWriter")

    while True:
        output.instance.set_dictionary({"payload": "On"})
        output.write()
        sleep(30)
        output.instance.set_dictionary({"payload": "Off"})
        output.write()
        sleep(5)
