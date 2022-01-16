import time
import rticonnextdds_connector as rti
from time import sleep
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

client = InfluxDBClient.from_config_file("config.ini")
write_api = client.write_api(write_options=SYNCHRONOUS)

def submit_data():
    

with rti.open_connector(url="./CPS.xml", config_name="ACCParticipantLibrary::Monitoring") as connector:
    inputA = connector.get_input("Subscriber::SpeedReader")
    inputB = connector.get_input("Subscriber::DistanceReader")
    inputC = connector.get_input("Subscriber::EngineCommandReader")
    inputD = connector.get_input("Subscriber::DriverCommandReader")

    while True:
        sleep(0.5)
        inputA.take()
        for sample in inputA.samples:
            if sample.valid_data:
                point = Point("Speed")
                point.tag("id", "1")
                point.field("payload", sample[payload])
                write_api.write(bucket="cps", record=point)
        inputB.take()
        for sample in inputB.samples:
            if sample.valid_data:
                point = Point("Distance")
                point.tag("id", "1")
                point.field("payload", sample[payload])
                write_api.write(bucket="cps", record=point)
        inputC.take()
        for sample in inputC.samples:
            if sample.valid_data:
                point = Point("EngineCommand")
                point.tag("id", "1")
                point.field("payload", sample[payload])
                write_api.write(bucket="cps", record=point)
        inputD.take()
        for sample in inputC.samples:
            if sample.valid_data:
                point = Point("DriverCommand")
                point.tag("id", "1")
                point.field("payload", sample[payload])
                write_api.write(bucket="cps", record=point)
