import time
import rticonnextdds_connector as rti
from time import sleep

enabled = False

with rti.open_connector(url="./CPS.xml", config_name="ACCParticipantLibrary::McuCentral") as connector:
    output = connector.get_output("Publisher::EngineCommandWriter")
    inputA = connector.get_input("Subscriber::SpeedReader")
    inputB = connector.get_input("Subscriber::DistanceReader")
    inputD = connector.get_input("Subscriber::DriverCommandReader")

    while True:
        sleep(1)
        speed = -1
        dst = -1.0
        inputA.take()
        for sample in inputA.samples:
            if sample.valid_data:
                speed = int(sample["payload"])
        inputB.take()
        for sample in inputB.samples:
            if sample.valid_data:
                dst = float(sample["payload"])
        inputD.take()
        for sample in inputD.samples:
            if sample.valid_data:
                if(sample["payload"] == "On"):
                    enabled = True
                elif(sample["payload"] == "Off"):
                    enabled = False
        if(speed < 0 or dst < 0):
            continue
                    
        print("Got data, calculating command..")
        if(enabled and speed > 80):
            output.instance.set_dictionary({"payload": "Decrease"})
            output.write()
            print("Decrease")
        elif(enabled and dst < 1.0):
            output.instance.set_dictionary({"payload": "Decrease"})
            output.write()
            print("Decrease")
        elif(enabled and speed < 80):
            output.instance.set_dictionary({"payload": "Increase"})
            output.write()
            print("Increase")
        elif(enabled and speed == 80):
            output.instance.set_dictionary({"payload": "Hold"})
            output.write()
            print("Hold")
        else:
            output.instance.set_dictionary({"payload": "Off"})
            output.write()
            print("Off")
